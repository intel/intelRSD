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
 * */

#include "agent-framework/module/model/chassis.hpp"
#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Chassis::component = enums::Component::Chassis;
const enums::CollectionName Chassis::collection_name = enums::CollectionName::Chassis;

Chassis::Chassis(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Chassis::~Chassis() {}

json::Json Chassis::to_json() const {
    json::Json json = json::Json();
    json[literals::Chassis::STATUS] = get_status().to_json();
    json[literals::Chassis::TYPE] = get_type().to_string();
    json[literals::Chassis::IS_MANAGED] = get_is_managed();
    json[literals::Chassis::LOCATION_OFFSET] = get_location_offset();
    json[literals::Chassis::PARENT_ID] = get_parent_id();
    json[literals::Chassis::LOCATION_ID] = get_location_id();
    json[literals::Chassis::POWER_ZONE] = get_power_zone();
    json[literals::Chassis::THERMAL_ZONE] = get_thermal_zone();
    json[literals::Chassis::FRU_INFO] = get_fru_info().to_json();
    json[literals::Chassis::COLLECTIONS] = get_collections().to_json();
    json[literals::Chassis::SKU] = get_sku();
    json[literals::Chassis::ASSET_TAG] = get_asset_tag();
    json[literals::Chassis::INDICATOR_LED] = get_indicator_led();
    json[literals::Chassis::GEO_TAG] = get_geo_tag();
    json[literals::Chassis::DISAGGREGATED_POWER_COOLING_SUPPORT] = get_disaggregated_power_cooling_support();
    json[literals::Chassis::ALLOWED_ACTIONS] = get_allowed_reset_actions().to_json();
    json[literals::Chassis::OEM] = get_oem().to_json();
    return json;
}

Chassis Chassis::from_json(const json::Json& json) {
    Chassis chassis{};

    chassis.set_status(attribute::Status::from_json(json[literals::Chassis::STATUS]));
    chassis.set_type(enums::ChassisType::from_string(json[literals::Chassis::TYPE]));
    chassis.set_is_managed(json[literals::Chassis::IS_MANAGED]);
    chassis.set_location_offset(json[literals::Chassis::LOCATION_OFFSET]);
    chassis.set_parent_id(json[literals::Chassis::PARENT_ID]);
    chassis.set_location_id(json[literals::Chassis::LOCATION_ID]);
    chassis.set_power_zone(json[literals::Chassis::POWER_ZONE]);
    chassis.set_thermal_zone(json[literals::Chassis::THERMAL_ZONE]);
    chassis.set_sku(json[literals::Chassis::SKU]);
    chassis.set_asset_tag(json[literals::Chassis::ASSET_TAG]);
    chassis.set_indicator_led(json[literals::Chassis::INDICATOR_LED]);
    chassis.set_geo_tag(json[literals::Chassis::GEO_TAG]);
    chassis.set_disaggregated_power_cooling_support(json[literals::Chassis::DISAGGREGATED_POWER_COOLING_SUPPORT]);
    chassis.set_fru_info(attribute::FruInfo::from_json(json[literals::Chassis::FRU_INFO]));
    chassis.set_collections(Collections::from_json(json[literals::Chassis::COLLECTIONS]));
    chassis.set_allowed_reset_actions(attribute::Array<enums::ResetType>::from_json(json[literals::Chassis::ALLOWED_ACTIONS]));
    chassis.set_oem(attribute::Oem::from_json(json[literals::Chassis::OEM]));

    return chassis;
}
