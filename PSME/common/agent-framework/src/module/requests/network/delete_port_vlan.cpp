/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file requests/network/delete_port_vlan.cpp
 *
 * @brief Network request DeletePortVlan implementation
 * */

#include "agent-framework/module/requests/network/delete_port_vlan.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeletePortVlan::DeletePortVlan(const std::string& port_vlan, Oem oem):
                m_port_vlan(port_vlan),
                m_oem{oem} {}

json::Json DeletePortVlan::to_json() const {
    json::Json value = json::Json();
    value[EthernetSwitchPortVlan::PORT_VLAN] = m_port_vlan;
    value[EthernetSwitchPortVlan::OEM] = m_oem.to_json();
    return value;
}

DeletePortVlan DeletePortVlan::from_json(const json::Json& json) {
    return DeletePortVlan{
        json[EthernetSwitchPortVlan::PORT_VLAN],
        Oem{}
    };
}
