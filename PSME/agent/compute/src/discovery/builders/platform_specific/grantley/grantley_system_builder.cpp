/*!
 * @brief Grantley system builder class implementation.
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

#include "discovery/builders/platform_specific/grantley/grantley_system_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;



void GrantleySystemBuilder::update_bios_version(agent_framework::model::System& system,
                                        const ipmi::command::sdv::response::GetBiosVersion& get_bios_version_response) {
    system.set_bios_version(get_bios_version_response.get_version());
}


void GrantleySystemBuilder::add_cable_id(agent_framework::model::System& system,
                                 const ipmi::command::sdv::response::GetCableId& get_cable_id) {
    system.add_cable_id(get_cable_id.get_cable_id());
}
