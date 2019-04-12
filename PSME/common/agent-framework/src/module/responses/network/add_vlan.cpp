/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 *
 * @file responses/network/add_vlan.cpp
 * @brief AddVlan response
 * */

#include "agent-framework/module/responses/network/add_vlan.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;

AddVlan::AddVlan() {}


json::Json AddVlan::to_json() const {
    json::Json value = json::Json();
    value[Vlan::VLAN] = m_vlan;
    value[Vlan::OEM] = m_oem.to_json();
    return value;
}

AddVlan AddVlan::from_json(const json::Json& json) {
    AddVlan vlan{};
    vlan.set_vlan(json[Vlan::VLAN]);
    vlan.set_oem(Oem::from_json(json[Acl::OEM]));
    return vlan;
}
