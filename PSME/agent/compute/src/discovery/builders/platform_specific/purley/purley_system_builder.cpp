/*!
 * @brief Purley system builder class implementation.
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
 * @file purley_system_builder.cpp
 */

#include "discovery/builders/platform_specific/purley/purley_system_builder.hpp"

#include <algorithm>



using namespace agent::compute::discovery;
using namespace agent_framework::model;


void PurleySystemBuilder::update_system_mode(agent_framework::model::System& system,
                                             const ipmi::command::sdv::rsd::response::GetSystemMode& get_system_mode_response) {
    bool user_mode_enabled =
        ipmi::command::sdv::SystemMode::USER_MODE == get_system_mode_response.get_system_mode() ? true : false;
    bool rackscale_mode_enabled = get_system_mode_response.is_rackscale_mode_enabled();

    system.set_user_mode_enabled(user_mode_enabled);
    system.set_rackscale_mode_enabled(rackscale_mode_enabled);
}


void PurleySystemBuilder::add_cable_id(agent_framework::model::System& system,
                                       const ipmi::command::sdv::rsd::response::ReadPcieCableEepromData& read_pcie_cable_eeprom_data_response) {
    const auto& cable_memory_data = read_pcie_cable_eeprom_data_response.get_cable_memory_data();
    std::string cable_id{};
    auto data_iterator = cable_memory_data.begin();

    while (data_iterator != cable_memory_data.end() && *data_iterator != ' ') {
        cable_id.push_back(*data_iterator);
        data_iterator++;
    }

    system.add_cable_id(cable_id);
}
