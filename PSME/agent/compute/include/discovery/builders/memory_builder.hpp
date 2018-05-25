/*!
 * @brief Memory builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file memory_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/memory.hpp"
#include "smbios/smbios_parser.hpp"



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
};

}
}
}
