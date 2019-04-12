/*!
 * @brief ACPI PCAT (Platform Configuration Attributes Table) structures
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file acpi_pcat.hpp
 */

#pragma once

#include <cstdint>

namespace acpi {
namespace structs {

#pragma pack(push, 1)

enum class PcatSubtableType: uint16_t {
    PLATFORM_CAPABILITY_INFORMATION               = 0,
    SOCKET_SKU_INFORMATION                        = 6,
    UNSUPPORTED
};

struct PCAT_SUBTABLE_HEADER {
    PcatSubtableType type;
    uint16_t length;
};

// Type 0 - Platform Capability Information Table
struct PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE_DATA {
    uint8_t IntelNvdimmManagementSwConfigInputSupport;
    uint8_t MemoryModeCapabilities;
    uint8_t CurrentMemoryMode;
    uint8_t PersistentMemoryRasCapability;
    uint8_t Rsvd0[8];
};

struct PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE {
    static constexpr PcatSubtableType ID = PcatSubtableType::PLATFORM_CAPABILITY_INFORMATION;
    PCAT_SUBTABLE_HEADER header;
    PCAT_PLATFORM_CAPABILITY_INFORMATION_STRUCTURE_DATA data;
};

// Type 6 - Socket SKU Information Table
struct PCAT_SOCKET_SKU_INFORMATION_STRUCTURE_DATA {
    uint16_t SocketID;
    uint8_t  Rsvd0[2];
    uint64_t MappedMemorySizeLimit;
    uint64_t TotalMemorySizeMappedIntoSpa;
    uint64_t MemorySizeExcludedInLimitCalculationsWhenIn2LmMode;
};

struct PCAT_SOCKET_SKU_INFORMATION_STRUCTURE {
    static constexpr  PcatSubtableType ID = PcatSubtableType::SOCKET_SKU_INFORMATION;
    PCAT_SUBTABLE_HEADER header;
    PCAT_SOCKET_SKU_INFORMATION_STRUCTURE_DATA data;
};

#pragma pack(pop)

}
}