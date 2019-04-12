/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "agent-framework/module/model/chassis_sensor.hpp"
#include "agent-framework/module/constants/rmm.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component ChassisSensor::component = enums::Component::ChassisSensor;
const enums::CollectionName ChassisSensor::collection_name = enums::CollectionName::ChassisSensors;

ChassisSensor::ChassisSensor(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

ChassisSensor::~ChassisSensor() {}

json::Json ChassisSensor::to_json() const {
    json::Json result = json::Json();
    result[literals::ChassisSensor::STATUS] = get_status().to_json();
    result[literals::ChassisSensor::READING] = get_reading();
    result[literals::ChassisSensor::READING_UNITS] = get_reading_units();
    result[literals::ChassisSensor::PHYSICAL_CONTEXT] = get_physical_context();
    result[literals::ChassisSensor::SENSOR_NUMBER] = get_sensor_number();
    result[literals::ChassisSensor::OEM] = get_oem().to_json();
    return result;
}

ChassisSensor ChassisSensor::from_json(const json::Json& json) {
    ChassisSensor chassis_sensor{};

    chassis_sensor.set_status(attribute::Status::from_json(json[literals::ChassisSensor::STATUS]));
    chassis_sensor.set_reading(json[literals::ChassisSensor::READING]);
    chassis_sensor.set_reading_units(json[literals::ChassisSensor::READING_UNITS]);
    chassis_sensor.set_physical_context(json[literals::ChassisSensor::PHYSICAL_CONTEXT]);
    chassis_sensor.set_sensor_number(json[literals::ChassisSensor::SENSOR_NUMBER]);
    chassis_sensor.set_oem(attribute::Oem::from_json(json[literals::ChassisSensor::OEM]));

    return chassis_sensor;
}
