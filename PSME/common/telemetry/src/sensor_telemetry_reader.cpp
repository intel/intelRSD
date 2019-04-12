/*!
 * @brief Implementation of all methods for Sensor telemetry reader
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
 * @file sensor_telemetry_reader.cpp
 */

#include "telemetry/sensor_telemetry_reader.hpp"

#include "ipmi/command/generic/reserve_sdr_repository.hpp"
#include "ipmi/command/generic/get_sdr_repository_info.hpp"
#include "ipmi/command/generic/get_sdr.hpp"
#include "ipmi/command/generic/get_sensor_reading.hpp"

#include "logger/logger_factory.hpp"

extern "C" {
#include "ipmitool/ipmi_sdr.h"
#undef GET_SDR_ENTIRE_RECORD
}

using namespace ipmi::command::generic;

namespace {

#pragma pack(1)
struct SdrHeader {
    uint16_t id;    /* record ID */
    uint8_t version;    /* SDR version (51h) */
    uint8_t type;    /* record type */
    uint8_t length;    /* remaining record bytes */
};
#pragma pack(0)


template<typename T>
std::string get_sdr_record_name(const T* record, const ipmi::IpmiInterface::ByteBuffer& data) {
    static_assert(std::is_array<decltype(record->id_string)>::value, "id_string is not an array");
    static_assert(std::is_pod<decltype(record->id_string)>::value, "id_string is not POD");
    constexpr const std::uint8_t LENGTH_MASK = 0x1F;
    const auto id_length = record->id_code & LENGTH_MASK;
    if (sizeof(T) + sizeof(SdrHeader) - sizeof(record->id_string) + id_length > data.size()) {
        log_warning("telemetry", " Unable to read name of sensor #" << std::hex << uint(record->cmn.keys.sensor_num));
        return "#" + std::to_string(uint(record->cmn.keys.sensor_num));
    }
    return std::string(reinterpret_cast<const char*>(record->id_string), id_length);
}


// view on Sdr record
struct SdrRecord {
    const SdrHeader* header{};
    union {
        const sdr_record_common_sensor* cmn{};
        const sdr_record_full_sensor* full;
        const sdr_record_compact_sensor* compact;
    };
};


SdrRecord get_record(const ipmi::IpmiInterface::ByteBuffer& data) {
    SdrRecord record{};
    if (data.size() < sizeof(SdrHeader)) {
        log_warning("telemetry", " SDR record header too short: " << data.size());
        return record;
    }
    record.header = reinterpret_cast<const SdrHeader*>(data.data());

    const auto rec_length = std::min(data.size(), static_cast<size_t>(record.header->length));

    switch (record.header->type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:
            if (rec_length < (sizeof(sdr_record_full_sensor) - sizeof(record.full->id_string))) {
                log_warning("telemetry", " Skipping full sensor record #" << uint(record.header->id)
                                                                          << " sensor #0x" << std::hex
                                                                          << uint(record.cmn->keys.sensor_num)
                                                                          << ", length too short: " << std::dec
                                                                          << rec_length);
                break;
            }
            record.full = reinterpret_cast<const sdr_record_full_sensor*>(data.data() + sizeof(SdrHeader));
            break;
        case SDR_RECORD_TYPE_COMPACT_SENSOR:
            if (rec_length < (sizeof(sdr_record_compact_sensor) - sizeof(record.compact->id_string))) {
                log_warning("telemetry", " Skipping compact sensor record #" << uint(record.header->id)
                                                                             << " sensor #0x" << std::hex
                                                                             << uint(record.cmn->keys.sensor_num)
                                                                             << ", length too short " << std::dec
                                                                             << rec_length);
                break;
            }
            record.compact = reinterpret_cast<const sdr_record_compact_sensor*>(data.data() + sizeof(SdrHeader));
            break;
        default:
            break;
    }
    return record;
}


}


namespace telemetry {

class SensorContext : public TelemetryReader::Context {
public:
    struct SensorDefinition {
        using ConversionFn = std::function<json::Json(std::uint8_t)>;
        int entity_id{};
        int entity_instance{};
        bool has_analog_reading{};
        OptionalField<bool> is_linear{};
        std::string name{};
        ConversionFn conversion_fn{[](std::uint8_t reading) { return reading; }};
    };
    using SensorMap = std::unordered_map<SensorTelemetryReader::SensorNumber, SensorDefinition>;

    SensorContext(ipmi::IpmiController& ctrl) :
        sdr_definitions(read_sdr_definitions(ctrl)) {}

    SensorMap& get_sdr_definitions() { return sdr_definitions; }

protected:
    /*!
     * @brief update context, nothing to be done here
     * @return true to process all records
     */
    bool update() override;

private:
    SensorMap sdr_definitions;

