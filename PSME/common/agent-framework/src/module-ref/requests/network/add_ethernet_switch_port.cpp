/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/module-ref/requests/network/add_ethernet_switch_port.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
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
    value[Switch::SWITCH] = m_switch;
    value[SwitchPort::PORT_IDENTIFIER] = m_port_identifier;
    value[SwitchPort::MODE] = m_mode.to_string();
    value[SwitchPort::MEMBERS] =
        agent_framework::model::utils::string_vector_to_json(m_members);
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddEthernetSwitchPort AddEthernetSwitchPort::from_json(const Json::Value& json) {
    return AddEthernetSwitchPort{
        json[Switch::SWITCH].asString(),
        json[SwitchPort::PORT_IDENTIFIER].asString(),
        agent_framework::model::enums::PortMode::from_string(
            json[SwitchPort::MODE].asString()),
        agent_framework::model::utils::json_to_string_vector(json[SwitchPort::MEMBERS]),
        agent_framework::model::attribute::Oem::from_json(json[SwitchPort::OEM])
    };
}
