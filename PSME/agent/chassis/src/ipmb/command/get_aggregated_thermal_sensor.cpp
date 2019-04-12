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
 *
 * @file get_aggregated_thermal_sensor.cpp
 * @brief GetAggregatedThermalSensor command handler for RMM.
 * */

#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"

#include <ipmb/utils.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/command/get_aggregated_thermal_sensor.hpp>

#include <bitset>



using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;
using namespace agent_framework::model;
using namespace agent_framework::module;

using agent_framework::module::get_manager;
using namespace agent_framework::model;


GetAggregatedThermalSensor::~GetAggregatedThermalSensor() {}


void GetAggregatedThermalSensor::unpack(IpmiMessage& msg) {
    log_debug(LOGUSR, "Unpacking Get Aggregated Thermal Sensor message.");
    msg.set_to_request();
}


void GetAggregatedThermalSensor::pack(IpmiMessage& msg) {

    log_debug(LOGUSR, "Packing Get Aggregated Thermal Sensor message.");

    ThermalSensorIpmbResponse response{};
    msg.set_to_response();

    make_response(response);

    // 1 byte bitmap present
    // + size of sled thermal data
    auto len = 1 + response.sled_count * sizeof(ThermalSensorIpmbResponse::Thermal);

    // set cc and data
    uint8_t* data = msg.get_data();
    *(data++) = static_cast<uint8_t>(CompletionCode::CC_OK);
    const uint8_t* src = reinterpret_cast<const uint8_t*>(&response);
    std::copy(src, src + len, data);

    // 1 byte completion code
    // + size of data
    msg.add_len(static_cast<uint16_t>(1 + len));

}


void GetAggregatedThermalSensor::make_response(ThermalSensorIpmbResponse& response) {
    uint8_t sled_presence_bit_map = 0;
    uint8_t sled_presence_mask = 1;

    auto drawer_manager_keys = get_manager<Manager>().get_keys("");
    auto sled_manager_keys = get_manager<Manager>().get_keys(drawer_manager_keys.front());

    for (const auto& key: sled_manager_keys) {
        auto manager = get_manager<Manager>().get_entry(key);

        if (manager.get_presence()) {
            sled_presence_bit_map = uint8_t(sled_presence_bit_map | sled_presence_mask << (manager.get_slot() - 1));
            log_debug(LOGUSR, "Sled presence mask: " << std::to_string(static_cast<uint>(sled_presence_mask))
                                                     << " Sled presence bit map: "
                                                     << std::to_string(static_cast<uint>(sled_presence_bit_map)));
            auto sled_chassis_keys = get_manager<Chassis>().get_keys(manager.get_uuid());

            auto thermal_zone_keys = get_manager<ThermalZone>().get_keys(sled_chassis_keys.front());

            auto temperature_sensor_keys = get_manager<ChassisSensor>()
                .get_keys(sled_chassis_keys.front(), [](const ChassisSensor& sensor) {
                return sensor.get_reading_units() == enums::ReadingUnits::Celsius;
            });

            auto chassis = get_manager<Chassis>().get_entry(sled_chassis_keys.front());
            auto thermal_zone = get_manager<ThermalZone>().get_entry(thermal_zone_keys.front());
            auto temperature_sensor = get_manager<ChassisSensor>().get_entry(
                temperature_sensor_keys.front());

            auto desired_speed_pwm = thermal_zone.get_desired_speed_pwm();
            auto temperature = temperature_sensor.get_reading();

            response.thermal_data[response.sled_count].sled_type = uint8_t(chassis.get_ipmb_type());
            response.thermal_data[response.sled_count].desired_pwm = uint8_t(
                desired_speed_pwm.has_value() ? desired_speed_pwm.value() : 0xff);
            response.thermal_data[response.sled_count].inlet_temp = uint8_t(
                temperature.has_value() ? temperature.value() : 0xff);

            response.sled_count++;
            sled_presence_mask = 1;
        }
    }

    response.sled_presence_bitmap = sled_presence_bit_map;
}
