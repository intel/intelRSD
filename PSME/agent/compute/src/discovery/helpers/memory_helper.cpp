/*!
 * @brief Helper functions for memory discoverer and builder
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file memory_helper.cpp
 */

#include "discovery/helpers/memory_helper.hpp"
#include "pmem/memory_topology.hpp"
#include "utils/conversion.hpp"
#include "utils/byte_swap.hpp"

#include <regex>
#include <unordered_map>

using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace smbios::parser;

namespace agent {
namespace compute {
namespace discovery {
namespace helpers {

bool is_present(const MEMORY_DEVICE& device) {
    return device.size > 0;
}

bool is_non_volatile_memory(const smbios::parser::MEMORY_DEVICE& device) {
    return ((device.type_detail & (uint16_t)smbios::parser::MemoryTypeDetailEnum::NON_VOLATILE) != 0);
}

OptionalField<std::uint32_t> get_memory_size_mbytes(const MEMORY_DEVICE& device) {
    std::uint32_t capacity = device.size;
    if (0xFFFF == capacity) {
        // 0xFFFF: unknown size
        log_warning("smbios-discovery", "SMBIOS reports 'unknown' memory device size!");
        return {};
    }
    else if (0x7FFF == capacity) {
        // 0x7FFF: size is (32GB-1MB) or greater - stored in extended size field
        capacity = device.extended_size;
    }
    else if (capacity & (1 << 15)) {
        // BIT15 set: size in kilobytes (Kib) - convert to megabytes (MiB)
        capacity = uint16_t(capacity / 1024);
    }
    return std::uint32_t(capacity);
}

OptionalField<DeviceType> get_device_type(const MEMORY_DEVICE& device) {
    switch (device.memory_type) {
        case MemoryTypeEnum::DDR:
            return DeviceType::DDR;
        case MemoryTypeEnum::DDR2:
            return DeviceType::DDR2;
        case MemoryTypeEnum::DDR3:
            return DeviceType::DDR3;
        case MemoryTypeEnum::DDR4:
            return DeviceType::DDR4;
        case MemoryTypeEnum::ROM:
            return DeviceType::ROM;
        case MemoryTypeEnum::SDRAM:
            return DeviceType::SDRAM;
        case MemoryTypeEnum::LOGICAL_NON_VOLATILE_DEVICE:
            return DeviceType::Logical;

        case MemoryTypeEnum::DRAM:
        case MemoryTypeEnum::OTHER:
        case MemoryTypeEnum::UNKNOWN:
        case MemoryTypeEnum::EDRAM:
        case MemoryTypeEnum::VRAM:
        case MemoryTypeEnum::SRAM:
        case MemoryTypeEnum::RAM:
        case MemoryTypeEnum::FLASH:
        case MemoryTypeEnum::EEPROM:
        case MemoryTypeEnum::FEPROM:
        case MemoryTypeEnum::EPROM:
        case MemoryTypeEnum::CDRAM:
        case MemoryTypeEnum::RAM_3D:
        case MemoryTypeEnum::SGRAM:
        case MemoryTypeEnum::RDRAM:
        case MemoryTypeEnum::DDR2_FB_DIMM:
        case MemoryTypeEnum::FBD2:
        case MemoryTypeEnum::LPDDR:
        case MemoryTypeEnum::LPDDR2:
        case MemoryTypeEnum::LPDDR3:
        case MemoryTypeEnum::LPDDR4:
        default:
            return OptionalField<DeviceType>{};
    }
}

OptionalField<MemoryModuleType> get_base_module_type(const smbios::parser::FormFactorEnum& form_factor) {
    using namespace smbios::parser;
    switch (form_factor) {
        case FormFactorEnum::DIMM:
            return MemoryModuleType::LRDIMM;
        case FormFactorEnum::SODIMM:
            return MemoryModuleType::SO_DIMM;
        case FormFactorEnum::OTHER:
        case FormFactorEnum::UNKNOWN:
        case FormFactorEnum::SIMM:
        case FormFactorEnum::SIP:
        case FormFactorEnum::CHIP:
        case FormFactorEnum::DIP:
        case FormFactorEnum::ZIP:
        case FormFactorEnum::PROPRIETARY_CARD:
        case FormFactorEnum::TSOP:
        case FormFactorEnum::ROW_OF_CHIPS:
        case FormFactorEnum::RIMM:
        case FormFactorEnum::SRIMM:
        case FormFactorEnum::FB_DIMM:
        default:
            return OptionalField<MemoryModuleType>{};
    }
}

OptionalField<Media> get_media(const smbios::parser::MemoryDeviceExtendedMedia& media) {
    using namespace smbios::parser;
    switch (media) {
        case MemoryDeviceExtendedMedia::DRAM:
            return Media::DRAM;
        case MemoryDeviceExtendedMedia::NAND:
            return Media::NAND;
        case MemoryDeviceExtendedMedia::INTEL3DXPOINT:
            return Media::Intel3DXPoint;
        case MemoryDeviceExtendedMedia::PROPRIETARY:
            return Media::Proprietary;
        default:
            return {};
    }
}

OptionalField<MemoryType> get_memory_type(const smbios::parser::MemoryDeviceExtendedType& type) {
    using namespace smbios::parser;
    switch (type) {
        case MemoryDeviceExtendedType::DIMM:
            return MemoryType::DRAM;
        case MemoryDeviceExtendedType::NVDIMM_F:
            return MemoryType::NVDIMM_F;
        case MemoryDeviceExtendedType::NVDIMM_N:
            return MemoryType::NVDIMM_N;
        case MemoryDeviceExtendedType::NVDIMM_P:
            return MemoryType::NVDIMM_P;
        case MemoryDeviceExtendedType::INTEL_PERSISTENT_MEMORY:
            return MemoryType::IntelOptane;
        default:
            return {};
    }
}

OptionalField<enums::ErrorCorrection> get_error_correction(MemoryArrays& arrays, const MEMORY_DEVICE& device) {
    auto array_handle = device.physical_memory_array_handle;
    const auto& array =
        std::find_if(arrays.cbegin(), arrays.cend(),
                     [array_handle](const SmbiosParser::StructEnhanced<SMBIOS_PHYSICAL_MEMORY_ARRAY>& entry) {
                         return entry.header.handle == array_handle;
                     }
        );

    if (arrays.cend() == array) {
        return OptionalField<enums::ErrorCorrection>{};
    }

    switch (array->data.memory_error_correction) {
        case MemoryErrorCorrectionEnum::NONE:
            return enums::ErrorCorrection::NoECC;
        case MemoryErrorCorrectionEnum::PARITY:
            return enums::ErrorCorrection::AddressParity;
        case MemoryErrorCorrectionEnum::SINGLE_BIT_ECC:
            return enums::ErrorCorrection::SingleBitECC;
        case MemoryErrorCorrectionEnum::MULTI_BIT_ECC:
            return enums::ErrorCorrection::MultiBitECC;
        case MemoryErrorCorrectionEnum::CRC:
        case MemoryErrorCorrectionEnum::OTHER:
        case MemoryErrorCorrectionEnum::UNKNOWN:
        default:
            return OptionalField<enums::ErrorCorrection>{};
    }
}

OptionalField<std::string> extract_fw_number(const std::string& org_string) {
    /* FW numbers have the following format:
     * FW API Version: aa.bb
     * FW Revision: aa.bb.cc.dddd
     * */
    std::regex valid_fw_number{R"(\d{2}(\.\d{2,4})+)"};
    std::smatch match_fw_number;

    if (std::regex_search(org_string.begin(), org_string.end(), match_fw_number, valid_fw_number)) {
        return match_fw_number.str();
    }
    return OptionalField<std::string>{};
}

OptionalField<attribute::MemoryLocation> extract_memory_location(const OptionalField<std::string>& device_locator) {
    return pmem::MemoryTopology::get_memory_location_for_locator(device_locator);
}

OptionalField<NvdimmControlRegion> get_nvdimm_control_region(const agent_framework::model::Memory& memory,
                                                             std::vector<NvdimmControlRegion>& nvdimm_control_region_array) {
    for (auto& nvdimm_control_region : nvdimm_control_region_array) {
        auto memory_serial_number = memory.get_fru_info().get_serial_number();
        auto acpi_serial_number = ::utils::int_to_hex_string<std::uint32_t>(::utils::byte_swap_32(nvdimm_control_region.data.SerialNumber));
        if (memory_serial_number == acpi_serial_number) {
            log_debug("acpi-discovery",
                      "NFIT NVDIMM Control Region matches DIMM [SerialNumber = " << acpi_serial_number << "]");
            return nvdimm_control_region;
        }
    }
    return OptionalField<NvdimmControlRegion>{};
}

OptionalField<std::vector<NvdimmRegionMapping>> get_nvdimm_region_mappings(const OptionalField<NvdimmControlRegion>& nvdimm_control_region,
                                                                           std::vector<NvdimmRegionMapping>& nvdimm_region_mapping_array) {
    if (nvdimm_control_region.has_value()) {
        std::vector<NvdimmRegionMapping> nvdimm_region_mappings_for_control_region{};
        const auto& nvdimm_control_region_index = nvdimm_control_region.value().data.NvdimmControlRegionStructureIndex;
        for (auto& nvdimm_region_mapping : nvdimm_region_mapping_array) {
            if (nvdimm_control_region_index == nvdimm_region_mapping.data.NvdimmControlRegionStructureIndex) {
                log_debug("acpi-discovery",
                          "NFIT NVDIMM Region Mapping [ID = " << nvdimm_region_mapping.data.NvdimmRegionID
                                                              << "] matches Control Region [Index = "
                                                              << nvdimm_control_region_index << "]");
                nvdimm_region_mappings_for_control_region.push_back(nvdimm_region_mapping);
            }
        }
        return nvdimm_region_mappings_for_control_region;
    }
    return OptionalField<std::vector<NvdimmRegionMapping>>{};
}

OptionalField<NvdimmSpaRange> get_spa_range_structure(const OptionalField<NvdimmRegionMapping>& nvdimm_region_mapping,
                                                      const std::vector<NvdimmSpaRange>& spa_range_array) {
    if (nvdimm_region_mapping.has_value()) {
        const auto& spa_range_structure_index = nvdimm_region_mapping.value().data.SpaRangeStructureIndex;
        for (auto& spa_range : spa_range_array) {
            if (spa_range_structure_index == spa_range.data.SpaRangeStructureIndex) {
                log_debug("acpi-discovery",
                          "NFIT NVDIMM Region Mapping [ID = " << nvdimm_region_mapping.value().data.NvdimmRegionID
                                                              << "] matches SPA Range [Index = "
                                                              << spa_range_structure_index << "]");
                return spa_range;
            }
        }
    }
    return OptionalField<NvdimmSpaRange>{};
}

OptionalField<MemoryClass> get_memory_class_of_spa_range(const OptionalField<NvdimmSpaRange>& spa_range) {
    OptionalField<MemoryClass> memory_class{};

    if (spa_range.has_value()) {
        const auto& spa_type_guid = ::utils::buffer_to_string(spa_range.value().data.AddressRangeTypeGuid,
                                                              sizeof(spa_range.value().data.AddressRangeTypeGuid),
                                                              ::utils::BufferEndianness::BigEndian);

        if (spa_type_guid == acpi::structs::NFIT_VOLATILE_MEMORY_REGION_GUID) {
            log_debug("acpi-discovery", "SPA Range Type: Volatile");
            memory_class = enums::MemoryClass::Volatile;
        }
        else if (spa_type_guid == acpi::structs::NFIT_BYTE_ADDRESSABLE_PERSISTENT_MEMORY_REGION_GUID) {
            log_debug("acpi-discovery", "SPA Range Type: ByteAccessiblePersistent");
            memory_class = enums::MemoryClass::ByteAccessiblePersistent;
        }
        else if (spa_type_guid == acpi::structs::NFIT_NVDIMM_BLOCK_DATA_WINDOW_GUID) {
            log_debug("acpi-discovery", "SPA Range Type: Block");
            memory_class = enums::MemoryClass::Block;
        }
        else if (spa_type_guid == acpi::structs::NFIT_NVDIMM_CONTROL_REGION_GUID) {
            log_debug("acpi-discovery", "SPA Range Type: NVDIMM Control Region - not supported by Redfish API");
        }
    }
    return memory_class;
}

}
}
}
}
