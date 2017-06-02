/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file memory.cpp
 *
 * @brief ipmi-way of discovering memory properties
 * */

#include "discovery/common.hpp"
#include "discovery/basic/memory.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "agent-framework/module/model/memory.hpp"
#include "agent-framework/module/compute_components.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace ipmi::command;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

namespace {

constexpr const std::uint8_t MEMORY_BANK_SIZE = 8;
constexpr const std::uint8_t HASSWEL_MEMORY_NO = 16;

string get_socket_name(uint8_t dimm_index) {
    if (dimm_index < MEMORY_BANK_SIZE) {
        std::string socket_name{"A" + std::to_string(unsigned(dimm_index))};
        log_debug(GET_LOGGER("agent"), "Dimm index: " << unsigned(dimm_index)
                                       << " is translated to: " << socket_name);
        return socket_name;
    }
    else {
        std::string socket_name{"B" + std::to_string(dimm_index % MEMORY_BANK_SIZE)};
        log_debug(GET_LOGGER("agent"), "Dimm index: " << unsigned(dimm_index)
                                       << " is translated to: " << socket_name);
        return socket_name;
    }
}

}

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

std::tuple<bool, agent_framework::model::enums::Health> discover_memory_modules(ipmi::ManagementController& mc,
                                                                                System& system) {
    log_debug(GET_LOGGER("basic-discovery"), "Send GetDimmInfo.");

    sdv::request::GetDimmInfo request{};
    sdv::response::GetDimmInfo response{};

    enums::Health composite_health = enums::Health::OK;

    // Memory modules in HSW are indexed from 1.
    for (std::uint8_t index = 1; index <= HASSWEL_MEMORY_NO; index++) {
        log_debug(GET_LOGGER("basic-discovery"), "Sending GetDimmInfo. Index: " << std::uint32_t(index));
        request.set_dimm_index(index);

        try {
            mc.send(request, response);
        }
        catch (const std::runtime_error& e) {
            composite_health = enums::Health::Warning;
            log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetDimmInfo: "
                << e.what() << " for System: " << system.get_uuid());
            continue;
        }

        const auto cc = response.get_completion_code();
        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            composite_health = enums::Health::Warning;
            log_bad_completion_code(system.get_uuid(), cc, "GetDimmInfo");
            continue;
        }

        Memory memory{system.get_uuid()};
        attribute::Status status{};

        /*
         * Deep Discovery numbering starts from 0. IPMI from 1.
         * Need to translate IPMI index into LSHW (used in Deep Discovery)
         * socket name (literal + numbering from 0);
         */
        memory.set_device_locator(get_socket_name(uint8_t(index - 1)));

        if (response.is_present()) {
            const auto capacity_mb = response.get_dimm_size_mbytes();
            memory.set_capacity_mb(capacity_mb);
            memory.set_operating_speed_mhz(response.get_dimm_speed_mhz());
            memory.set_voltage_volt(response.get_dimm_voltage());

            try {
                const auto& dimm_type = response.get_dimm_type();
                memory.set_device_type(enums::DeviceType::from_string(dimm_type));
            }
            catch (const agent_framework::exceptions::InvalidValue&) {
                memory.set_device_type(enums::DeviceType::Unknown);
            }

            attribute::Region region{};
            region.set_region_id("0");
            region.set_offset_mb(0);
            region.set_size_mb(capacity_mb);
            memory.add_region(std::move(region));

            status.set_state(enums::State::Enabled);
        }
        else {
            log_warning(GET_LOGGER("basic-discovery"), "Memory (" << memory.get_device_locator() << ") is not present.");
            status.set_state(enums::State::Absent);
            status.set_health({}); // Set to null
        }

        memory.set_status(status);
        ComputeComponents::get_instance()->get_memory_manager().add_entry(std::move(memory));
        log_debug(GET_LOGGER("basic-discovery"), "GetDimmInfo successful.");
    }

    log_info(GET_LOGGER("basic-discovery"), "Memory Modules Discovery was successful.");
    return std::make_tuple(enums::Health::OK == composite_health ? true : false, composite_health);
}


}
}
}
}
