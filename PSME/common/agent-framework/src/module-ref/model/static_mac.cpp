/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

#include "agent-framework/module-ref/model/static_mac.hpp"
#include "agent-framework/module-ref/constants/network.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component StaticMac::component = enums::Component::StaticMac;
const enums::CollectionName StaticMac::collection_name = enums::CollectionName::StaticMacs;

StaticMac::StaticMac(const std::string& parent_uuid) :
        Resource{parent_uuid, enums::Component::SwitchPort} {}

StaticMac::~StaticMac() {}

Json::Value StaticMac::to_json() const {
    Json::Value result;
    result[literals::StaticMac::STATUS] = get_status().to_json();
    result[literals::StaticMac::ADDRESS] = get_address();
    result[literals::StaticMac::VLAN_ID] = get_vlan_id();
    result[literals::StaticMac::OEM] = get_oem().to_json();
    return result;
}

StaticMac StaticMac::from_json(const Json::Value& json) {
    StaticMac static_mac;
    static_mac.set_status(attribute::Status::from_json(json[literals::StaticMac::STATUS]));
    static_mac.set_address(json[literals::StaticMac::ADDRESS]);
    static_mac.set_vlan_id(json[literals::StaticMac::VLAN_ID]);
    static_mac.set_oem(attribute::Oem::from_json(json[literals::StaticMac::OEM]));
    static_mac.set_resource_hash(compute_hash(json));
    return static_mac;
}
