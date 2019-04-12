/*!
 * @brief Class to handle System Event Log entries
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/sel.cpp
 */

#include "ipmi/sel.hpp"

#include "ipmi/command/generic/sel/delete_sel_entry.hpp"
#include "ipmi/command/generic/sel/get_sel_info.hpp"
#include "ipmi/command/generic/sel/reserve_sel.hpp"
#include "ipmi/command/generic/sel/get_sel_entry.hpp"
#include "ipmi/command/generic/sel/get_sel_time.hpp"

#include "logger/logger_factory.hpp"

#include <iostream>

using namespace ipmi::command::generic;

ipmi::Sel::Sel(ipmi::IpmiController& ctrl): controller(ctrl) { }

ipmi::Sel::~Sel() { }


bool ipmi::Sel::process_records() {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    /* last "processing" timestamp. Some events might be newer either! */
    request::GetSelTime time_req{};
    response::GetSelTime time_resp{};
    controller.send(time_req, time_resp);
    sel_time = time_resp.get_sel_time();


    /* get SEL configuration */
    request::GetSelInfo sel_info_req{};
    response::GetSelInfo sel_info_resp{};
    controller.send(sel_info_req, sel_info_resp);

    if (first_run) {
        reserve_supported = sel_info_resp.is_reserve_supported();
        first_run = false;
    }
    else if (!previous_failed) {
        if ((sel_info_resp.get_last_add_timestamp() == last_add) && (sel_info_resp.get_last_erase_timestamp() == last_erase)) {
            /* nothing has changed in the SEL */
            return false;
        }
    }
    last_add = sel_info_resp.get_last_add_timestamp();
    last_erase = sel_info_resp.get_last_erase_timestamp();
    SelReservationId reservation = get_reservation();

    unsigned read_records = 0;
    SelEntryId record_id = last_read_record;
    for (;;) {
        /* read whole records, all records are 16 bytes and fit into single message */
        request::GetSelEntry get_sel_entry_req{record_id, reservation};
        response::GetSelEntry get_sel_entry_resp{};

        try {
            controller.send(get_sel_entry_req, get_sel_entry_resp);
            read_records++;
        }
        catch (const RequestedRecordNotPresentError&) {
            /* SEL content was cleared, last record does not exist */
            log_debug("telemetry", "Last record was removed: SEL cleared?");
            last_read_record = FIRST_ENTRY;
            records.clear();
            return true;
        }
        catch (const ResponseError& e) {
            /* All read data should be processed, it should continue from the last read one */
            log_info("telemetry", "Not all records read: " << e.what());
            previous_failed = true;
            break;
        }

        if (record_id == FIRST_ENTRY) {
            record_id = ipmi::SelRecord::get_id(get_sel_entry_resp.get_entry());
            log_debug("telemetry", "First record in SEL " << std::hex << record_id);
        }
        records.emplace_back(build(get_sel_entry_resp.get_entry()));
        /* in next loop last record must be read to get appropriate next record id.. */
        last_read_record = record_id;

        /* get next entry if there are some. Next will be read on SEL changed */
        if (get_sel_entry_resp.is_last_record()) {
            previous_failed = false;
            break;
        }
        record_id = get_sel_entry_resp.get_next_entry_id();
    }
    log_debug("telemetry", "Last record read from SEL " << std::hex << record_id
                                    << ", " << std::dec << read_records << " records read");

    /* remove all rolled-out records */
    if ((!previous_failed) && (!records.empty())) {
        /* read very first record in the SEL */
        try {
            request::GetSelEntry get_sel_entry_req{FIRST_ENTRY, reservation};
            response::GetSelEntry get_sel_entry_resp{};

            controller.send(get_sel_entry_req, get_sel_entry_resp);
            record_id = ipmi::SelRecord::get_id(get_sel_entry_resp.get_entry());
        }
        catch (const ResponseError&) {
            record_id = records.front()->get_id();
        }

        /* look for very first one, and remove all records before it */
        RecordVect::iterator record_it;
        for (record_it = records.begin(); record_it != records.end(); record_it++) {
            if ((*record_it)->get_id() == record_id) {
                if (record_it != records.begin()) {
                    log_debug("telemetry", (record_it - records.begin()) << " records rolled out from SEL");
                    records.erase(records.begin(), record_it);
                }
                break;
            }
        }
    }

    /* It is possible to detect removed records only when records are read from the scratch.. */

    return true;
}

