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
 * @file requests/network/delete_switch_port_members.cpp
 *
 * @brief Network request DeleteEthernetSwitchPortMembers implementation
 * */

#include "agent-framework/module/requests/network/delete_ethernet_switch_port_members.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/utils/utils.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeleteEthernetSwitchPortMembers::DeleteEthernetSwitchPortMembers(const std::vector<std::string>& members,
             const std::string& port,
            agent_framework::model::attribute::Oem):
                m_members{members},
                m_port(port),
                m_oem{} {}

json::Json DeleteEthernetSwitchPortMembers::to_json() const {
    json::Json value = json::Json();
    value[EthernetSwitchPort::MEMBERS] = m_members;
    value[EthernetSwitchPort::PORT] = m_port;
    value[EthernetSwitchPort::OEM] = m_oem.to_json();
    return value;
}

DeleteEthernetSwitchPortMembers DeleteEthernetSwitchPortMembers::from_json(const json::Json& json) {
    return DeleteEthernetSwitchPortMembers{
        json[EthernetSwitchPort::MEMBERS],
        json[EthernetSwitchPort::PORT],
        agent_framework::model::attribute::Oem::from_json(json[EthernetSwitchPort::OEM])
    };
}
