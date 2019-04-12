/*!
 * @brief Class to maintain System Event Log
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
 * @file ipmi/sel.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/sel_record.hpp"

#include <memory>
#include <map>
#include <mutex>
#include <functional>

namespace ipmi {

class Sel {
public:
    using RecordVect = std::vector<SelRecord::Ptr>;
    using RecordFilter = std::function<bool(SelRecord::Ptr)>;

    /*!
     * @brief Default constructor
     * @param controller shared pointer to the controller
     */
    Sel(IpmiController& controller);

    /*! Destructor */
    virtual ~Sel();

    /*!
     * @brief Read all records from SEL
     *
     * It tries to records from the last one read. If reservation is canceled, awaiting
     * records are to be read in next cycle.
     *
     * @return true if event "database" has changed
     */
    bool process_records();

    /*!
     * @brief Remove event from event log
     *
     * Both local and SEL entries are removed. Event is to be purged on acknowledge.
     *
     * @param id of the record to be purged
     * @return true if record does not exist anymore
     */
    bool purge_record(ipmi::command::generic::SelEntryId id);

    /*!
     * @brief Remove event from event log
     *
     * Only SEL entry is removed, local copy is kept.
     *
     * @param id of the record to be purged
     * @return true if record does not exist anymore
     */
    bool remove_record(ipmi::command::generic::SelEntryId id);

    /*!
     * @brief Get last processing SEL time
     * @return number of seconds since SEL service started
     */
    ipmi::command::generic::SelTimestamp get_sel_time() const { return sel_time; }

    /*!
     * @brief Get current events from SEL
     * @param filter event selector or null if all events should be passed
     * @warning If called in callback, the list might be incomplete
     * @return shapshot of events matching the filter
     */
    RecordVect get_records(RecordFilter filter = nullptr) const;

    /*!
     * @brief Get record by sel entry id
     * @param id of the record to be get
     * @return found record or nullptr reference
     */
    SelRecord::Ptr get_record(ipmi::command::generic::SelEntryId id) const;

private:
    /*!
     * @brief Special record ID to find very first record in the SEL
     */
    static constexpr ipmi::command::generic::SelEntryId FIRST_ENTRY = 0;

    /*!
     * @brief SEL record builder
     * @param record plain bytes to create appropriate event object
     * @return shared pointer to created record
     */
    SelRecord::Ptr build(const ipmi::IpmiInterface::ByteBuffer& record);

    /*!
     * @brief Get new reservation
     * @return reservation ID or 0 if reservation is not supported.
     */
    ipmi::command::generic::SelReservationId get_reservation() const;

    IpmiController& controller;

    ipmi::command::generic::SelTimestamp sel_time{};

    bool first_run{true};
    bool previous_failed{false};
    ipmi::command::generic::SelTimestamp last_add{};
    ipmi::command::generic::SelTimestamp last_erase{};
    bool reserve_supported{};

    ipmi::command::generic::SelEntryId last_read_record{FIRST_ENTRY};
    RecordVect records{};

    /*!
     * @brief Mutex to synchronize the log
     * @warning mutable for get_events() const
     */
    mutable std::recursive_mutex mutex{};
};

}

