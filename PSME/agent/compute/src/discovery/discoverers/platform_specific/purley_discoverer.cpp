/*!
 * @brief Compute agent Purley platform discoverer.
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
 * @file purley_discoverer.cpp
 */

#include "discovery/discoverers/platform_specific/purley_discoverer.hpp"
#include "discovery/builders/platform_specific/purley/purley_chassis_builder.hpp"
#include "discovery/builders/platform_specific/purley/purley_system_builder.hpp"
#include "ipmi/command/sdv/get_slot_id.hpp"
#include "ipmi/command/sdv/rsd/get_system_mode.hpp"
#include "ipmi/command/sdv/rsd/set_system_mode.hpp"
#include "ipmi/command/sdv/rsd/read_pcie_cable_eeprom_data.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;
using namespace ipmi::command;


PurleyDiscoverer::~PurleyDiscoverer() {}


std::uint32_t PurleyDiscoverer::get_platform_id() const {
    return ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_PURLEY;
}


bool PurleyDiscoverer::discover_user_mode(agent_framework::model::System& system) {
    bool generic_discovery_successful = GenericDiscoverer::discover_user_mode(system);

    if (!generic_discovery_successful) {
        sdv::rsd::request::GetSystemMode get_system_mode_request{};
        sdv::rsd::response::GetSystemMode get_system_mode_response{};

        try {
            log_debug("purley-discoverer", "Sending " << get_system_mode_request.get_command_name());
            get_management_controller().send(get_system_mode_request, get_system_mode_response);
            PurleySystemBuilder::update_system_mode(system, get_system_mode_response);
        }
        catch (const ipmi::ResponseError& response_error) {
            log_ipmi_error(response_error);
            return false;
        }
        catch (const std::runtime_error& e) {
            log_discovery_error(e, system.get_component().to_string());
            return false;
        }
    }

    return true;
}


bool PurleyDiscoverer::set_rackscale_mode(agent_framework::model::System& system) {
    if (system.is_rackscale_mode_enabled()) {
        return true;
    }
    sdv::rsd::request::SetSystemMode set_system_mode_request{};
    sdv::rsd::response::SetSystemMode set_system_mode_response{};

    ipmi::command::sdv::SystemMode system_mode = system.is_user_mode_enabled() ?
                                                 sdv::SystemMode::USER_MODE : sdv::SystemMode::ADMIN_MODE;
    set_system_mode_request.set_system_mode(system_mode);
    set_system_mode_request.set_rackscale_mode(true);
    try {
        log_debug("purley-discoverer", "Sending " << set_system_mode_request.get_command_name());
        get_management_controller().send(set_system_mode_request, set_system_mode_response);
    }
    catch (const ipmi::ResponseError& response_error) {
        log_ipmi_error(response_error);
        return false;
    }
    catch (const std::runtime_error& e) {
        log_discovery_error(e, system.get_component().to_string());
        return false;
    }
    return true;
}


bool PurleyDiscoverer::discover_cable_id(System& system) {
    bool generic_discovery_successful = GenericDiscoverer::discover_cable_id(system);

    if (!generic_discovery_successful) {
        log_debug("purley-discoverer", "Generic discovery of cable ID failed");
        constexpr unsigned MAX_CHANNEL_NUMBER = 4;
        constexpr unsigned CABLE_ID_OFFSET = 0xC4;
        constexpr unsigned CABLE_ID_LENGTH = 16;
        for (std::uint8_t channel = 0; channel < MAX_CHANNEL_NUMBER; channel++) {
            sdv::rsd::request::ReadPcieCableEepromData get_cable_data_request{};
            sdv::rsd::response::ReadPcieCableEepromData get_cable_data_response{};

            get_cable_data_request.set_pcie_slot_id(0);
            get_cable_data_request.set_cable_port_index(channel);
            get_cable_data_request.set_upper_memory_page_select(0);
            get_cable_data_request.set_reading_data_offset(CABLE_ID_OFFSET);
            get_cable_data_request.set_reading_data_length(CABLE_ID_LENGTH);

            try {
                log_debug("purley-discoverer", "Sending " << get_cable_data_request.get_command_name());
                get_management_controller().send(get_cable_data_request, get_cable_data_response);
                PurleySystemBuilder::add_cable_id(system, get_cable_data_response);
            }
            catch (const ipmi::ResponseError& response_error) {
                log_ipmi_error(response_error);
            }
            catch (const std::runtime_error& e) {
                log_discovery_error(e, system.get_component().to_string());
            }
        }
    }

    return true;
}


bool PurleyDiscoverer::discover_chassis(Chassis& chassis) {
    GenericDiscoverer::discover_chassis(chassis);

    sdv::request::GetSlotId get_slot_id_request{};
    sdv::response::GetSlotId get_slot_id_response{};

    try {
        log_debug("purley-discoverer", "Sending " << get_slot_id_request.get_command_name());
        get_management_controller().send(get_slot_id_request, get_slot_id_response);

        PurleyChassisBuilder::update_slot_id(chassis, get_slot_id_response);
    }
    catch (const ipmi::ResponseError& response_error) {
        log_ipmi_error(response_error);
        return false;
    }
    catch (const std::runtime_error& e) {
        log_discovery_error(e, chassis.get_component().to_string());
        return false;
    }

    return true;
}
