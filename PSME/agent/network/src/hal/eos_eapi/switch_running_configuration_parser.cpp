/*!
 * @brief Eapi switch running configuration command parser class definition
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
 * @file switch_running_configuration.cpp
 */

#include "hal/eos_eapi/switch_running_configuration_parser.hpp"
#include "agent-framework/module/enum/network.hpp"
#include <regex>



using namespace agent::network::hal::eapi;
using namespace agent_framework::model::attribute;


namespace {

constexpr char DCBX_INFO[] = "(dcbx\\sapplication\\s(udp|tcp-sctp|ether)\\s[0-9]+\\spriority\\s[0-9])|"
                             "(dcbx\\sapplication\\s(iscsi)\\spriority\\s[0-9])|"
                             "(dcbx\\sets\\sqos\\smap\\scos\\s[0-9]+\\straffic-class\\s[0-9]+)|"
                             "(dcbx\\sets\\straffic-class\\s[0-9+]\\sbandwidth\\s[0-9]+)";

constexpr char DCBX_APPLICATION_PRIORITY[] = "(dcbx\\sapplication)\\s(udp|tcp-sctp|ether)\\s([0-9]+)\\s(priority)\\s([0-9]+)";
constexpr int DCBX_APPLICATION_NAME_REGEX_GROUP_INDEX = 2;
constexpr int DCBX_APPLICATION_PORT_REGEX_GROUP_INDEX = 3;
constexpr int DCBX_APPLICATION_PRIORITY_REGEX_GROUP_INDEX = 5;

constexpr char DCBX_APPLICATION_ISCSI_PRIORITY[] = "(dcbx\\sapplication)\\s(iscsi)\\s(priority)\\s([0-9]+)";
constexpr int DCBX_APPLICATION_ISCSI_PRIORITY_REGEX_GROUP_INDEX = 4;

constexpr char DCBX_PRIORITY_MAPPING[] = "(dcbx\\sets\\sqos\\smap\\scos\\s)([0-7]+)\\s(traffic-class)\\s([0-9]+)";
constexpr int DCBX_PRIORITY_REGEX_GROUP_INDEX = 2;
constexpr int DCBX_PRIORITY_GROUP_REGEX_GROUP_INDEX = 4;

constexpr char DCBX_BANDWIDTH_ALLOCATION[] = "(dcbx\\sets\\straffic-class\\s)([0-7]+)\\s(bandwidth)\\s([0-9]+)";
constexpr int PRIORITY_GROUP_REGEX_GROUP_INDEX = 2;
constexpr int BANDWIDTH_REGEX_GROUP_INDEX = 4;

constexpr char LLDP_DISABLED[] = "no\\slldp\\srun";

}


void SwitchRunningConfigurationParser::parse(const std::string& output) {
    read_switch_lldp(output);
    const auto dcbx_info = get_dcbx_info(output);
    read_bandwidth_allocation(dcbx_info);
    read_priority_group_mapping(dcbx_info);
    read_application_protocol(dcbx_info);
    read_application_iscsi_protocol(dcbx_info);
}


std::vector<std::string> SwitchRunningConfigurationParser::get_dcbx_info(const std::string& switch_config) {
    std::vector<std::string> output{};
    std::regex dcbx_regex(DCBX_INFO);
    auto config = switch_config;
    std::sregex_iterator next(config.begin(), config.end(), dcbx_regex);
    std::sregex_iterator end;
    while (next != end) {
        std::smatch match = *next;
        output.push_back(match.str());
        next++;
    }
    return output;
}


void SwitchRunningConfigurationParser::read_bandwidth_allocation(const std::vector<std::string>& list) {
    std::regex dcbx_regex(DCBX_BANDWIDTH_ALLOCATION);
    for (const auto& line : list) {
        std::sregex_iterator next(line.begin(), line.end(), dcbx_regex);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            QosBandwidthAllocation qos_bandwidth_allocation{};
            qos_bandwidth_allocation.set_priority_group(std::stoi(match.str(PRIORITY_GROUP_REGEX_GROUP_INDEX)));
            qos_bandwidth_allocation.set_bandwidth_percent(std::stoi(match.str(BANDWIDTH_REGEX_GROUP_INDEX)));
            m_qos_bandwidth_allocation.add_entry(qos_bandwidth_allocation);
            next++;
        }
    }
}


