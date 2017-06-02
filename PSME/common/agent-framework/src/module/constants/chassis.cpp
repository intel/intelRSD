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
 * @file chassis.cpp
 * @brief Contains chassis constants used all over the model
 * */

#include "agent-framework/module/constants/chassis.hpp"

namespace agent_framework {
namespace model {
namespace literals {

constexpr const char Chassis::CHASSIS[];
constexpr const char Chassis::STATUS[];
constexpr const char Chassis::LOCATION_OFFSET[];
constexpr const char Chassis::PARENT_ID[];
constexpr const char Chassis::SIZE[];
constexpr const char Chassis::TYPE[];
constexpr const char Chassis::POWER_ZONE[];
constexpr const char Chassis::THERMAL_ZONE[];
constexpr const char Chassis::FRU_INFO[];
constexpr const char Chassis::COLLECTIONS[];
constexpr const char Chassis::SKU[];
constexpr const char Chassis::ASSET_TAG[];
constexpr const char Chassis::INDICATOR_LED[];
constexpr const char Chassis::OEM[];

constexpr const char AuthorizationCertificate::TYPE[];
constexpr const char AuthorizationCertificate::CERTIFICATE[];
constexpr const char AuthorizationCertificate::ENCODING_METHOD[];
constexpr const char AuthorizationCertificate::HASH[];
constexpr const char AuthorizationCertificate::HASH_METHOD[];
constexpr const char AuthorizationCertificate::OEM[];

constexpr const char Fan::STATUS[];
constexpr const char Fan::SLOT[];
constexpr const char Fan::CURRENT_SPEED[];
constexpr const char Fan::DESIRED_SPEED[];
constexpr const char Fan::FRU_INFO[];
constexpr const char Fan::OEM[];
constexpr const char Fan::FAN[];

constexpr const char ThermalZone::ZONE[];
constexpr const char ThermalZone::STATUS[];
constexpr const char ThermalZone::TEMPERATURE[];
constexpr const char ThermalZone::COLLECTIONS[];
constexpr const char ThermalZone::OEM[];

constexpr const char Psu::PSU[];
constexpr const char Psu::STATUS[];
constexpr const char Psu::SLOT[];
constexpr const char Psu::POWER_INPUT[];
constexpr const char Psu::POWER_OUTPUT[];
constexpr const char Psu::POWER_CAPACITY[];
constexpr const char Psu::FRU_INFO[];
constexpr const char Psu::OEM[];

constexpr const char PowerZone::ZONE[];
constexpr const char PowerZone::STATUS[];
constexpr const char PowerZone::POWER_INPUT[];
constexpr const char PowerZone::POWER_OUTPUT[];
constexpr const char PowerZone::POWER_CAPACITY[];
constexpr const char PowerZone::COLLECTIONS[];
constexpr const char PowerZone::OEM[];

}
}
}
