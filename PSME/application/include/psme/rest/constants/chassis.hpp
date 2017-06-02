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

#pragma once

namespace psme {
namespace rest {
namespace constants {

/*!
 * @brief Constant literals for Chassis endpoint.
 */
namespace Chassis {
extern const char CHASSIS_TYPE[];
extern const char SKU[];
extern const char INDICATOR_LED[];
extern const char LOCATION[];
extern const char PARENT_ID[];
extern const char CONTAINS[];
extern const char COMPUTER_SYSTEMS[];
extern const char SWITCHES[];
extern const char MANAGERS_IN_CHASSIS[];
extern const char THERMAL_ZONES[];
extern const char POWER_ZONES[];
extern const char DRIVES[];
extern const char STORAGE[];
extern const char PCIE_DEVICES[];
}

/*!
 * @brief Common constant literals for PowerZone and ThermalZone endpoints.
 */
namespace Zone {
extern const char RACK_LOCATION[];
extern const char UHEIGHT[];
extern const char ULOCATION[];
extern const char UNIT[];
extern const char XLOCATION[];
extern const char MEMBER_ID[];
extern const char RELATED_ITEM[];
}

/*!
 * @brief Constant literals for PowerZone endpoint.
 */
namespace PowerZone {
extern const char POWER_CONTROL[];
extern const char MAX_PSUS_SUPPORTED[];
extern const char POWER_AVAILABLE[];
extern const char POWER_CAPACITY[];
extern const char POWER_CONSUMED[];
extern const char POWER_SUPPLIES[];
extern const char LAST_POWER_OUTPUT[];
}

/*!
 * @brief Constant literals for ThermalZone endpoint.
 */
namespace ThermalZone {
extern const char FANS[];
extern const char FAN_NAME[];
extern const char PHYSICAL_CONTEXT[];
extern const char READING[];
extern const char READING_UNITS[];
extern const char DESIRED_SPEED_RPM[];
extern const char MAX_FANS_SUPPORTED[];
extern const char PWM[];
extern const char POLICY[];
extern const char TEMPERATURES[];
extern const char READING_CELCIUS[];
extern const char SENSOR_NUMBER[];
}

/*!
 * @brief Constant literals for Chassis/n/Drive endpoint.
 */
namespace PncDrive {
extern const char SECURE_ERASE[];
extern const char SECURE_ERASE_ENDPOINT[];
extern const char TARGET[];
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
extern const char ERASE_ON_DETACH[];
extern const char DRIVE_ERASED[];
extern const char FIRMWARE_VERSION[];
extern const char IDENTIFIERS[];
extern const char DURABLE_NAME[];
extern const char DURABLE_NAME_FORMAT[];
extern const char SKU[];
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

}
}
}
