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

#include "agent-framework/module/model/psu.hpp"
#include "agent-framework/module/constants/rmm.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Psu::component = enums::Component::PSU;
const enums::CollectionName Psu::collection_name = enums::CollectionName::Psus;


Psu::Psu(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


Psu::~Psu() {}


json::Json Psu::to_json() const {
    json::Json result = json::Json();
    result[literals::Psu::STATUS] = get_status().to_json();
    result[literals::Psu::FRU_INFO] = get_fru_info().to_json();
    result[literals::Psu::POWER_SUPPLY_TYPE] = get_power_supply_type();
    result[literals::Psu::LINE_INPUT_VOLTAGE_TYPE] = get_line_input_voltage_type();
    result[literals::Psu::LINE_INPUT_VOLTAGE_VOLTS] = get_line_input_voltage_volts();
    result[literals::Psu::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::Psu::LAST_POWER_OUTPUT_WATTS] = get_last_power_output_watts();
    result[literals::Psu::POWER_CAPACITY_WATTS] = get_power_capacity_watts();
    result[literals::Psu::INDICATOR_LED] = get_indicator_led();
    result[literals::Psu::OEM] = get_oem().to_json();
    return result;
}


Psu Psu::from_json(const json::Json& json) {
    Psu psu;

    psu.set_status(attribute::Status::from_json(json[literals::Psu::STATUS]));
    psu.set_fru_info(attribute::FruInfo::from_json(json[literals::Psu::FRU_INFO]));
    psu.set_power_supply_type(json[literals::Psu::POWER_SUPPLY_TYPE]);
    psu.set_line_input_voltage_type(json[literals::Psu::LINE_INPUT_VOLTAGE_TYPE]);
    psu.set_line_input_voltage_volts(json[literals::Psu::LINE_INPUT_VOLTAGE_VOLTS]);
    psu.set_firmware_version(json[literals::Psu::FIRMWARE_VERSION]);
    psu.set_power_capacity_watts(json[literals::Psu::POWER_CAPACITY_WATTS]);
    psu.set_last_power_output_watts(json[literals::Psu::LAST_POWER_OUTPUT_WATTS]);
    psu.set_indicator_led(json[literals::Psu::INDICATOR_LED]);
    psu.set_oem(attribute::Oem::from_json(json[literals::Psu::OEM]));

    return psu;
}
