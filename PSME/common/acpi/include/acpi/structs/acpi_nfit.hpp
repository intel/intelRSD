/*!
 * @brief ACPI NFIT (NVDIMM Firmware Interface Table) structures
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
 * @file acpi_nfit.hpp
 */

#pragma once

#include <cstdint>

namespace acpi {
namespace structs {

constexpr std::size_t SYSTEM_UUID_LENGTH = 16;

#pragma pack(push, 1)

enum class NfitSubtableType: uint16_t {
    SYSTEM_PHYSICAL_ADDRESS_RANGE   = 0,
    NVDIMM_REGION_MAPPING           = 1,
    NVDIMM_CONTROL_REGION           = 4,
    NVDIMM_BLOCK_DATA_WINDOW_REGION = 5,
    PLATFORM_CAPABILITIES           = 7,
    UNSUPPORTED
};

struct NFIT_SUBTABLE_HEADER {
    NfitSubtableType type;
    uint16_t length;
};

// Type 0 - System Physical Address (SPA) Range Structure
struct NFIT_SPA_RANGE_STRUCTURE_DATA {
    uint16_t SpaRangeStructureIndex;
    uint16_t Flags;
    uint8_t  Rsvd0[4];
    uint32_t ProximityDomain;
    uint8_t  AddressRangeTypeGuid[SYSTEM_UUID_LENGTH];
    uint64_t SystemPhysicalAddressRangeBase;
    uint64_t SystemPhysicalAddressRangeLength;
    uint64_t AddressRangeMemoryMappingAttribute;
};

struct NFIT_SPA_RANGE_STRUCTURE {
    static constexpr NfitSubtableType ID = NfitSubtableType::SYSTEM_PHYSICAL_ADDRESS_RANGE;
    NFIT_SUBTABLE_HEADER header;
    NFIT_SPA_RANGE_STRUCTURE_DATA data;
};

// ACPI Specification defines the following Address Range Types GUIDs, below converted to string representation:
const char NFIT_VOLATILE_MEMORY_REGION_GUID[] = "4f940573dafde344b16c3f22d252e5d0";
const char NFIT_BYTE_ADDRESSABLE_PERSISTENT_MEMORY_REGION_GUID[] = "79d3f066f3b47440ac430d3318b78cdb";
const char NFIT_NVDIMM_CONTROL_REGION_GUID[] = "f601f792b4135d40910b299367e8234c";
const char NFIT_NVDIMM_BLOCK_DATA_WINDOW_GUID[] = "3005af91865d0e47a6b00a2db9408249";

// Type 1 - NVDIMM Region Mapping Structure
struct NFIT_NVDIMM_REGION_MAPPING_STRUCTURE_DATA {
    uint32_t NfitDeviceHandle;
    uint16_t NvdimmPhysicalID;
    uint16_t NvdimmRegionID;
    uint16_t SpaRangeStructureIndex;
    uint16_t NvdimmControlRegionStructureIndex;
    uint64_t NvdimmRegionSize;
    uint64_t RegionOffset;
    uint64_t NvdimmPhysicalAddressRegionBase;
    uint16_t InterleaveStructureIndex;
    uint16_t InterleaveWays;
    uint16_t NvdimmStateFlags;
    uint8_t  Rsvd0[2];
};

struct NFIT_NVDIMM_REGION_MAPPING_STRUCTURE {
    static constexpr  NfitSubtableType ID = NfitSubtableType::NVDIMM_REGION_MAPPING;
    NFIT_SUBTABLE_HEADER header;
    NFIT_NVDIMM_REGION_MAPPING_STRUCTURE_DATA data;
};

// Type 4 - NVDIMM Control Region Structure
struct NFIT_NVDIMM_CONTROL_REGION_STRUCTURE_DATA {
    uint16_t NvdimmControlRegionStructureIndex;
    uint16_t VendorID;
    uint16_t DeviceID;
    uint16_t RevisionID;
    uint16_t SubsystemVendorID;
    uint16_t SubsystemDeviceID;
    uint16_t SubsystemRevisionID;
    uint8_t  ValidFields;
    uint8_t  ManufacturingLocation;
    uint16_t ManufacturingDate;
    uint8_t  Rsvd0[2];
    uint32_t SerialNumber;
    uint16_t RegionFormatInterfaceCode;
    uint16_t NumberOfBlockControlWindows;
    // Below applies for 80B structure version only - not used currently
    /*
    uint64_t SizeOfBlockControlWindows;
    uint64_t CommandRegisterOffsetInBlockControlWindow;
    uint64_t SizeOfCommandRegisterInBlockControlWindows;
    uint64_t StatusRegisterOffsetInBlockControlWindow;
    uint64_t SizeOfStatusRegisterInBlockControlWindows;
    uint16_t NvdimmControlRegionFlag;
    uint8_t  Rsvd1[6];
    */
};

struct NFIT_NVDIMM_CONTROL_REGION_STRUCTURE {
    static constexpr  NfitSubtableType ID = NfitSubtableType::NVDIMM_CONTROL_REGION;
    NFIT_SUBTABLE_HEADER header;
    NFIT_NVDIMM_CONTROL_REGION_STRUCTURE_DATA data;
};

// Type 5 - NVDIMM Block Data Window Region Structure
struct NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE_DATA {
    uint16_t NvdimmControlRegionStructureIndex;
    uint16_t NumberOfBlockDataWindows;
    uint64_t BlockDataWindowStartOffset;
    uint64_t SizeOfBlockDataWindow;
    uint64_t BlockAccessibleMemoryCapacity;
    uint64_t BeginningAddressOfFirstBlockInBlockAccessibleMemory;
};

struct NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE {
    static constexpr  NfitSubtableType ID = NfitSubtableType::NVDIMM_BLOCK_DATA_WINDOW_REGION;
    NFIT_SUBTABLE_HEADER header;
    NFIT_NVDIMM_BLOCK_DATA_WINDOW_REGION_STRUCTURE_DATA data;
};

// Type 7 - Platform Capabilities Structure
struct NFIT_PLATFORM_CAPABILITIES_STRUCTURE_DATA {
    uint8_t  HighestValidCapability;
    uint8_t  Rsvs0[3];
    uint32_t Capabilities;
    uint8_t  Rsvd1[4];
};

struct NFIT_PLATFORM_CAPABILITIES_STRUCTURE {
    static constexpr  NfitSubtableType ID = NfitSubtableType::PLATFORM_CAPABILITIES;
    NFIT_SUBTABLE_HEADER header;
    NFIT_PLATFORM_CAPABILITIES_STRUCTURE_DATA data;
};

#pragma pack(pop)

}
}