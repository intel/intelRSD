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

#include "agent-framework/module-ref/model/fan.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/chassis.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Fan::component = enums::Component::Fan;
const enums::CollectionName Fan::collection_name = enums::CollectionName::Fans;

Fan::Fan(const std::string& parent_uuid) : Resource{parent_uuid} {}

Fan::~Fan() {}

Json::Value Fan::to_json() const {
    Json::Value result;
    result[literals::Fan::STATUS] = get_status().to_json();
    result[literals::Fan::SLOT] = get_slot();
    result[literals::Fan::CURRENT_SPEED] = get_current_speed();
    result[literals::Fan::DESIRED_SPEED] = get_desired_speed();
    result[literals::Fan::FRU_INFO] = get_fru_info().to_json();
    result[literals::Fan::OEM] = get_oem().to_json();
    return result;
}

Fan Fan::from_json(
    const Json::Value& json) {
    Fan fan{};

    fan.set_status(attribute::Status::from_json(json[literals::Fan::STATUS]));
    fan.set_slot(json[literals::Fan::SLOT]);
    fan.set_current_speed(json[literals::Fan::CURRENT_SPEED]);
    fan.set_desired_speed(json[literals::Fan::DESIRED_SPEED]);
    fan.set_fru_info(attribute::FruInfo::from_json(
        json[literals::Fan::FRU_INFO]));
    fan.set_oem(attribute::Oem::from_json(json[literals::Fan::OEM]));
    fan.set_resource_hash(compute_hash(json));

    return fan;
}
