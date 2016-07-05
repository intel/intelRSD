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

#include "agent-framework/module-ref/model/switch.hpp"
#include "agent-framework/module-ref/constants/network.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Switch::component =
    enums::Component::Switch;
const enums::CollectionName Switch::collection_name =
    enums::CollectionName::EthernetSwitches;

Switch::Switch(const std::string& parent_uuid) :
    Resource{parent_uuid} {}

Switch::~Switch() {}

Json::Value Switch::to_json() const {
    Json::Value result;
    result[literals::Switch::STATUS] = get_status().to_json();
    result[literals::Switch::SWITCH_IDENTIFIER] = get_switch_identifier();
    result[literals::Switch::TECHNOLOGY] = get_technology();
    result[literals::Switch::MAC_ADDRESS] = get_mac_address();
    result[literals::Switch::FIRMWARE_NAME] = get_firmware_name();
    result[literals::Switch::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::Switch::ROLE] = get_role();
    result[literals::Switch::FRU_INFO] = get_fru_info().to_json();
    result[literals::Switch::MANUFACTURING_DATE] = get_manufacturing_date();
    result[literals::Switch::LOCATION] = get_location();
    result[literals::Switch::CHASSIS] = get_chassis();
    result[literals::Switch::MAX_ACL_NUMBER] = get_max_acl_number();
    result[literals::Switch::COLLECTIONS] = get_collections().to_json();
    result[literals::Switch::OEM] = get_oem().to_json();
    return result;
}

Switch Switch::from_json(const Json::Value& json) {
    Switch sw;

    sw.set_status(attribute::Status::from_json(json[literals::Switch::STATUS]));
    sw.set_switch_identifier(
        json[literals::Switch::SWITCH_IDENTIFIER]);
    sw.set_technology(json[literals::Switch::TECHNOLOGY]);
    sw.set_mac_address(json[literals::Switch::MAC_ADDRESS]);
    sw.set_firmware_name(json[literals::Switch::FIRMWARE_NAME]);
    sw.set_firmware_version(json[literals::Switch::FIRMWARE_VERSION]);
    sw.set_role(json[literals::Switch::ROLE]);
    sw.set_fru_info(attribute::FruInfo::from_json(
        json[literals::Switch::FRU_INFO]));
    sw.set_manufacturing_date(json[literals::Switch::MANUFACTURING_DATE]);
    sw.set_location(json[literals::Switch::LOCATION]);
    sw.set_chassis(json[literals::Switch::CHASSIS]);
    sw.set_max_acl_number(json[literals::Switch::MAX_ACL_NUMBER]);
    sw.set_collections(Collections::from_json(
        json[literals::Switch::COLLECTIONS]));
    sw.set_oem(attribute::Oem::from_json(json[literals::Switch::OEM]));
    sw.set_resource_hash(compute_hash(json));

    return sw;
}
