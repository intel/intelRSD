/*!
 * @brief Grantley memory builder class implementation.
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
 * @file grantley_memory_builder.hpp
 */

#include "discovery/builders/platform_specific/grantley/grantley_memory_builder.hpp"

#include <bitset>
#include <algorithm>



using namespace agent::compute::discovery;
using namespace smbios::parser;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {

constexpr const std::uint8_t MEMORY_BANK_SIZE = 8;

}


void GrantleyMemoryBuilder::update_memory_index(agent_framework::model::Memory& memory, std::uint8_t dimm_index) {
    std::string socket_name{};

    if (dimm_index < MEMORY_BANK_SIZE) {
        socket_name = "A" + std::to_string(unsigned(dimm_index));
        log_debug(GET_LOGGER("agent"), "Dimm index: " << unsigned(dimm_index - 1)
                                                      << " is translated to: " << socket_name);
    }
    else {
        socket_name = "B" + std::to_string(dimm_index % MEMORY_BANK_SIZE);
        log_debug(GET_LOGGER("agent"), "Dimm index: " << unsigned(dimm_index - 1)
                                                      << " is translated to: " << socket_name);
    }

    memory.set_device_locator(socket_name);
}


void GrantleyMemoryBuilder::update_dimm_info(agent_framework::model::Memory& memory,
                                             const ipmi::command::sdv::response::GetDimmInfo& get_dimm_info_response) {
    attribute::Status status{};

    if (!get_dimm_info_response.is_present()) {
        const auto capacity_mb = get_dimm_info_response.get_dimm_size_mbytes();
        const auto& dimm_type = get_dimm_info_response.get_dimm_type();

        memory.set_capacity_mb(capacity_mb);
        memory.set_operating_speed_mhz(get_dimm_info_response.get_dimm_speed_mhz());
        memory.set_voltage_volt(get_dimm_info_response.get_dimm_voltage());

        try {
            memory.set_device_type(enums::DeviceType::from_string(dimm_type));
        }
        catch (const agent_framework::exceptions::InvalidValue&) {
            log_warning(GET_LOGGER("legacy-discovery"), "Unknown DIMM type value received: " << dimm_type);
        }

        attribute::Region region{};
        region.set_region_id("0");
        region.set_offset_mb(0);
        region.set_size_mb(0);

        memory.add_region(std::move(region));

        status.set_state(enums::State::Enabled);
    }
    else {
        status.set_state(enums::State::Absent);
    }

    memory.set_status(std::move(status));
}
