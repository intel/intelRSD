/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file requests/network/add_ethernet_switch_port.cpp
 *
 * @brief Network requests AddEthernetSwitchPort implementation
 * */

#include "agent-framework/module/requests/network/add_ethernet_switch_port.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include <json/json.h>

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddEthernetSwitchPort::AddEthernetSwitchPort(const std::string& switch_v,
            const std::string& port_identifier,
            const agent_framework::model::enums::PortMode mode,
            std::vector<std::string> members,
            agent_framework::model::attribute::Oem):
                m_switch{switch_v},
                m_port_identifier{port_identifier},
                m_mode{mode},
                m_members{members},
                m_oem{} {}

Json::Value AddEthernetSwitchPort::to_json() const {
    Json::Value value;
    value[EthernetSwitch::SWITCH] = m_switch;
    value[EthernetSwitchPort::PORT_IDENTIFIER] = m_port_identifier;
    value[EthernetSwitchPort::MODE] = m_mode.to_string();
    value[EthernetSwitchPort::MEMBERS] =
        agent_framework::model::utils::string_vector_to_json(m_members);
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddEthernetSwitchPort AddEthernetSwitchPort::from_json(const Json::Value& json) {
    return AddEthernetSwitchPort{
        json[EthernetSwitch::SWITCH].asString(),
        json[EthernetSwitchPort::PORT_IDENTIFIER].asString(),
        agent_framework::model::enums::PortMode::from_string(
            json[EthernetSwitchPort::MODE].asString()),
        agent_framework::model::utils::json_to_string_vector(json[EthernetSwitchPort::MEMBERS]),
        agent_framework::model::attribute::Oem::from_json(json[EthernetSwitchPort::OEM])
    };
}
