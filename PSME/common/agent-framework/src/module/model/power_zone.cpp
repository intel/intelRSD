/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "agent-framework/module/model/power_zone.hpp"
#include "agent-framework/module/constants/chassis.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component PowerZone::component = enums::Component::PowerZone;
const enums::CollectionName PowerZone::collection_name =
    enums::CollectionName::PowerZones;

PowerZone::PowerZone(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

PowerZone::~PowerZone() {}

Json::Value PowerZone::to_json() const {
    Json::Value result;
    result[literals::PowerZone::STATUS] = get_status().to_json();
    result[literals::PowerZone::POWER_INPUT] = get_power_input();
    result[literals::PowerZone::POWER_OUTPUT] = get_power_output();
    result[literals::PowerZone::POWER_CAPACITY] = get_power_capacity();
    result[literals::PowerZone::COLLECTIONS] = get_collections().to_json();
    result[literals::PowerZone::OEM] = get_oem().to_json();
    return result;
}

PowerZone PowerZone::from_json(const Json::Value& json) {
    PowerZone zone;

    zone.set_status(attribute::Status::from_json(json[literals::PowerZone::STATUS]));
    zone.set_power_input(json[literals::PowerZone::POWER_INPUT]);
    zone.set_power_output(json[literals::PowerZone::POWER_OUTPUT]);
    zone.set_power_capacity(
        json[literals::PowerZone::POWER_CAPACITY]);
    zone.set_collections(Collections::from_json(
        json[literals::PowerZone::COLLECTIONS]));
    zone.set_oem(attribute::Oem::from_json(json[literals::PowerZone::OEM]));
    zone.set_resource_hash(json);

    return zone;
}
