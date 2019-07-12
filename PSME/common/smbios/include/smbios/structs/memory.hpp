/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file memory.hpp
 *
 * @brief smbios structures describing memory components
 * */

#pragma once

#include "smbios_structs.hpp"
#include <cstdint>


namespace smbios {
namespace parser {


#pragma pack(push, 1)
/*!
 * @brief Memory Array - Location
 */
enum class LocationEnum : uint8_t {
    OTHER = 1,
    UNKNOWN,
    SYSTEM_BOARD,
    ISA,
    EISA,
    PCI,
    MCA,
    PCMCIA,
    PROPRIETARY,
    NU_BUS,
    PC98_C20 = 0xA0,
    PC98_C24 = 0xA1,
    PC98_E = 0xA2,
    PC98_LOCALBUS = 0xA3
};

/*!
 * @brief Memory Array - Use
 */
enum class UseEnum : uint8_t {
    OTHER = 1,
    UNKNOWN,
    SYSTEM_MEMORY,
    VIDEO_MEMORY,
    FLASH_MEMORY,
    NON_VOLATILE_MEMORY,
    CACHE_MEMORY
};

/*!
 * @brief Memory Array - Error Correction Types
 */
enum class MemoryErrorCorrectionEnum : uint8_t {
    OTHER = 1,
    UNKNOWN,
    NONE,
    PARITY,
    SINGLE_BIT_ECC,
    MULTI_BIT_ECC,
    CRC
};

/*!
 * @brief Physical Memory Array (Type 16)
 */
struct PHYSICAL_MEMORY_ARRAY {
    LocationEnum location;
    UseEnum use;
    MemoryErrorCorrectionEnum memory_error_correction;
    std::uint32_t capacity;
    std::uint16_t memory_error_information_handle;
    std::uint16_t number_of_memory_devices;
};

/*!
 * @brief Physical Memory Array (Type 16)
 *
 * This structure describes a collection of memory devices that operate together to form a memory address space
 */
struct SMBIOS_PHYSICAL_MEMORY_ARRAY {
    static constexpr uint8_t ID = 16;
    SMBIOS_OOB_HEADER header;
    PHYSICAL_MEMORY_ARRAY data;
};

/*!
 * @brief Memory Device - Form Factor
 */
enum class FormFactorEnum : uint8_t {
    OTHER = 1,
    UNKNOWN,
    SIMM,
    SIP,
    CHIP,
    DIP,
    ZIP,
    PROPRIETARY_CARD,
    DIMM,
    TSOP,
    ROW_OF_CHIPS,
    RIMM,
    SODIMM,
    SRIMM,
    FB_DIMM};

/*!
 * @brief Memory Device - Type
 */
enum class MemoryTypeEnum : uint8_t {
    OTHER = 0x01,
    UNKNOWN = 0x02,
    DRAM = 0x03,
    EDRAM = 0x04,
    VRAM = 0x05,
    SRAM = 0x06,
    RAM = 0x07,
    ROM = 0x08,
    FLASH = 0x09,
    EEPROM = 0x0a,
    FEPROM = 0x0b,
    EPROM = 0x0c,
    CDRAM = 0x0d,
    RAM_3D = 0x0e,
    SDRAM = 0x0f,
    SGRAM = 0x10,
    RDRAM = 0x11,
    DDR = 0x12,
    DDR2 = 0x13,
    DDR2_FB_DIMM = 0x14,
    DDR3 = 0x18,
    FBD2 = 0x19,
    DDR4 = 0x1a,
    LPDDR = 0x1b,
    LPDDR2 = 0x1c,
    LPDDR3 = 0x1d,
    LPDDR4 = 0x1e,
    LOGICAL_NON_VOLATILE_DEVICE = 0x1f
};

/*!
 * @brief Memory Device - Type Detail
 */
enum class MemoryTypeDetailEnum : uint16_t {
    RESERVED = 0x01, // BIT0
    OTHER = 0x02, // BIT1
    UNKNOWN = 0x04, // BIT2
    // ... several types
    SYNCHRONOUS = 0x80, // BIT7
    NON_VOLATILE = 0x1000, // BIT12
    LRDIMM = 0x8000 // BIT15
};

/*!
 * @brief Memory Device
 *
 */
struct MEMORY_DEVICE {
    uint16_t physical_memory_array_handle;
    uint16_t memory_error_info_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;
    FormFactorEnum form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    MemoryTypeEnum memory_type;
    uint16_t type_detail;
    uint16_t speed;
    uint8_t manufacturer;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t attributes;
    uint32_t extended_size;
    uint16_t configured_memory_clock_speed;
    uint16_t voltage_min;
    uint16_t voltage_max;
    uint16_t voltage_configured;
};

/*!
 * @brief Memory Device (Type 17)
 *
 * This structure describes a single memory device that is part of a larger Physical Memory Array (Type 16).
 * Note: If a system includes memory-device sockets, the SMBIOS implementation includes a Memory Device
 * structure instance for each slot whether or not the socket is currently populated.
 *
 */
struct SMBIOS_MEMORY_DEVICE {
    static constexpr uint8_t ID = 17;
    SMBIOS_OOB_HEADER header;
    MEMORY_DEVICE data;
};

/*!
 * @brief Memory Device Extended - Type
 */
enum class MemoryDeviceExtendedType : uint8_t {
    DIMM = 0x00,
    NVDIMM_N = 0x01,
    NVDIMM_F = 0x02,
    NVDIMM_P = 0x03,
    INTEL_PERSISTENT_MEMORY = 0x07
};

/*!
 * @brief Memory Device Extended - Media
 */
enum class MemoryDeviceExtendedMedia : uint8_t {
    DRAM = 0x00,
    NAND = 0x01,
    PROPRIETARY = 0x03,
    INTEL3DXPOINT = 0x04
};

/*!
 * @brief Memory Device info data
 */
struct MEMORY_DEVICE_EXTENDED_INFO_DATA {
    uint16_t memory_device_handle;
    MemoryDeviceExtendedType type;
    MemoryDeviceExtendedMedia media;
    uint8_t firmware_revision;
    uint8_t firmware_api_version;
    uint32_t max_tdp;
    uint8_t smbus_address;
};

/*!
 * @brief Memory Device Extended (Type 197)
 *
 * This structure is used to declare extended information of the memory devices present on the system.
 * Each of the memory device extended information OEM record structure shall match the SMBIOS spec defined,
 * Type 17 memory device structure.
 *
 */
struct SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA {
    static constexpr uint8_t ID = 197;
    SMBIOS_OOB_HEADER header;
    MEMORY_DEVICE_EXTENDED_INFO_DATA data;
};



#pragma pack(pop)

}
}

