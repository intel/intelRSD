/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module-ref/model/port_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component PortVlan::component = enums::Component::PortVlan;
const enums::CollectionName PortVlan::collection_name =
    enums::CollectionName::PortVlans;

PortVlan::PortVlan(const std::string& parent_uuid) :
    Resource{parent_uuid, enums::Component::SwitchPort} {}

PortVlan::~PortVlan() {}

Json::Value PortVlan::to_json() const {
    Json::Value result;
    result[literals::PortVlan::STATUS] = get_status().to_json();
    result[literals::PortVlan::VLAN_ID] = get_vlan_id();
    result[literals::PortVlan::VLAN_NAME] = get_vlan_name();
    result[literals::PortVlan::VLAN_ENABLE] = get_vlan_enable();
    result[literals::PortVlan::TAGGED] = get_tagged();
    result[literals::PortVlan::OEM] = get_oem().to_json();
    return result;
}

PortVlan PortVlan::from_json(const Json::Value& json) {
    PortVlan vlan;

    vlan.set_status(attribute::Status::from_json(json[literals::PortVlan::STATUS]));
    vlan.set_vlan_id(json[literals::PortVlan::VLAN_ID]);
    vlan.set_vlan_name(json[literals::PortVlan::VLAN_NAME]);
    vlan.set_vlan_enable(json[literals::PortVlan::VLAN_ENABLE]);
    vlan.set_tagged(json[literals::PortVlan::TAGGED]);
    vlan.set_oem(attribute::Oem::from_json(json[literals::PortVlan::OEM]));
    vlan.set_resource_hash(compute_hash(json));

    return vlan;
}
