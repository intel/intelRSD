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

#pragma once

namespace psme {
namespace rest {
namespace constants {

/*!
 * @brief Constant literals for Chassis endpoint.
 */
namespace Chassis {
extern const char CHASSIS_TYPE[];
extern const char INDICATOR_LED[];
extern const char LOCATION[];
extern const char PARENT_ID[];
extern const char CONTAINS[];
extern const char COMPUTER_SYSTEMS[];
extern const char ETHERNET_SWITCHES[];
extern const char MANAGERS_IN_CHASSIS[];
extern const char POWERED_BY[];
extern const char COOLED_BY[];
extern const char THERMAL[];
extern const char POWER[];
extern const char DRIVES[];
extern const char STORAGE[];
extern const char PCIE_DEVICES[];
extern const char GEO_TAG[];
extern const char RACK_SUPPORTS_DISAGGREGATED_POWER_COOLING[];
extern const char HASH_CHASSIS_RESET[];
extern const char CHASSIS_RESET[];
extern const char POWER_STATE[];
}

/*!
 * @brief Constant literals for PowerZone endpoint.
 */
namespace PowerZone {
extern const char VOLTAGES[];
extern const char POWER_CONTROL[];
extern const char POWER_AVAILABLE_WATTS[];
extern const char POWER_CAPACITY_WATTS[];
extern const char POWER_CONSUMED_WATTS[];
extern const char POWER_CONSUMED_WATTS_PATH[];
extern const char POWER_REQUESTED_WATTS[];
extern const char POWER_ALLOCATED_WATTS[];
extern const char POWER_SUPPLIES[];
extern const char POWER_METRICS[];
extern const char POWER_LIMIT[];
extern const char INTERVAL_IN_MIN[];
extern const char MIN_CONSUMED_WATTS[];
extern const char MAX_CONSUMED_WATTS[];
extern const char AVERAGE_CONSUMED_WATTS[];
extern const char LIMIT_IN_WATTS[];
extern const char LIMIT_EXCEPTION[];
extern const char CORRECTION_IN_MS[];
extern const char POWER_SUPPLY_TYPE[];
extern const char LINE_INPUT_VOLTAGE_TYPE[];
extern const char LINE_INPUT_VOLTAGE[];
extern const char LAST_POWER_OUTPUT_WATTS[];
extern const char SPARE_PART_NUMBER[];
extern const char INPUT_RANGES[];
extern const char INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE[];
extern const char HASH_INTEL_OEM_REQUEST_POWER_SUPPLY_STATE_CHANGE[];
extern const char STATE_ALLOWABLE_VALUES[];
extern const char MEMBER_ID_ALLOWABLE_VALUES[];
extern const char INPUT_AC_POWER_WATTS[];
extern const char INPUT_AC_POWER_WATTS_PATH[];
}

/*!
 * @brief Constant literals for ThermalZone endpoint.
 */
namespace ThermalZone {
extern const char FANS[];
extern const char READING[];
extern const char READING_PATH[];
extern const char READING_UNITS[];
extern const char TEMPERATURES[];
extern const char DESIRED_SPEED_PWM[];
extern const char VOLUMETRIC_AIRFLOW_CFM[];
extern const char VOLUMETRIC_AIRFLOW_CFM_PATH[];
}

/*!
 * @brief Common constant literals for ChassisSensor.
 */
namespace ChassisSensor {
extern const char SENSOR_NUMBER[];
extern const char UPPER_THRESHOLD_NON_CRITICAL[];
extern const char UPPER_THRESHOLD_CRITICAL[];
extern const char UPPER_THRESHOLD_FATAL[];
extern const char LOWER_THRESHOLD_NON_CRITICAL[];
extern const char LOWER_THRESHOLD_CRITICAL[];
extern const char LOWER_THRESHOLD_FATAL[];
extern const char MIN_READING_RANGE[];
extern const char MAX_READING_RANGE[];
extern const char MIN_READING_RANGE_TEMP[];
extern const char MAX_READING_RANGE_TEMP[];
extern const char PHYSICAL_CONTEXT[];
extern const char READING_CELSIUS[];
extern const char READING_CELSIUS_PATH[];
extern const char READING_VOLTS[];
extern const char READING_VOLTS_PATH[];
}

/*!
 * @brief Constant literals for Chassis/n/Drive endpoint.
 */
namespace Drive {
extern const char SECURE_ERASE[];
extern const char SECURE_ERASE_ENDPOINT[];
extern const char CAPABLE_SPEED[];
extern const char INDICATOR_LED[];
extern const char ENDPOINTS[];
extern const char VOLUMES[];
extern const char MEDIA_TYPE[];
extern const char NEGOTIATED_SPEED[];
extern const char PROTOCOL[];
extern const char CAPACITY_BYTES[];
extern const char INFO[];
extern const char INFO_FORMAT[];
extern const char DRIVE_ERASED[];
extern const char FIRMWARE_VERSION[];
extern const char LATENCY_TRACKING_ENABLED[];
extern const char STATUS_INDICATOR[];
extern const char REVISION[];
extern const char FAILURE_PREDICTED[];
extern const char HOTSPARE_TYPE[];
extern const char ENCRYPTION_ABILITY[];
extern const char ENCRYPTION_STATUS[];
extern const char ROTATION_SPEED_RPM[];
extern const char BLOCK_SIZE_BYTES[];
extern const char PREDICTED_MEDIA_LIFE_LEFT[];
extern const char PCIE_FUNCTION[];
extern const char USED_BY[];
extern const char INTERFACE[];
extern const char CAPACITY[];
extern const char TYPE[];
extern const char RPM[];
extern const char BUS_INFO[];
}

/*!
 * @brief Constant literals for PcieDevice endpoint.
 */
namespace PcieDevice {
extern const char PCIE_FUNCTIONS[];
extern const char DEVICE_TYPE[];
}

/*!
 * @brief Constant literals for PcieFunction endpoint.
 */
namespace PcieFunction {
extern const char FUNCTION_ID[];
extern const char FUNCTION_TYPE[];
extern const char DEVICE_CLASS[];
extern const char DEVICE_ID[];
extern const char VENDOR_ID[];
extern const char CLASS_CODE[];
extern const char REVISION_ID[];
extern const char SUBSYSTEM_ID[];
extern const char SUBSYSTEM_VENDOR_ID[];
extern const char PCIE_DEVICE[];
}

namespace DriveMetrics {
extern const char TEMPERATURE_KELVIN[];
extern const char LIFETIME[];
extern const char HEALTH_DATA[];
extern const char UNIT_SIZE_BYTES[];
extern const char UNITS_READ[];
extern const char UNITS_WRITTEN[];
extern const char HOST_READ_COMMANDS[];
extern const char HOST_WRITE_COMMANDS[];
extern const char POWER_CYCLES[];
extern const char POWER_ON_HOURS[];
extern const char CONTROLLER_BUSY_TIME_MINUTES[];
extern const char AVAILABLE_SPARE_PERCENTAGE[];
extern const char PREDICTED_MEDIA_LIFE_USED_PERCENT[];
extern const char UNSAFE_SHUTDOWNS[];
extern const char MEDIA_ERRORS[];

}

}
}
}