void SwitchRunningConfigurationParser::read_priority_group_mapping(const std::vector<std::string>& list) {
    std::regex dcbx_regex(DCBX_PRIORITY_MAPPING);
    for (const auto& line : list) {
        std::sregex_iterator next(line.begin(), line.end(), dcbx_regex);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            QosPriorityGroupMapping qos_priority_group_mapping{};
            qos_priority_group_mapping.set_priority_group(std::stoi(match.str(DCBX_PRIORITY_GROUP_REGEX_GROUP_INDEX)));
            qos_priority_group_mapping.set_priority(std::stoi(match.str(DCBX_PRIORITY_REGEX_GROUP_INDEX)));
            m_qos_priority_group_mapping.add_entry(qos_priority_group_mapping);
            next++;
        }
    }
}


void SwitchRunningConfigurationParser::read_application_protocol(const std::vector<std::string>& list) {
    std::map<std::string, agent_framework::model::enums::TransportLayerProtocol> protocol_type = {
        {"tcp-sctp", agent_framework::model::enums::TransportLayerProtocol::TCP},
        {"udp", agent_framework::model::enums::TransportLayerProtocol::UDP},
        {"ether", agent_framework::model::enums::TransportLayerProtocol::L2}
    };
    std::regex dcbx_regex(DCBX_APPLICATION_PRIORITY);
    for (const auto& line : list) {
        std::sregex_iterator next(line.begin(), line.end(), dcbx_regex);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            QosApplicationProtocol qos_application_protocol{};
            qos_application_protocol.set_port(std::stoi(match.str(DCBX_APPLICATION_PORT_REGEX_GROUP_INDEX)));
            qos_application_protocol.set_priority(std::stoi(match.str(DCBX_APPLICATION_PRIORITY_REGEX_GROUP_INDEX)));
            qos_application_protocol.set_protocol(protocol_type.at(match.str(DCBX_APPLICATION_NAME_REGEX_GROUP_INDEX)));
            m_qos_application_protocol.add_entry(qos_application_protocol);
            next++;
        }
    }
}

// TCP ports for iSCSI
constexpr int DCBX_APPLICATION_ISCSI_PORT_1 = 860;
constexpr int DCBX_APPLICATION_ISCSI_PORT_2 = 3260;

void SwitchRunningConfigurationParser::read_application_iscsi_protocol(const std::vector<std::string>& list) {
    std::regex dcbx_regex(DCBX_APPLICATION_ISCSI_PRIORITY);
    for (const auto& line : list) {
        std::sregex_iterator next(line.begin(), line.end(), dcbx_regex);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            QosApplicationProtocol qos_application_protocol{};
            qos_application_protocol.set_port(DCBX_APPLICATION_ISCSI_PORT_1);
            qos_application_protocol.set_priority(std::stoi(match.str(DCBX_APPLICATION_ISCSI_PRIORITY_REGEX_GROUP_INDEX)));
            qos_application_protocol.set_protocol(agent_framework::model::enums::TransportLayerProtocol::TCP);
            m_qos_application_protocol.add_entry(qos_application_protocol);

            qos_application_protocol.set_port(DCBX_APPLICATION_ISCSI_PORT_2);
            m_qos_application_protocol.add_entry(qos_application_protocol);
            next++;
        }
    }
}

void SwitchRunningConfigurationParser::read_switch_lldp(const std::string& switch_config) {
    std::regex lldp_regex(LLDP_DISABLED);
    if (regex_search(switch_config, lldp_regex)) {
        m_lldp_enabled = false;
    }
    else {
        m_lldp_enabled = true;
    }
}