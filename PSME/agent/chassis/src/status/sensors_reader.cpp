/*!
 * @brief SensorsReader implementation
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
 * @file status/sensors_reader.cpp
 */

#include "status/sensors_reader.hpp"
#include "status/bmc.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "ipmi/command/generic/enums.hpp"
#include "ipmi/command/generic/get_sensor_reading.hpp"
#include "ipmi/command/generic/get_sensor_reading_factors.hpp"
#include "ipmi/command/sdv/get_fan_pwm.hpp"

#include <unordered_map>
#include <cmath>

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::chassis;
using namespace ipmi;
using namespace ipmi::command;

using agent_framework::module::ChassisComponents;
using agent_framework::module::CommonComponents;

namespace {

constexpr const std::uint8_t SLED_TYPE_HSW = 0x00;
constexpr const std::uint8_t SLED_TYPE_BDX_DE = 0x01;
constexpr const std::uint8_t SLED_TYPE_PURLEY = 0x02;

enum SENSOR_TYPE : std::uint8_t {
    INLET_TEMP,
    HSC_INPUT_POWER
};

const std::unordered_map<std::uint8_t, std::unordered_map<std::uint8_t, std::uint8_t>> sensors_map = {
    {SLED_TYPE_BDX_DE,
        {
            {INLET_TEMP, 0x07},
            {HSC_INPUT_POWER, 0x29}
        }
    },
    {SLED_TYPE_HSW,
        {
            {INLET_TEMP, 0x07},
            {HSC_INPUT_POWER, 0x29}
        }
    },
    {SLED_TYPE_PURLEY,
        {
            {INLET_TEMP, 0xA0},
            {HSC_INPUT_POWER, 0x29}
        }
    }
};


double read_sensor_value(ipmi::IpmiController& ipmi, std::uint8_t sensor_no) {
    ipmi::command::generic::request::GetSensorReading reading_req;
    ipmi::command::generic::response::GetSensorReading reading_res;
    reading_req.set_sensor_number(sensor_no);
    ipmi.send(reading_req, reading_res);
    reading_res.get_sensor_reading();

    ipmi::command::generic::request::GetSensorReadingFactors reading_factors_req;
    ipmi::command::generic::response::GetSensorReadingFactors reading_factors;
    reading_factors_req.set_sensor_number(sensor_no);
    reading_factors_req.set_reading_byte(reading_res.get_sensor_reading());
    ipmi.send(reading_factors_req, reading_factors);

    return (reading_res.get_sensor_reading() * reading_factors.get_multiplier()
            + reading_factors.get_additive_offset() * std::pow(10, reading_factors.get_exponent()))
            * std::pow(10, reading_factors.get_result_exponent());
}

uint8_t get_sled_type(std::uint16_t product_id) {
    if (ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R == product_id ||
        ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_A == product_id) {
        return SLED_TYPE_HSW;
    }
    else if (ipmi::command::generic::PRODUCT_ID_INTEL_BDX_DE_BDC_R == product_id) {
        return SLED_TYPE_BDX_DE;
    }
    else if (ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY == product_id) {
        return SLED_TYPE_PURLEY;
    }
    else {
        throw std::runtime_error("Unknown sled type");
    }
}

uint8_t get_desired_pwm(ipmi::IpmiController& ipmi) {
    sdv::request::GetFanPwm ipmi_request;
    sdv::response::GetFanPwm ipmi_response;
    ipmi.send(ipmi_request, ipmi_response);
    return ipmi_response.get_maximum_pwm();
}

}


void SensorsReader::operator()() {
    auto manager = get_manager<Manager>().get_entry(manager_uuid);
    auto platform_id = bmc.get_platform_id();
    if (manager.get_presence() && platform_id.has_value()) {
        auto type = get_sled_type(platform_id.value());
        auto pwm = get_desired_pwm(bmc.ipmi());
        auto inlet_temperature = read_sensor_value(bmc.ipmi(), sensors_map.at(type).at(INLET_TEMP));
        auto input_power = read_sensor_value(bmc.ipmi(), sensors_map.at(type).at(HSC_INPUT_POWER));

        auto chassis_keys = get_manager<Chassis>().get_keys(manager.get_uuid());
        auto power_zone_keys = get_manager<PowerZone>().get_keys(chassis_keys.front());
        auto thermal_zone_keys = get_manager<ThermalZone>().get_keys(chassis_keys.front());
        auto temperature_sensor_keys = get_manager<ChassisSensor>().get_keys(chassis_keys.front(), [](const ChassisSensor & sensor) {
            return sensor.get_reading_units() == enums::ReadingUnits::Celsius;
        });
        auto chassis = get_manager<Chassis>().get_entry_reference(chassis_keys.front());
        auto thermal_zone = get_manager<ThermalZone>().get_entry_reference(thermal_zone_keys.front());
        auto temperature_sensor = get_manager<ChassisSensor>().get_entry_reference(temperature_sensor_keys.front());
        auto power_zone = get_manager<PowerZone>().get_entry_reference(power_zone_keys.front());

        chassis->set_ipmb_type(type);
        thermal_zone->set_desired_speed_pwm(pwm);
        temperature_sensor->set_reading(inlet_temperature);
        power_zone->set_power_consumed_watts(input_power);

        log_debug("sensors-reader", bmc.get_id()
                << " sled_type: " << static_cast<uint>(type)
                << " desired_pwm: " << static_cast<uint>(pwm)
                << " inlet_temp: " << inlet_temperature
                << " power: " << input_power);
    }
}

SensorsReader::SensorsReader(const std::string& _manager_uuid, Bmc& _bmc)
    : manager_uuid(_manager_uuid), bmc(_bmc) {}
