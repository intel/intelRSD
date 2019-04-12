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
 *
 *
 * @file enums.hpp
 *
 * @brief IPMI generic enums.
 * */

#pragma once

#include <cstdint>

namespace ipmi {
namespace command {
namespace generic {

/*!
 * @brief Represents codes for Network Function tailored to IPMI spec.
 */
enum NetFn : std::uint8_t {
    CHASSIS = 0x00,
    SENSOR_EVENT = 0x04,
    APP = 0x06,
    STORAGE   = 0x0A,
    TRANSPORT = 0x0C
};

/*!
 * @brief Represents codes for IPMI Command.
 */
enum Cmd : std::uint8_t {
    GET_CHASSIS_STATUS = 0x01,
    GET_DEVICE_ID = 0x01,
    GET_THERMAL_STATE = 0x08,
    SET_BOOT_ORDER = 0x08,
    GET_BOOT_ORDER = 0x09,
    GET_SENSOR_READING_FACTORS = 0x23,
    GET_SENSOR_READING = 0x2D,
    CHASSIS_CONTROLL_COMMAND = 0x02,
    GET_SYSTEM_GUID = 0x37,
    GET_DEVICE_GUID = 0x08,
    MASTER_WRITE_READ = 0x52,
    GET_LAN_CONFIG_PARAMS = 0x02,
    WARM_RESET = 0x03,

    /*! @{ @i{STORAGE netfn} */

    /* SDR Device Commands */
    GET_SDR_REPOSITORY_INFO = 0x20,
    RESERVE_SDR_REPOSITORY = 0x22,
    GET_SDR = 0x23,

    /* FRU Device Commands */
    GET_FRU_INVENTORY_AREA_INFO = 0x10,
    READ_FRU_DATA = 0x11,

    /* SEL Device Commands */
    GET_SEL_INFO = 0x40,
    GET_SEL_ALLOCATION_INFO = 0x41,
    RESERVE_SEL = 0x42,
    GET_SEL_ENTRY = 0x43,
    ADD_SEL_ENTRY = 0x44,
    PARTIAL_ADD_SEL_ENTRY = 0x45,
    DELETE_SEL_ENTRY = 0x46,
    CLEAR_SEL = 0x47,
    GET_SEL_TIME = 0x48,
    SET_SEL_TIME = 0x49,
    GET_AUXILIARY_LOG_STATUS = 0x5A,
    SET_AUXILIARY_LOG_STATUS = 0x5B,
    GET_SEL_TIME_UTC_OFFSET = 0x5C,
    SET_SEL_TIME_UTC_OFFSET = 0x5D

    /*! @} */
};

/*!
 * @brief Represents available boot override targets
 */
enum class BootOverrideTarget : std::uint8_t {
    None,
    Hdd,
    RemoteDrive,
    Pxe,
    Other
};

/*!
 * @brief Represents available boot override types
 */
enum class BootOverride : std::uint8_t {
    Disabled,
    Once,
    Continuous
};

/*!
 * @brief Represents available Boot Modes
 */
enum class BootMode : std::uint8_t {
    Legacy,
    UEFI
};

enum class SensorType : std::uint8_t {
    RESERVED = 0x00,
    TEMPERATURE = 0x01,
    VOLTAGE = 0x02,
    CURRENT = 0x03,
    FAN = 0x04,
    PHYSICAL_SECURITY = 0x05,
    PERFORM_SECURITY_VIOLATION = 0x06,
    PROCESSOR = 0x07,
    POWER_SUPPLY = 0x08,
    POWER_UNIT = 0x09,
    COOLING_DEVICE = 0x0a,
    OTHER_UNIT_BASED_SENSOR = 0x0b,
    MEMORY = 0x0c,
    DRIVE_SLOT = 0x0d,
    POST_MEMORY_RESIZE = 0x0e,
    POST_ERRORS = 0x0f,
    SEL_LOGGING = 0x10,
    WATCHDOG1 = 0x11,
    SYSTEM_EVENT = 0x12,
    CRITICAL_INTERRUPT = 0x13,
    BUTTON = 0x14,
    MODULE_BOARD = 0x15,
    COPROCESSOR = 0x16,
    ADDIN_CARD = 0x17,
    CHASSIS = 0x18,
    CHIPSET = 0x19,
    OTHER_FRU = 0x1a,
    INTERCONNECT = 0x1b,
    TERMINATOR = 0x1c,
    SYSTEM_BOOT_RESTART_INITIATED = 0x1d,
    BOOT_ERROR = 0x1e,
    OS_BOOT_STATUS = 0x1f,
    OS_STOP_SHUTDOWN = 0x20,
    CONNECTOR = 0x21,
    SYSTEM_ACPI_POWER_STATE = 0x22,
    WATCHDOG2 = 0x23,
    PLATFORM_ALERT = 0x24,
    ENTITY_PRESENCE = 0x25,
    MONITOR_ASIC = 0x26,
    LAN = 0x27,
    MANAGEMENT_SUBSYSTEM_HEALTH = 0x28,
    BATTERY = 0x29,
    SESSION_AUDIT = 0x2a,
    VERSION_CHANGE = 0x2b,
    FRU_STATE = 0x2c,
    /* 2d-bf reserved for OEMs */
    OEM_RESERVED_FAMILY = 0xc0, /* up to 0xff */
    OEM_C0 = 0xC0,
    OEM_C1 = 0xC1,
    OEM_C2 = 0xC2,
    OEM_C3 = 0xC3,
    OEM_C5 = 0xC5,
    OEM_C6 = 0xC6,
    OEM_C8 = 0xC8,
    OEM_CA = 0xCA,
    OEM_DC = 0xDC

};

/*!
 * @brief Represents module's type.
 */
enum ProductId : std::uint16_t {
    PRODUCT_ID_INTEL_XEON_BDC_R = 0x2041,
    PRODUCT_ID_INTEL_XEON_BDC_A = 0x3036,
    PRODUCT_ID_INTEL_XEON_PURLEY = 0x3038,
    PRODUCT_ID_INTEL_ATOM = 0x3146,
    PRODUCT_ID_INTEL_BDX_DE_BDC_R = 0xF20A,
    PRODUCT_ID_INTEL_LAST,
    PRODUCT_ID_INTEL_UNKNOWN,
};

/*!
 * @brief Represents module's type.
 */
enum ManufacturerId : std::uint32_t {
    MANUFACTURER_ID_INTEL = 0x000157,
    MANUFACTURER_ID_QUANTA = 0x001C4C,
    MANUFACTURER_ID_LAST,
    MANUFACTURER_ID_UNKNOWN,
};

enum class BmcInterface : std::uint16_t {
    RSA_1_2,
    RSD_2_2,
    RSD_2_4,
    UNKNOWN,
};

}
}
}
