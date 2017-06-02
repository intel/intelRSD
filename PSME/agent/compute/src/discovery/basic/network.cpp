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
 * @file dimm.cpp
 *
 * @brief ipmi-way of discovering memory properties
 * */

#include "discovery/common.hpp"
#include "discovery/basic/network.hpp"
#include "ipmi/management_controller.hpp"
#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"
#include "agent-framework/module/model/network_interface.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace ipmi::command;
using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

std::tuple<bool, agent_framework::model::enums::Health> discover_network_interfaces(ipmi::ManagementController& mc,
                                                                                    System& system) {
    log_debug(GET_LOGGER("basic-discovery"), "Send GetSystemMacAddressHaswell.");
    sdv::request::GetSystemMacAddressHaswell request{};
    sdv::response::GetSystemMacAddressHaswell response{};
    enums::Health composite_health = enums::Health::OK;

    /*
     * This is workaround for BDC-RRC switch issue.
     * BDC-RRC has two network interfaces, but in legacy mode it is possible
     * to retrieve information only about one.
     * This code retrieve information only about first one to avoid setting
     * System::State::Health to Warning because this would prevent assemble
     * from working correctly.
     */
    const std::uint8_t nics_ids[] = {1};

    for (const std::uint8_t nic_index : nics_ids) {
        request.set_interface_id(nic_index);

        try {
            log_debug(GET_LOGGER("basic-discovery"), "Sending GetSystemMacAddress. NIC index: " << std::uint32_t(nic_index));
            mc.send(request, response);
        }
        catch (const std::runtime_error& e) {
            composite_health = enums::Health::Warning;

            log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetSystemMacAddress: "
                                           << e.what()
                                           << " for System: "
                                           << system.get_uuid());
            continue;
        }

        const auto cc = response.get_completion_code();

        if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
            composite_health = enums::Health::Warning;
            log_bad_completion_code(system.get_uuid(), cc, "GetSystemMacAddress");
            continue;
        }

        NetworkInterface interface{system.get_uuid()};
        interface.set_mac_address(response.get_system_mac_address());
        interface.set_factory_mac_address(response.get_system_mac_address());

        attribute::Status status;
        status.set_state(enums::State::Enabled);
        interface.set_status(status);

        ComputeComponents::get_instance()->get_network_interface_manager().add_entry(std::move(interface));
        log_debug(GET_LOGGER("basic-discovery"), "GetSystemMacAddress successful.");
    }

    /* Retrieve information about factory MAC address on RRC interface using Netlink API */
    auto rrc_status = discover_rrc_network_interface(system);
    bool status = rrc_status && (enums::Health::OK == composite_health);

    log_info(GET_LOGGER("basic-discovery"), "Network information Discovery was successful.");
    return std::make_tuple(status, composite_health);
}

}
}
}
}
