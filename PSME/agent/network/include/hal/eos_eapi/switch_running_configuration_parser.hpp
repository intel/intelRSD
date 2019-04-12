/*!
 * @brief Eapi show switch running configuration command parser class declaration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file switch_running_configuration_parser.hpp
 */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include <string>



namespace agent {
namespace network {
namespace hal {
namespace eapi {


/*!
 * Class for accessing information obtained from "show running configuration" command
 */
class SwitchRunningConfigurationParser {

public:
    /*!
     * Constructor
     * @param output switch command output
     */
    SwitchRunningConfigurationParser(const std::string& output) {
        parse(output);
    }


    /*!
     * Default constructor
     */
    SwitchRunningConfigurationParser() = delete;


    /*!
     * Destructor
     */
    virtual ~SwitchRunningConfigurationParser() = default;


    /*!
     * @brief Getter for QoS priority group to priority mapping for ETS
     * @return QoS priority group to priority mapping
     */
    const agent_framework::model::attribute::Array<agent_framework::model::attribute::QosPriorityGroupMapping>&
    get_qos_priority_group_mapping() const {
        return m_qos_priority_group_mapping;
    }


    /*!
     * @brief Getter for QoS bandwidth allocation for ETS
     * @return QoS bandwidth allocation object
     */
    const agent_framework::model::attribute::Array<agent_framework::model::attribute::QosBandwidthAllocation>&
    get_qos_bandwidth_allocation() const {
        return m_qos_bandwidth_allocation;
    }


    /*!
     * @brief Getter for QoS application protocol
     * @return QoS application protocol
     */
    const agent_framework::model::attribute::Array<agent_framework::model::attribute::QosApplicationProtocol>&
    get_qos_application_protocol() const {
        return m_qos_application_protocol;
    }

    /*!
     * @brief Is switch LLDP enabled
     * @return true if LLDP is enabled, false otherwise
     */
    bool is_switch_lldp_enabled() {
        return m_lldp_enabled;
    }


private:
    void parse(const std::string& output);
    std::vector<std::string> get_dcbx_info(const std::string& switch_config);
    void read_bandwidth_allocation(const std::vector<std::string>& list);
    void read_priority_group_mapping(const std::vector<std::string>& list);
    void read_application_protocol(const std::vector<std::string>& list);
    void read_application_iscsi_protocol(const std::vector<std::string>& list);
    void read_switch_lldp(const std::string& switch_config);

    agent_framework::model::attribute::Array<agent_framework::model::attribute::QosApplicationProtocol> m_qos_application_protocol{};
    agent_framework::model::attribute::Array<agent_framework::model::attribute::QosPriorityGroupMapping> m_qos_priority_group_mapping{};
    agent_framework::model::attribute::Array<agent_framework::model::attribute::QosBandwidthAllocation> m_qos_bandwidth_allocation{};
    bool m_lldp_enabled{};
};

}
}
}
}