/*!
 * @brief Delete QoS application protocol command class definition.
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
 * @file delete_switch_qos_application_protocol.cpp
 */

#include "hal/eos_eapi/delete_switch_qos_application_protocol.hpp"

using namespace agent::network::hal;


std::vector<std::string> DeleteSwitchQosApplicationProtocol::serialize() const {
    std::map<agent_framework::model::enums::TransportLayerProtocol, std::string> protocol_type = {
            {agent_framework::model::enums::TransportLayerProtocol::TCP, "tcp-sctp"},
            {agent_framework::model::enums::TransportLayerProtocol::UDP, "udp"},
            {agent_framework::model::enums::TransportLayerProtocol::L2, "ether"}
    };
    std::vector<std::string> commands;
    commands.emplace_back("configure");
    commands.push_back("no dcbx application " + protocol_type.at(m_protocol) + " " + std::to_string(m_port) +
                       " priority " + std::to_string(m_priority));
    return commands;
}

DeleteSwitchQosApplicationProtocol::~DeleteSwitchQosApplicationProtocol() { }