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
 * @file chassis.hpp
 * @brief Contains chassis constants used all over the model
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for Chassis model objects
 */
class Chassis {
public:
    static constexpr const char CHASSIS[] = "chassis";
    static constexpr const char STATUS[] = "status";
    static constexpr const char LOCATION_OFFSET[] = "locationOffset";
    static constexpr const char PARENT_ID[] = "parentId";
    static constexpr const char SIZE[] = "size";
    static constexpr const char TYPE[] = "type";
    static constexpr const char POWER_ZONE[] = "powerZone";
    static constexpr const char THERMAL_ZONE[] = "thermalZone";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char SKU[] = "sku";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char INDICATOR_LED[] = "indicatorLED";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for AuthorizationCertificate model objects
 */
class AuthorizationCertificate {
public:
    static constexpr const char TYPE[] = "certificateType";
    static constexpr const char CERTIFICATE[] = "certificate";
    static constexpr const char ENCODING_METHOD[] = "encodingMethod";
    static constexpr const char HASH[] = "certificateHash";
    static constexpr const char HASH_METHOD[] = "hashMethod";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for Fan model objects
 */
class Fan {
public:
    static constexpr const char FAN[] = "fan";
    static constexpr const char STATUS[] = "status";
    static constexpr const char SLOT[] = "slot";
    static constexpr const char CURRENT_SPEED[] = "currentSpeed";
    static constexpr const char DESIRED_SPEED[] = "desiredSpeed";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for ThermalZone model objects
 */
class ThermalZone {
public:
    static constexpr const char ZONE[] = "zone";
    static constexpr const char STATUS[] = "status";
    static constexpr const char TEMPERATURE[] = "temperature";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for Psu model objects
 */
class Psu {
public:
    static constexpr const char PSU[] = "psu";
    static constexpr const char STATUS[] = "status";
    static constexpr const char SLOT[] = "slot";
    static constexpr const char POWER_INPUT[] = "powerInput";
    static constexpr const char POWER_OUTPUT[] = "powerOutput";
    static constexpr const char POWER_CAPACITY[] = "powerCapacity";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for PowerZone model objects
 */
class PowerZone {
public:
    static constexpr const char ZONE[] = "zone";
    static constexpr const char STATUS[] = "status";
    static constexpr const char POWER_INPUT[] = "powerInput";
    static constexpr const char POWER_OUTPUT[] = "powerOutput";
    static constexpr const char POWER_CAPACITY[] = "powerCapacity";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

}
}
}


