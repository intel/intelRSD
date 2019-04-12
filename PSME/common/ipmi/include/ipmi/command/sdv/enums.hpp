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
 * @brief IPMI sdv enums.
 * */

#pragma once

namespace ipmi {
namespace command {
namespace sdv {

/*!
 * @brief Represents codes for Network Function tailored to IPMI spec.
 */
enum NetFn : std::uint8_t {
    QUANTA = 0x30,
    INTEL = 0x38,
    RACKSCALE = 0x3A,
    INTEL_NM = 0x2E,
    GROUP_EXTENSION = 0x2C
};

/*!
 *  * @brief Represents codes for defining entities within group extension net
 *  function
 *   */
enum class GroupExtension : uint8_t {
    RACKSCALE = 0x04
};

/*!
 * @brief Represents codes for IPMI Command.
 */
enum Cmd : std::uint8_t {
    GET_THERMAL_STATE = 0x08,
    GET_FAN_PWM = 0x01,
    GET_SLOT_ID = 0x09,
    GET_NODE_INFO = 0x0A,
    GET_BIOS_VERSION = 0x83,
    GET_PROCESSOR_INFO = 0x1B,
    GET_DIMM_INFO = 0x1D,
    GET_SYSTEM_MAC_INFO = 0x19,
    SET_ISCSI_FIELD = 0x02,
    GET_ISCSI_FIELD = 0x03,
    SET_OOB_BOOT_OPTION = 0x14,
    GET_OOB_BOOT_OPTION = 0x15,
    GET_CABLE_ID = 0x16,
    GET_MDR_DATA_REGION_STATUS = 0x20,
    MDR_REGION_READ = 0x22,
    GET_CHASSIS_PRESENCE = 0x02,
    GET_FAN_PRESENCE = 0x05,
    GET_PSU_PRESENCE = 0x0B,
    GET_PZONE_PRESENCE = 0x0A,
    GET_CHASSIS_ALERT = 0x03,
    GET_PWM = 0x06,
    SET_PWM = 0x07,
    SET_ID_FIELD = 0x06,
    GET_FAN_TACHS = 0x08,
    GET_PSSHELF_CRITICAL = 0x0D,
    SEND_PSU_COMMAND = 0x0E,
    GET_AGGREGATED_THERMAL_SENSOR = 0xC1,
    GET_AGGREGATED_HEALTH_SENSOR = 0xC2,
    GET_DRAWER_POWER = 0xC3,
    GET_AUTHORIZED_CERT_HASH = 0xC6,
    GET_AUTHORIZED_CERT = 0xC5,
    SET_AUTHORIZED_CERT = 0xC4,
    RESET = 0x01,
    GET_ID_FIELD = 0x07,
    GET_PS_DC_CURRENT = 0x21,
    GET_PS_INPUT_POWER = 0x25,
    GET_CPU_AND_MEMORY_TEMPERATURE = 0x4B,
    TRIGGER_DCPMEM_PROXY_ACCESS = 0x49,
    GET_DCPMEM_READINGS = 0x4A,
    UPDATE_READING_PACKAGE = 0x50,
    QUERY_READING_PACKAGE = 0x51,
    GET_TELEMETRY_READINGS = 0x52,
    GET_SYSTEM_MODE = 0x25,
    SET_SYSTEM_MODE = 0x26,
    GET_CPU_PECI_PACKAGE_CONFIG_DATA = 0x1F,
    GET_TPM_CONFIGURATION = 0x29,
    SET_TPM_CONFIGURATION = 0x30,
    READ_PCIE_CABLE_EEPROM_DATA = 0x37,
    MDR_REGION_UPDATE_COMPLETE = 0x21,
    MDR_REGION_LOCK = 0x24,
    MDR_REGION_WRITE = 0x23,
    GET_ACPI_TABLES_NAMES = 0x32,
    SET_ACPI_TABLES_NAMES = 0x33,
    GET_DCPMEM_CONFIGURATION = 0x34,
    SET_DCPMEM_CONFIGURATION = 0x35,
    GET_SPEED_SELECT_CONFIGURATION = 0x40,
    SET_SPEED_SELECT_CONFIGURATION = 0x41
};

/*!
 * @brief Data Region IDs
 */
enum DataRegionId : std::uint8_t {
    SMBIOS_TABLE = 0x1,
    ACPI_TABLE = 0x2,
    ISCSI_BOOT_OPTIONS = 0x4
};

/*!
 * @brief MDR data valid flag
 */
enum DataValidation : std::uint8_t {
    INVALID = 0x0,
    VALID = 0x1
};

/*!
 * @brief MDR region lock status
 */
enum LockStatus : std::uint8_t {
    UNLOCKED = 0x0,
    STRICT_LOCK = 0x1,
    PREEMPTIBLE_LOCK = 0x2
};

/*!
 * @brief MDR region lock type
 */
enum class LockType : std::uint8_t {
    ABORT = 0x0,
    STRICK_LOCK = 0x1,
    PREEMPTIBLE_LOCK = 0x2
};

/*!
 * @brief Certificate types
 */
enum class CertificateType : std::uint8_t {
    PODM = 0x00,
    RMM = 0x01
};

enum class IdFieldType : std::uint8_t {
    RESERVED = 0x00,
    RACK_PUID = 0x01,
    RACK_BPID = 0x02,
    TRAY_PUID = 0x03,
    RACK_ID = 0x04
};

enum class SystemMode : std::uint8_t {
    ADMIN_MODE = 0x00,
    USER_MODE = 0x01
};

enum class TpmCommandStatus : std::uint8_t {
    COMMAND_INVALID = 0x00,
    COMMAND_VALID = 0x01
};

enum class ClearTpmAction : std::uint8_t {
    PRESERVE_USER_TPM_OWNERSHIP = 0x00,
    CLEAR_USER_TPM_OWNERSHIP = 0x01
};

/*!
 * @brief Intel Optane DC Persistent Memory Command Status
 */
enum class DcpmemConfigurationCommandStatus : std::uint8_t {
    INVALID = 0x00,
    VALID = 0x01
};

/*!
 * @brief Intel Optane DC Persistent Memory Action Type
 */
enum class EraseDcpmemAction : std::uint8_t {
    PRESERVE_EXISTING = 0x00,
    SECURE_ERASE = 0x01,  // BIT0 set
    OVERWRITE_PCD = 0x02, // BIT1 set
};

/*! @brief Indicates no reading available */
constexpr const std::uint8_t NO_READING = 0xff;

}
}
}
