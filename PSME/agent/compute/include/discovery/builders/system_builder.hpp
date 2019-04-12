/*!
 * @brief System builder class interface.
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
 * @file system_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/system.hpp"
#include "ipmi/command/generic/get_system_guid.hpp"
#include "smbios/smbios_parser.hpp"



namespace agent {
namespace compute {
namespace discovery {

class SystemBuilder {
public:
    /*!
     * @brief Build default system object.
     * @param parent_uuid Parent UUID.
     * @param chassis_uuid Enclosing chassis UUID.
     * @return Built system object.
     */
    static agent_framework::model::System
    build_default(const std::string& parent_uuid, const std::string& chassis_uuid);


    /*!
     * @brief Update system object with SMBIOS system data.
     * @param system System object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_system_info(agent_framework::model::System& system,
                                          const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_SYSTEM_INFO_DATA>& smbios_data);


    /*!
     * @brief Update system object with SMBIOS module data.
     * @param system System object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_module_info(agent_framework::model::System& system,
                                          const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MODULE_INFO_DATA>& smbios_data);


    /*!
     * @brief Update system object with SMBIOS bios data.
     * @param system System object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_bios_info(agent_framework::model::System& system,
                                        const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_BIOS_INFO_DATA>& smbios_data);


    /*!
     * @brief Update system object with SMBIOS PCIe info data.
     * @param system System object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_pcie_info(agent_framework::model::System& system,
                                        const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>>& smbios_data);


    /*!
     * @brief Update system object with SMBIOS TXT module info data.
     * @param system System object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_txt_info(agent_framework::model::System& system,
                                       const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_TXT_INFO_DATA>& smbios_data);


    /*!
     * @brief Update system object with GUID data retrieved via IPMI.
     * @param system System object to be updated with discovered data.
     * @param get_system_guid_response GetSystemGuid IPMI reponse object.
     */
    static void update_system_guid(agent_framework::model::System& system,
                                   const ipmi::command::generic::response::GetSystemGuid& get_system_guid_response);


    /*!
     * @brief Update system object with SMBIOS speedselect info data.
     * @param system System object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_performance_configurations(agent_framework::model::System& system,
                                                         const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_SPEED_SELECT_INFO_DATA>& smbios_data);
};

}
}
}
