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
 * */

#include "agent-framework/module/model/vlan.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Vlan::component = enums::Component::Vlan;
const enums::CollectionName Vlan::collection_name =
    enums::CollectionName::Vlans;

Vlan::Vlan(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Vlan::~Vlan() {}

json::Json Vlan::to_json() const {
    json::Json result = json::Json();
    result[literals::Vlan::STATUS] = get_status().to_json();
    result[literals::Vlan::VLAN_ID] = get_vlan_id();
    result[literals::Vlan::VLAN_NAME] = get_vlan_name();
    result[literals::Vlan::VLAN_ENABLE] = get_vlan_enable();
    result[literals::Vlan::OEM] = get_oem().to_json();
    return result;
}

Vlan Vlan::from_json(const json::Json& json) {
    Vlan vlan;

    vlan.set_status(attribute::Status::from_json(json[literals::Vlan::STATUS]));
    vlan.set_vlan_id(json[literals::Vlan::VLAN_ID]);
    vlan.set_vlan_name(json[literals::Vlan::VLAN_NAME]);
    vlan.set_vlan_enable(json[literals::Vlan::VLAN_ENABLE]);
    vlan.set_oem(attribute::Oem::from_json(json[literals::Vlan::OEM]));

    return vlan;
}
