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
 * @file rmm.hpp
 * @brief Contains RMM constants used all over the model
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for Fan model objects
 */
class Fan {
public:
    static constexpr const char FAN[] = "fan";
    static constexpr const char STATUS[] = "status";
    static constexpr const char CURRENT_SPEED[] = "currentSpeed";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char OEM[] = "oem";
    static constexpr const char PHYSICAL_CONTEXT[] = "physicalContext";
    static constexpr const char CURRENT_SPEED_UNITS[] = "currentSpeedUnits";
};

/*!
 * @brief Class consisting of literals for ThermalZone model objects
 */
class ThermalZone {
public:
    static constexpr const char ZONE[] = "zone";
    static constexpr const char STATUS[] = "status";
    static constexpr const char VOLUMETRIC_AIRFLOW_CFM[] = "volumetricAirflowCfm";
    static constexpr const char DESIRED_SPEED_PWM[] = "desiredSpeedPwm";
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
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char OEM[] = "oem";
    static constexpr const char POWER_SUPPLY_TYPE[] = "powerSupplyType";
    static constexpr const char LINE_INPUT_VOLTAGE_TYPE[] = "lineInputVoltageType";
    static constexpr const char LINE_INPUT_VOLTAGE_VOLTS[] = "lineInputVoltageVolts";
    static constexpr const char LAST_POWER_OUTPUT_WATTS[] = "lastPowerOutputWatts";
    static constexpr const char POWER_CAPACITY_WATTS[] = "powerCapacityWatts";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char INDICATOR_LED[] = "indicatorLED";
    static constexpr const char REQUESTED_STATE[] = "requestedState";
};

/*!
 * @brief Class consisting of literals for PowerZone model objects
 */
class PowerZone {
public:
    static constexpr const char ZONE[] = "zone";
    static constexpr const char STATUS[] = "status";
    static constexpr const char POWER_ALLOCATED[] = "powerAllocatedWatts";
    static constexpr const char POWER_REQUESTED[] = "powerRequestedWatts";
    static constexpr const char POWER_CAPACITY[] = "powerCapacityWatts";
    static constexpr const char POWER_AVAILABLE[] = "powerAvailableWatts";
    static constexpr const char POWER_CONSUMED[] = "powerConsumedWatts";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for PowerZone model objects
 */
class ChassisSensor {
public:
    static constexpr const char SENSOR[] = "sensor";
    static constexpr const char STATUS[] = "status";
    static constexpr const char READING[] = "reading";
    static constexpr const char PHYSICAL_CONTEXT[] = "physicalContext";
    static constexpr const char READING_UNITS[] = "readingUnits";
    static constexpr const char SENSOR_NUMBER[] = "sensorNumber";
    static constexpr const char OEM[] = "oem";
};

}
}
}
