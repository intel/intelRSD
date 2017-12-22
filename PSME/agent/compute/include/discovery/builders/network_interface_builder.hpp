/*!
 * @brief Network interface builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @file network_interface_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/network_interface.hpp"
#include "smbios/smbios_parser.hpp"



namespace agent {
namespace compute {
namespace discovery {

class NetworkInterfaceBuilder {
public:
    /*!
     * @brief Build default network inteface object.
     * @param parent_uuid Parent UUID.
     * @return Default build network interface object.
     */
    static agent_framework::model::NetworkInterface build_default(const std::string& parent_uuid);


    /*!
     * @brief Update network interface object with SMBIOS data.
     * @param network_interface Network interface object to be updated with SMBIOS data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_data(agent_framework::model::NetworkInterface& network_interface,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA>& smbios_data);


    /*!
     * @brief Update network interface object with SMBIOS data.
     * @param network_interface Network interface object to be updated with SMBIOS data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_data(agent_framework::model::NetworkInterface& network_interface,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_NIC_INFO_DATA_V2>& smbios_data);
};

}
}
}
