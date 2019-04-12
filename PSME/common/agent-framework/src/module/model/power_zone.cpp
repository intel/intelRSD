/*!
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
 *
 * */

#include "agent-framework/module/model/power_zone.hpp"
#include "agent-framework/module/constants/rmm.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component PowerZone::component = enums::Component::PowerZone;
const enums::CollectionName PowerZone::collection_name =
    enums::CollectionName::PowerZones;

PowerZone::PowerZone(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

PowerZone::~PowerZone() {}

json::Json PowerZone::to_json() const {
    json::Json result = json::Json();

    result[literals::PowerZone::STATUS] = get_status().to_json();
    result[literals::PowerZone::POWER_CONSUMED] = get_power_consumed_watts();
    result[literals::PowerZone::POWER_REQUESTED] = get_power_requested_watts();
    result[literals::PowerZone::POWER_CAPACITY] = get_power_capacity_watts();
    result[literals::PowerZone::POWER_ALLOCATED] = get_power_allocated_watts();
    result[literals::PowerZone::POWER_AVAILABLE] = get_power_available_watts();
    result[literals::PowerZone::COLLECTIONS] = get_collections().to_json();
    result[literals::PowerZone::OEM] = get_oem().to_json();
    return result;
}

PowerZone PowerZone::from_json(const json::Json& json) {
    PowerZone zone{};

    zone.set_status(attribute::Status::from_json(json[literals::PowerZone::STATUS]));
    zone.set_power_allocated_watts(json[literals::PowerZone::POWER_ALLOCATED]);
    zone.set_power_available_watts(json[literals::PowerZone::POWER_AVAILABLE]);
    zone.set_power_capacity_watts(json[literals::PowerZone::POWER_CAPACITY]);
    zone.set_power_requested_watts(json[literals::PowerZone::POWER_REQUESTED]);
    zone.set_power_consumed_watts(json[literals::PowerZone::POWER_CONSUMED]);
    zone.set_collections(Collections::from_json(json[literals::PowerZone::COLLECTIONS]));
    zone.set_oem(attribute::Oem::from_json(json[literals::PowerZone::OEM]));

    return zone;
}
