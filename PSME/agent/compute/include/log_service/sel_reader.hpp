/*!
 * @brief Sel reader class declaration
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file sel_reader.hpp
 */



#include "agent-framework/module/common_components.hpp"
#include "status/bmc.hpp"
#include "ipmi/command/generic/sel/sel_defs.hpp"
#include "ipmi/sel.hpp"
#include "tree_stability/compute_stabilizer.hpp"


class SELReader {
public:

    using TimePoint = std::chrono::steady_clock::time_point;
    using SelTimestamp = ipmi::command::generic::SelTimestamp;

    using RedfishSensorType = agent_framework::model::enums::SensorType;
    using IpmiSensorType = ipmi::command::generic::SensorType;

    static constexpr SelTimestamp SEL_INIT_TIME = 0x20000000;


    /*!
     * @brief Single SEL reading
     * @param bmc BMC object
     */
    void read(agent::compute::Bmc& bmc);

private:

    /*!
     * @brief Fill log entry with sel info
     * @param log_entry log entry to be filled
     * @param sel_record sel record with data
     */
    void fill_entry(agent_framework::model::LogEntry& log_entry, const std::shared_ptr<ipmi::SelRecordGeneric>& sel_record);


    /*!
     * @brief Method for checking whether sel got updated
     * @param ipmi Object for connecting to bmc
     * @return true if sel updated, false otherwise
     */
    bool was_sel_changed(ipmi::IpmiController& ipmi);


    /*!
     * @brief Method for gathering sel records from bmc
     * @param ipmi Object for connecting to bmc
     * @return vector of sel records
     */
    ipmi::Sel::RecordVect get_records(ipmi::IpmiController& ipmi);


    /*!
     * @brief Cast ipmi sensor type to redfish sensor type
     * @param sensor_type ipmi sensor type
     * @return Optional field conatining sensor type or no value
     */
    OptionalField<RedfishSensorType> ipmi_sensor_to_redfish(IpmiSensorType sensor_type);


    /*!
     * @brief Add or update log entries
     * @param events events vector
     * @param log_entry log entry
     */
    void add_or_update(agent_framework::model::attribute::EventData::Vector& events,
                       const agent_framework::model::LogEntry& log_entry);


    /*!
     * @brief Remove entries not present in sel
     * @param bmc_id bmc id
     * @param before_change_epoch epoch before any change to entry
     * @param events events vector
     */
    void remove_untouched(const std::string& bmc_id,
                          const std::uint64_t before_change_epoch,
                          agent_framework::model::attribute::EventData::Vector& events);


    SelTimestamp m_last_add_timestamp{};
    SelTimestamp m_last_remove_timestamp{};

    agent::compute::ComputeStabilizer m_stabilizer{};

    static const std::map<IpmiSensorType, RedfishSensorType> m_ipmi_redfish_sensor_map;

};