    static SensorMap read_sdr_definitions(ipmi::IpmiController& ctrl);
};


bool SensorContext::update() {
    return true;
}


SensorContext::SensorMap SensorContext::read_sdr_definitions(ipmi::IpmiController& ctrl) {

    SensorContext::SensorMap ret{};

    /* actual sensors are described by SDR records 1/2 */
    response::ReserveSdrRepository reserve_sdr_repo_resp{};
    ctrl.send(request::ReserveSdrRepository(), reserve_sdr_repo_resp);

    request::GetSdr get_sdr_req{};
    get_sdr_req.set_record_id(request::GetSdr::SDR_FIRST_RECORD_ID);
    get_sdr_req.set_reservation_id(reserve_sdr_repo_resp.get_reservation_id());
    get_sdr_req.set_bytes_to_read(request::GetSdr::GET_SDR_ENTIRE_RECORD);
    response::GetSdr get_sdr_resp{};

    do {

        ctrl.send(get_sdr_req, get_sdr_resp);

        const auto& record_data = get_sdr_resp.get_record_data();

        const auto record = get_record(record_data);

        if (nullptr == record.cmn) {
            log_debug("telemetry",
                "Skipping record #" << uint(record.header->id) << " type: " << uint(record.header->type));
            get_sdr_req.set_record_id(get_sdr_resp.get_next_record_id());
            continue;
        }

        if (ret.count(record.cmn->keys.sensor_num) != 0) {
            log_warning("telemetry", "Duplicated sensor#"
                << std::hex << static_cast<unsigned> (record.cmn->keys.sensor_num) << " in SDR");
        }
        else {
            SensorContext::SensorDefinition def{};
            def.entity_id = record.cmn->entity.id;
            def.entity_instance = record.cmn->entity.instance;

            if (record.header->type == SDR_RECORD_TYPE_FULL_SENSOR) {
                def.has_analog_reading = IS_THRESHOLD_SENSOR(record.cmn) && !UNITS_ARE_DISCRETE(record.cmn);
                def.name = get_sdr_record_name(record.full, record_data);

                if (def.has_analog_reading) {
                    def.is_linear = record.full->linearization != SDR_SENSOR_L_NONLINEAR;
                    int16_t multiplier = __TO_M(record.full->mtol);
                    int32_t additive_offset = __TO_B(record.full->bacc);
                    int32_t exponent = __TO_B_EXP(record.full->bacc);
                    int32_t result_exponent = __TO_R_EXP(record.full->bacc);
                    log_debug("telemetry", def.name << " (#0x" << std::hex << uint(record.cmn->keys.sensor_num)
                                                    << ") [M: " << std::dec << multiplier
                                                    << " B: " << additive_offset << " B_EXP: " << exponent << " R_EXP: "
                                                    << result_exponent << "]");
                    def.conversion_fn = [multiplier, additive_offset, exponent, result_exponent](
                        std::uint8_t raw_reading) {
                        return (raw_reading * multiplier + additive_offset * std::pow(10, exponent))
                               * std::pow(10, result_exponent);
                    };
                }
            }
            else if (record.header->type == SDR_RECORD_TYPE_COMPACT_SENSOR) {
                def.name = get_sdr_record_name(record.compact, record_data);
                log_debug("telemetry", def.name << " (#0x" << std::hex << uint(record.cmn->keys.sensor_num) << ")");
            }

            ret[record.cmn->keys.sensor_num] = def;
        }

        get_sdr_req.set_record_id(get_sdr_resp.get_next_record_id());
    }
    while (get_sdr_resp.get_next_record_id() != request::GetSdr::SDR_LAST_RECORD_ID);

    return ret;
}


TelemetryReader::ReaderId SensorTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}

TelemetryReader::Context::Ptr SensorTelemetryReader::create_context(ipmi::IpmiController& ctrl,
                                                                    TelemetryReader::PtrVector&) {
    return std::make_shared<SensorContext>(ctrl);
}

std::string SensorTelemetryReader::get_info() const {
    return "Sensor #" + std::to_string(uint(get_sensor_number()));
}


bool SensorTelemetryReader::is_valid(TelemetryReader::Context::Ptr context) const {
    SensorContext::SensorMap& defined = static_cast<SensorContext*>(context.get())->get_sdr_definitions();

    bool ret = false;
    auto sensor_def_it = defined.find(get_sensor_number());
    if (sensor_def_it == defined.end()) {
        log_error("telemetry", "Sensor #0x" << std::hex << static_cast<unsigned>(get_sensor_number())
                  << " not defined in SDR");
    }
    else if (sensor_def_it->second.entity_id == 0) {
        log_error("telemetry", "Duplicated sensor #0x" << std::hex << static_cast<unsigned>(get_sensor_number())
                  << " in definitions");
    }
    else if ((sensor_def_it->second.entity_id != get_entity_id()) ||
             (sensor_def_it->second.entity_instance != get_entity_instance())) {
        log_error("telemetry", "Entity mismatch " << sensor_def_it->second.entity_id << "."
                  << sensor_def_it->second.entity_instance << " for sensor #0x" << std::hex
                  << static_cast<unsigned>(get_sensor_number()));
    }
    else {
        sensor_def_it->second.entity_id = 0; /* mark as "already defined */
        ret = true;
    }
    return ret;
}


bool SensorTelemetryReader::read(TelemetryReader::Context::Ptr context, ipmi::IpmiController& ctrl) {
    SensorContext* ctx = static_cast<SensorContext*>(context.get());

    request::GetSensorReading request{};
    request.set_sensor_number(get_sensor_number());
    response::GetSensorReading response{};

    ctrl.send(request, response);

    const auto value = response.is_valid_reading()
                       ? ctx->get_sdr_definitions()[get_sensor_number()].conversion_fn(response.get_sensor_reading())
                       : json::Json();
    return update_value(value);
}

}
