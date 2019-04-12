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
 * @file requests/network/get_vlan_info.cpp
 * @brief Network request get port vlan information implementation
 * */

#include "agent-framework/module/requests/network/get_vlan_info.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetVlanInfo::GetVlanInfo(const std::string& vlan): m_vlan(vlan){}

json::Json GetVlanInfo::to_json() const {
    json::Json value = json::Json();
    value[Vlan::VLAN] = m_vlan;
    return value;
}

GetVlanInfo GetVlanInfo::from_json(const json::Json& json) {
    return GetVlanInfo{
        json[Vlan::VLAN].get<std::string>()
    };
}
