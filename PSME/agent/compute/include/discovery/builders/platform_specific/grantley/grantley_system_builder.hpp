/*!
 * @brief Grantley system builder class interface.
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
 * @file grantley_system_builder.hpp
 */

#pragma once



#include "discovery/builders/system_builder.hpp"
#include "agent-framework/module/model/system.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_cable_id.hpp"



namespace agent {
namespace compute {
namespace discovery {

class GrantleySystemBuilder : public SystemBuilder {
public:
    /*!
     * @brief Update system object with BIOS data retrieved via IPMI.
     * @param system System object to be updated with discovered data.
     * @param get_bios_version_response GetBiosVersion IPMI response object.
     */
    static void update_bios_version(agent_framework::model::System& system,
                                    const ipmi::command::sdv::response::GetBiosVersion& get_bios_version_response);


    /*!
     * @brief Update system object with cable ID retrieved via IPMI.
     * @param system System object to be updated with discovered data.
     * @param get_cable_id GetCableId IPMI response object.
     */
    static void add_cable_id(agent_framework::model::System& system,
                             const ipmi::command::sdv::response::GetCableId& get_cable_id);
};

}
}
}
