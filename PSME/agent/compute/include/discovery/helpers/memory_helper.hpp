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
 * @file memory_helper.hpp
 */

#pragma once


#include "agent-framework/module/model/memory.hpp"
#include "smbios/smbios_parser.hpp"
#include "acpi/acpi_parser.hpp"

namespace agent {
namespace compute {
namespace discovery {
namespace helpers {

/*!
 * @brief Checks if given memory device is present.
 * @param device SMBIOS data object containing memory device info.
 * @return true if memory device present, false otherwise.
 */
bool is_present(const smbios::parser::MEMORY_DEVICE& device);

/*!
 * @brief Checks if given memory device is non-volatile (persistent) memory.
 * @param device SMBIOS data object containing memory device info.
 * @return true if non-volatile memory, false otherwise.
 */
bool is_non_volatile_memory(const smbios::parser::MEMORY_DEVICE& device);

/*!
 * @brief Converts memory size from kilobytes to megabytes.
 * @param device SMBIOS data object containing memory device info.
 * @return memory size in MB.
 */
OptionalField<std::uint32_t> get_memory_size_mbytes(const smbios::parser::MEMORY_DEVICE& device);

/*!
 * @brief Converts memory device type from SMBIOS to Redfish standard.
 * @param device SMBIOS data object containing memory device info.
 * @return DeviceType defined by Redfish.
 */
OptionalField<agent_framework::model::enums::DeviceType> get_device_type(const smbios::parser::MEMORY_DEVICE& device);

/*!
 * @brief Converts memory module type from SMBIOS to Redfish standard.
 * @param device SMBIOS data object containing memory device info.
 * @return MemoryModuleType defined by Redfish.
 */
OptionalField<agent_framework::model::enums::MemoryModuleType> get_base_module_type(const smbios::parser::FormFactorEnum& form_factor);

/*!
 * @brief Converts memory media type from SMBIOS to Redfish standard.
 * @param device SMBIOS data object containing memory device info.
 * @return Media defined by Redfish.
 */
OptionalField<agent_framework::model::enums::Media> get_media(const smbios::parser::MemoryDeviceExtendedMedia& media);

/*!
 * @brief Converts memory type from SMBIOS to Redfish standard.
 * @param device SMBIOS data object containing memory device info.
 * @return MemoryType defined by Redfish.
 */
OptionalField<agent_framework::model::enums::MemoryType> get_memory_type(const smbios::parser::MemoryDeviceExtendedType& type);

/*!
 * @brief Converts memory error correction type from SMBIOS to Redfish standard.
 * @param device SMBIOS data object containing memory device info.
 * @return ErrorCorrection defined by Redfish.
 */
using MemoryArrays = const smbios::parser::SmbiosParser::StructVec<smbios::parser::SMBIOS_PHYSICAL_MEMORY_ARRAY>;
OptionalField<agent_framework::model::enums::ErrorCorrection> get_error_correction(MemoryArrays& arrays, const smbios::parser::MEMORY_DEVICE& device);

/*!
 * @brief Extracts FW API Version and Revision from verbose strings published in SMBIOS.
 * @param org_string original version/revision strings exposed in SMBIOS table.
 * @return String containing only relevant version/revision numbers.
 */
OptionalField<std::string> extract_fw_number(const std::string& org_string);

/*!
 * @brief Maps memory device locator published in SMBIOS to Redfish memory location type.
 * @param device_locator device locator string exposed in SMBIOS table.
 * @return MemoryLocation containing memory topology.
 */
OptionalField<agent_framework::model::attribute::MemoryLocation> extract_memory_location(const OptionalField<std::string>& device_locator);

/*!
 * @brief Seek and return ACPI NFIT NVDIMM Control Region structure for a given Memory module.
 * @param memory Memory object to bind with NVDIMM Control Region.
 * @param nvdimm_control_region_array Vector of NVDIMM Control Region structures to look through.
 * @return NvdimmControlRegion corresponding to the Memory module.
 */
using NvdimmControlRegion = acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>;
OptionalField<NvdimmControlRegion> get_nvdimm_control_region(const agent_framework::model::Memory& memory,
                                                             std::vector<NvdimmControlRegion>& nvdimm_control_region_array);

/*!
 * @brief Seek and return ACPI NFIT NVDIMM Region Mapping structures for a given Control Region.
 * @param nvdimm_control_region Control Region structure to bind with Region Mappings.
 * @param nvdimm_region_mapping_array Vector of all system's NVDIMM Region Mapping structures to look through.
 * @return NvdimmRegionMappings vector corresponding to the given NvdimmControlRegion.
 */
using NvdimmRegionMapping = acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>;
OptionalField<std::vector<NvdimmRegionMapping>> get_nvdimm_region_mappings(const OptionalField<NvdimmControlRegion>& nvdimm_control_region,
                                                                           std::vector<NvdimmRegionMapping>& nvdimm_region_mapping_array);

/*!
* @brief Seek and return ACPI NFIT SPA (System Physical Address) Range structure for a given Region Mapping.
* @param nvdimm_region_mapping Region Mapping structure to bind with SPA Range.
* @param spa_range_array Vector of all system's SPA Range structures to look through.
* @return NvdimmSpaRange corresponding to the given NvdimmRegionMapping.
*/
using NvdimmSpaRange = acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>;
OptionalField<NvdimmSpaRange> get_spa_range_structure(const OptionalField<NvdimmRegionMapping>& nvdimm_region_mapping,
                                                      const std::vector<NvdimmSpaRange>& spa_range_array);

/*!
* @brief Gets memory classification of ACPI NFIT SPA (System Physical Address) Range structure.
* @param spa_range SPA Range structure to get a memory class.
* @return MemoryClass memory classification (Volatile, ByteAccessiblePersistent or Block) of a given address range.
*/
OptionalField<agent_framework::model::enums::MemoryClass> get_memory_class_of_spa_range(const OptionalField<NvdimmSpaRange>& spa_range);

}
}
}
}