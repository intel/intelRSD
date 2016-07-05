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
*/

#include "agent-framework/module-ref/model/chassis.hpp"
#include "agent-framework/module-ref/constants/chassis.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Chassis::component = enums::Component::Chassis;
const enums::CollectionName Chassis::collection_name =
    enums::CollectionName::Chassis;

Chassis::Chassis(const std::string& parent_uuid) : Resource{parent_uuid} {}

Chassis::~Chassis() {}

Json::Value Chassis::to_json() const {
    Json::Value json;
    json[literals::Chassis::STATUS] = get_status().to_json();
    json[literals::Chassis::TYPE] = get_type().to_string();
    json[literals::Chassis::SIZE] = get_size();
    json[literals::Chassis::LOCATION_OFFSET] =
        get_location_offset();
    json[literals::Chassis::PARENT_ID] = get_parent_id();
    json[literals::Chassis::POWER_ZONE] = get_power_zone();
    json[literals::Chassis::THERMAL_ZONE] = get_thermal_zone();
    json[literals::Chassis::FRU_INFO] = get_fru_info().to_json();
    json[literals::Chassis::COLLECTIONS] = get_collections().to_json();
    json[literals::Chassis::SKU] = get_sku();
    json[literals::Chassis::ASSET_TAG] = get_asset_tag();
    json[literals::Chassis::INDICATOR_LED] = get_indicator_led();
    json[literals::Chassis::OEM] = get_oem().to_json();
    return json;
}

Chassis Chassis::from_json(const Json::Value& json) {
    Chassis chassis;

    chassis.set_status(attribute::Status::from_json(
        json[literals::Chassis::STATUS]));
    chassis.set_type(enums::ChassisType::from_string(
        json[literals::Chassis::TYPE].asString()));
    chassis.set_size(json[literals::Chassis::SIZE].asUInt());
    chassis.set_location_offset(
        json[literals::Chassis::LOCATION_OFFSET].asUInt());
    chassis.set_parent_id(json[literals::Chassis::PARENT_ID].asString());
    chassis.set_power_zone(json[literals::Chassis::POWER_ZONE]);
    chassis.set_thermal_zone(json[literals::Chassis::THERMAL_ZONE]);
    chassis.set_sku(json[literals::Chassis::SKU]);
    chassis.set_asset_tag(json[literals::Chassis::ASSET_TAG]);
    chassis.set_indicator_led(json[literals::Chassis::INDICATOR_LED]);
    chassis.set_fru_info(attribute::FruInfo::from_json(
        json[literals::Chassis::FRU_INFO]));
    chassis.set_collections(Collections::from_json(
        json[literals::Chassis::COLLECTIONS]));
    chassis.set_oem(attribute::Oem::from_json(json[literals::Chassis::OEM]));
    chassis.set_resource_hash(compute_hash(json));

    return chassis;
}
