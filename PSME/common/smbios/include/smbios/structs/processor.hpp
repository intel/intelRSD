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
 * @file processor.hpp
 * @brief Processor information (type 4)
 * */

#pragma once

#include "smbios_header.hpp"
#include <cstdint>
#pragma pack(push, 1)

namespace smbios {
namespace parser {

enum class ProcessorType : uint8_t {
    NOT_SET = 0x00,
    OTHER = 0x01,
    UNKNOWN = 0x02,
    CENTRAL = 0x03,
    MATH = 0x04,
    DSP = 0x05,
    VIDEO = 0x06
};

enum class ProcessorFamily : uint8_t {
    OTHER = 0x01,
    UNKNOWN = 0x02,
    FAMILY_8086 = 0x03,
    FAMILY_80286 = 0x04,
    INTEL_386 = 0x05,
    INTEL_486 = 0x06,
    FAMILY_8087 = 0x07,
    FAMILY_80287 = 0x08,
    FAMILY_80387 = 0x09,
    FAMILY_80487 = 0x0A,
    PENTIUM_FAMILY = 0x0B,
    PENTIUM_PRO = 0x0C,
    PENTIUM_II = 0x0D,
    PENTIUM_MMX = 0x0E,
    CELERON = 0x0F,
    PENTIUM_XEON = 0x10,
    PENTIUM_III = 0x11,
    M1_FAMILY = 0x12,
    M2_FAMILY = 0x13
};

struct PROCESSOR_INFO_DATA {
    uint8_t socket_designation;
    ProcessorType processor_type;
    ProcessorFamily processor_family;
    uint8_t processor_manufacturer;
    struct {
        uint32_t eax;
        uint32_t edx;
    } processor_id;
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed_mhz;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_update;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t core_count;
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t processor_characteristics;
    uint16_t processor_family_2;
    uint16_t core_count_2;
    uint16_t core_enabled_2;
    uint16_t thread_count_2;
};

struct SMBIOS_PROCESSOR_INFO_DATA {
    static constexpr uint8_t ID = 4;
    SMBIOS_OOB_HEADER header;
    PROCESSOR_INFO_DATA data;
};

}
}
#pragma pack(pop)
