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
 * */

#include "agent-framework/module/model/psu.hpp"
#include "agent-framework/module/constants/chassis.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Psu::component = enums::Component::PSU;
const enums::CollectionName Psu::collection_name =
    enums::CollectionName::Psus;

Psu::Psu(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Psu::~Psu() {}

Json::Value Psu::to_json() const {
    Json::Value result;
    result[literals::Psu::STATUS] = get_status().to_json();
    result[literals::Psu::SLOT] = get_slot();
    result[literals::Psu::POWER_INPUT] = get_power_input();
    result[literals::Psu::POWER_OUTPUT] = get_power_output();
    result[literals::Psu::POWER_CAPACITY] = get_power_capacity();
    result[literals::Psu::FRU_INFO] = get_fru_info().to_json();
    result[literals::Psu::OEM] = get_oem().to_json();
    return result;
}

Psu Psu::from_json(const Json::Value& json) {
    Psu psu;

    psu.set_status(attribute::Status::from_json(json[literals::Psu::STATUS]));
    psu.set_slot(json[literals::Psu::SLOT].asUInt());
    psu.set_power_input(json[literals::Psu::POWER_INPUT].asDouble());
    psu.set_power_output(json[literals::Psu::POWER_OUTPUT].asDouble());
    psu.set_power_capacity(json[literals::Psu::POWER_CAPACITY].asDouble());
    psu.set_oem(attribute::Oem::from_json(json[literals::Psu::OEM]));
    psu.set_resource_hash(json);

    return psu;
}
