/*!
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file requests/network/get_port_vlan_info.cpp
 * @brief Network request get port vlan information implementation
 * */

#include "agent-framework/module/requests/network/get_port_vlan_info.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetPortVlanInfo::GetPortVlanInfo(const std::string& port_vlan): m_port_vlan(port_vlan){}

json::Json GetPortVlanInfo::to_json() const {
    json::Json value = json::Json();
    value[EthernetSwitchPortVlan::PORT_VLAN] = m_port_vlan;
    return value;
}

GetPortVlanInfo GetPortVlanInfo::from_json(const json::Json& json) {
    return GetPortVlanInfo{
        json[EthernetSwitchPortVlan::PORT_VLAN].get<std::string>()
    };
}
