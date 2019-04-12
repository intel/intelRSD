/*!
 * @brief Drive builder class interface.
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
 * @file drive_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/drive.hpp"
#include "smbios/smbios_parser.hpp"



namespace agent {
namespace compute {
namespace discovery {

class DriveBuilder {
public:
    /*!
     * @brief Build default drive object.
     * @param parent_uuid Parent UUID.
     * @return Default built drive object.
     */
    static agent_framework::model::Drive build_default(const std::string& parent_uuid);


    /*!
     * @brief Update drive object with SMBIOS drive v1 data.
     * @param drive Drive object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_data(agent_framework::model::Drive& drive,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA>& smbios_data);


    /*!
     * @brief Update drive object with SMBIOS drive v2 data.
     * @param drive Drive object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_data(agent_framework::model::Drive& drive,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA_V2>& smbios_data);


    /*!
     * @brief Update drive object with SMBIOS storage device info data.
     * @param drive Drive object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_data(agent_framework::model::Drive& drive,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_STORAGE_DEVICE_INFO_DATA>& smbios_data);
};

}
}
}