ipmi::Sel::RecordVect ipmi::Sel::get_records(ipmi::Sel::RecordFilter filter) const {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    RecordVect ret;
    for (const auto& record : records) {
        if ((!filter) || filter(record)) {
            ret.emplace_back(record);
        }
    }
    return ret;
}


bool ipmi::Sel::purge_record(SelEntryId id) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    /* if record is kept locally */
    if (remove_record(id)) {
        RecordVect::iterator record_it;
        for (record_it = records.begin(); record_it != records.end(); record_it++) {
            if ((*record_it)->get_id() == id) {
                records.erase(record_it);
                return true;
            }
        }
    }
    return false;
}


bool ipmi::Sel::remove_record(SelEntryId id) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    RecordVect::iterator record_it;
    for (record_it = records.begin(); record_it != records.end(); record_it++) {
        if ((*record_it)->get_id() == id) {
            break;
        }
    }
    if (record_it == records.end()) {
        /* record doesn't exist locally, cannot remove */
        return false;
    }

    /* record already removed */
    if ((*record_it)->only_in_memory) {
        return true;
    }

    /* send delete command with proper reservation */
    request::DeleteSelEntry delete_req{id, get_reservation()};
    response::DeleteSelEntry delete_resp{};
    controller.send(delete_req, delete_resp);

    (*record_it)->only_in_memory = true;
    return true;
}

ipmi::SelRecord::Ptr ipmi::Sel::get_record(ipmi::command::generic::SelEntryId id) const {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    RecordVect::const_iterator record_it;
    for (record_it = records.begin(); record_it != records.end(); record_it++) {
        if ((*record_it)->get_id() == id) {
            return *record_it;
        }
    }
    return {};
}


SelReservationId ipmi::Sel::get_reservation() const {
    if (!reserve_supported) {
        /* if reserve is not supported, reservation id must be 0 */
        return 0;
    }
    else {
        /* each run is to be run in new reservation */
        request::ReserveSel reserve_req{};
        response::ReserveSel reserve_resp{};
        controller.send(reserve_req, reserve_resp);
        return reserve_resp.get_reservation_id();
    }
}


ipmi::SelRecord::Ptr ipmi::Sel::build(const ipmi::IpmiInterface::ByteBuffer& record) {
    if (SelRecord::is_oem(record)) {
        return std::make_shared<ipmi::SelRecordOem>(record);
    }

    switch (SelRecordGeneric::get_event_type(record)) {
        /*
         * unspecified 00h n/a
         * Event/Reading Type unspecified.
         */
        case 0x00:
            return std::make_shared<ipmi::SelRecordUnspecified>(record);

        /*
         * Threshold 01h threshold
         * Threshold-based. Indicates a sensor that utilizes values that represent
         * discrete threshold states in sensor access and/or events. The
         * Event/Reading event offsets for the different threshold states are
         * given in Table 42-, Generic Event/Reading Type Codes, below.
         */
        case 0x01:
            return std::make_shared<ipmi::SelRecordThreshold>(record);

        /*
         * Generic 02h-0Ch discrete
         * Generic Discrete. Indicates a sensor that utilizes an Event/Reading
         * Type code & State bit positions / event offsets from one of the sets
         * specified for Discrete or ‘digital’ Discrete Event/Reading class in Table
         * 42-, Generic Event/Reading Type Codes, below.
         */
        case 0x02: /* DMI-based “Usage State” STATES */
        case 0x03: /* DIGITAL/DISCRETE EVENT STATES */
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07: /* SEVERITY EVENT STATES */
        case 0x08: /* AVAILABILITY STATUS STATES */
        case 0x09:
        case 0x0a:
        case 0x0b: /* Other AVAILABILITY STATUS STATES */
        case 0x0c:
            return std::make_shared<ipmi::SelRecordDiscrete>(record);

        /*
         * Sensor-specific 6Fh discrete
         * Sensor-specific Discrete. Indicates that the discrete state information
         * is specific to the sensor type. State bit positions / event offsets for a
         * particular sensor type are specified in the ‘sensor-specific offset’
         * column in Table 42-, Sensor Type Codes.
         */
        case 0x6f:
            return std::make_shared<ipmi::SelRecordSensor>(record);

        /*
         * OEM 70h-7Fh OEM
         * OEM Discrete. Indicates that the discrete state information is specific
         * to the OEM identified by the Manufacturer ID for the IPM device that is
         * providing access to the sensor.
         */
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
            return std::make_shared<ipmi::SelRecordGenericOEM>(record);

        /* reserved codes */
        default:
            return std::make_shared<ipmi::SelRecordOem>(record);
    }
}
