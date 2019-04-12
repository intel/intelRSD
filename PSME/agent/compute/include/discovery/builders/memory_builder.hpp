/*!
 * @brief Memory builder class interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file memory_builder.hpp
 */

#pragma once


#include "agent-framework/module/model/memory.hpp"
#include "smbios/smbios_parser.hpp"
#include "acpi/acpi_parser.hpp"


namespace agent {
namespace compute {
namespace discovery {

class MemoryBuilder {
public:

    /*!
     * @brief Build default memory object.
     * @param parent_uuid Parent UUID.
     * @return Default memory object.
     */
    static agent_framework::model::Memory build_default(const std::string& parent_uuid);


    /*!
     * @brief Update memory object with SMBIOS data.
     * @param memory Memory object to be filled with discovered data.
     * @param smbios_data SMBIOS data object containing memory device info.
     * @param arrays Vector of SMBIOS data objects containing physical memory array info.
     */
    static void update_smbios_data(agent_framework::model::Memory& memory,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE>& smbios_data,
                                   const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PHYSICAL_MEMORY_ARRAY>>& arrays);


    /*!
     * @brief Update memory object with SMBIOS memory device extended data.
     * @param memory Memory object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_memory_device_extended_data(agent_framework::model::Memory& memory,
                                                          const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>& smbios_data);

    /*!
     * @brief Update memory object with Intel Optane DC Persistent Memory general information.
     * @param memory Memory object to be filled with discovered data.
     */
    static void update_general_dcpmem_data(agent_framework::model::Memory& memory);

    /*!
     * @brief Update memory object with ACPI NFIT data.
     * @param memory Memory object to be filled with discovered data.
     * @param nfit_control_region ACPI NFIT data object containing Control Region info.
     * @param nfit_region_mapping ACPI NFIT data object containing Region Mapping info.
     * @param nfit_spa_ranges ACPI NFIT data object containing SPA Range info.
     */
    static void update_acpi_nfit_data(agent_framework::model::Memory& memory,
                                      const OptionalField<acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_NVDIMM_CONTROL_REGION_STRUCTURE>>& nfit_control_region,
                                      const OptionalField<std::vector<acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_NVDIMM_REGION_MAPPING_STRUCTURE>>>& nfit_region_mappings,
                                      const OptionalField<std::vector<acpi::parser::AcpiParser::StructEnhanced<acpi::structs::NFIT_SPA_RANGE_STRUCTURE>>>& nfit_spa_ranges);

};

}
}
}
