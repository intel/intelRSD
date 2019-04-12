/*!
 * @brief Chassis builder class interface.
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
 * @file chassis_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/chassis.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "smbios/smbios_parser.hpp"



namespace agent {
namespace compute {
namespace discovery {

class ChassisBuilder {
public:
    /*!
     * @brief Build default chassis object.
     * @param parent_uuid Parent UUID.
     * @return Default built chassis object.
     */
    static agent_framework::model::Chassis build_default(const std::string& parent_uuid);


    /*!
     * @brief Update chassis object with SMBIOS module data.
     * @param chassis Chassis object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_module_info(agent_framework::model::Chassis& chassis,
                                          const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MODULE_INFO_DATA>& smbios_data);


    /*!
     * @brief Update chassis object with SMBIOS chassis data.
     * @param chassis Chassis object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_system_enclosure_or_chassis_info(agent_framework::model::Chassis& chassis,
                                                               const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_SYSTEM_ENCLOSURE_OR_CHASSIS>& smbios_data);


    /*!
     * @brief Update chassis object with GetDeviceId IPMI command response.
     * @param chassis Chassis object to be filled with discovered data.
     * @param get_device_id_response GetDeviceId IPMI command response object.
     */
    static void update_device_id(agent_framework::model::Chassis& chassis,
                                 const ipmi::command::generic::response::GetDeviceId& get_device_id_response);
};

}
}
}
