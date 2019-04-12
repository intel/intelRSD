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
 * */

#include "agent-framework/module/model/fan.hpp"
#include "agent-framework/module/constants/rmm.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Fan::component = enums::Component::Fan;
const enums::CollectionName Fan::collection_name = enums::CollectionName::Fans;

Fan::Fan(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Fan::~Fan() {}

json::Json Fan::to_json() const {
    json::Json result = json::Json();
    result[literals::Fan::STATUS] = get_status().to_json();
    result[literals::Fan::CURRENT_SPEED] = get_current_speed();
    result[literals::Fan::CURRENT_SPEED_UNITS] = get_current_speed_units();
    result[literals::Fan::PHYSICAL_CONTEXT] = get_physical_context();
    result[literals::Fan::FRU_INFO] = get_fru_info().to_json();
    result[literals::Fan::OEM] = get_oem().to_json();
    return result;
}

Fan Fan::from_json(const json::Json& json) {
    Fan fan{};

    fan.set_status(attribute::Status::from_json(json[literals::Fan::STATUS]));
    fan.set_current_speed(json[literals::Fan::CURRENT_SPEED]);
    fan.set_current_speed_units(json[literals::Fan::CURRENT_SPEED_UNITS]);
    fan.set_physical_context(json[literals::Fan::PHYSICAL_CONTEXT]);
    fan.set_fru_info(attribute::FruInfo::from_json(json[literals::Fan::FRU_INFO]));
    fan.set_oem(attribute::Oem::from_json(json[literals::Fan::OEM]));

    return fan;
}
