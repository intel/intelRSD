/*!
 * @brief Purley system builder class interface.
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
 * @file purley_system_builder.hpp
 */

#pragma once



#include "discovery/builders/system_builder.hpp"
#include "agent-framework/module/model/system.hpp"
#include "ipmi/command/sdv/rsd/get_system_mode.hpp"
#include "ipmi/command/sdv/rsd/read_pcie_cable_eeprom_data.hpp"



namespace agent {
namespace compute {
namespace discovery {

class PurleySystemBuilder : public SystemBuilder {
public:
    /*!
     * @brief Update system object with system mode data.
     * @param system System object to be updated with discovered data.
     * @param get_system_mode_response GetSystemMode IPMI command response object.
     */
    static void update_system_mode(agent_framework::model::System& system,
                                   const ipmi::command::sdv::rsd::response::GetSystemMode& get_system_mode_response);


    /*!
     * @brief Update system object with cable ID retrieved via IPMI.
     * @param system System object to be updated with discovered data.
     * @param read_pcie_cable_eeprom_data_response ReadPcieCableEepromData IPMI command response object.
     */
    static void add_cable_id(agent_framework::model::System& system,
                             const ipmi::command::sdv::rsd::response::ReadPcieCableEepromData& read_pcie_cable_eeprom_data_response);
};

}
}
}
