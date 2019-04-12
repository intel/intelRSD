/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
#include "json-wrapper/json-wrapper.hpp"

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

json::Json AddEthernetSwitchPort::to_json() const {
    json::Json value = json::Json();
    value[EthernetSwitch::SWITCH] = m_switch;
    value[EthernetSwitchPort::PORT_IDENTIFIER] = m_port_identifier;
    value[EthernetSwitchPort::MODE] = m_mode.to_string();
    value[EthernetSwitchPort::MEMBERS] = m_members;
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddEthernetSwitchPort AddEthernetSwitchPort::from_json(const json::Json& json) {
    return AddEthernetSwitchPort{
        json[EthernetSwitch::SWITCH],
        json[EthernetSwitchPort::PORT_IDENTIFIER],
        agent_framework::model::enums::PortMode::from_string(json[EthernetSwitchPort::MODE]),
        json[EthernetSwitchPort::MEMBERS],
        agent_framework::model::attribute::Oem::from_json(json[EthernetSwitchPort::OEM])
    };
}
