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
 *
 *
 * @file rmm.cpp
 * @brief Contains RMM constants used all over the model
 * */

#include "agent-framework/module/constants/rmm.hpp"

namespace agent_framework {
namespace model {
namespace literals {

constexpr const char Fan::STATUS[];
constexpr const char Fan::CURRENT_SPEED[];
constexpr const char Fan::FRU_INFO[];
constexpr const char Fan::OEM[];
constexpr const char Fan::FAN[];
constexpr const char Fan::PHYSICAL_CONTEXT[];
constexpr const char Fan::CURRENT_SPEED_UNITS[];

constexpr const char ThermalZone::ZONE[];
constexpr const char ThermalZone::STATUS[];
constexpr const char ThermalZone::VOLUMETRIC_AIRFLOW_CFM[];
constexpr const char ThermalZone::DESIRED_SPEED_PWM[];
constexpr const char ThermalZone::COLLECTIONS[];
constexpr const char ThermalZone::OEM[];

constexpr const char Psu::PSU[];
constexpr const char Psu::STATUS[];
constexpr const char Psu::FRU_INFO[];
constexpr const char Psu::OEM[];
constexpr const char Psu::POWER_SUPPLY_TYPE[];
constexpr const char Psu::LINE_INPUT_VOLTAGE_TYPE[];
constexpr const char Psu::LINE_INPUT_VOLTAGE_VOLTS[];
constexpr const char Psu::FIRMWARE_VERSION[];
constexpr const char Psu::LAST_POWER_OUTPUT_WATTS[];
constexpr const char Psu::POWER_CAPACITY_WATTS[];
constexpr const char Psu::INDICATOR_LED[];
constexpr const char Psu::REQUESTED_STATE[];

constexpr const char PowerZone::ZONE[];
constexpr const char PowerZone::STATUS[];
constexpr const char PowerZone::POWER_ALLOCATED[];
constexpr const char PowerZone::POWER_REQUESTED[];
constexpr const char PowerZone::POWER_AVAILABLE[];
constexpr const char PowerZone::POWER_CONSUMED[];
constexpr const char PowerZone::POWER_CAPACITY[];
constexpr const char PowerZone::COLLECTIONS[];
constexpr const char PowerZone::OEM[];

constexpr const char ChassisSensor::SENSOR[];
constexpr const char ChassisSensor::STATUS[];
constexpr const char ChassisSensor::READING[];
constexpr const char ChassisSensor::PHYSICAL_CONTEXT[];
constexpr const char ChassisSensor::READING_UNITS[];
constexpr const char ChassisSensor::SENSOR_NUMBER[];
constexpr const char ChassisSensor::OEM[];

}
}
}
