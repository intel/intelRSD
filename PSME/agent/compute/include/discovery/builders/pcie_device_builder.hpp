/*!
 * @brief Pcie device builder interface
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
 * @file pcie_device_builder.hpp
 */

#pragma once

#include "agent-framework/module/model/pcie_device.hpp"
#include "smbios/smbios_parser.hpp"

namespace agent {
namespace compute {
namespace discovery {

class PcieDeviceBuilder {
public:
    /*!
     * @brief Build default Pcie Device object
     * @return Default Pcie Device object
     */
    static agent_framework::model::PcieDevice build_default(const std::string& parent_uuid);

    /*!
     * @brief Update pcie device object with chassis UUID
     * @param pcie_device Pcie Device object to be filled.
     * @param chassis_uuid the UUID of the sled chassis.
     */
    static void update_chassis_link(agent_framework::model::PcieDevice& pcie_device, const std::string& chassis_uuid);

    /*!
     * @brief Update pcie device object with SMBIOS data.
     * @param pcie_device Pcie Device object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_data(agent_framework::model::PcieDevice& pcie_device,
                                   const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PCIE_INFO_DATA>& smbios_data);
};

}
}
}