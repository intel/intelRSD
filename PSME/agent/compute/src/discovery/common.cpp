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
 * */

#include "logger/logger_factory.hpp"
#include "discovery/common.hpp"
#include "agent-framework/module/common_components.hpp"
#include "netlink/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;
using ChassisRef = ObjReference<agent_framework::model::Chassis, std::recursive_mutex>;

namespace agent {
namespace compute {
namespace discovery {

using COMPLETION_CODE = ::ipmi::Response::COMPLETION_CODE;

void log_bad_completion_code(const std::string& uuid, COMPLETION_CODE cc, const std::string& command) {
    log_error(GET_LOGGER("compute-discovery"), command << " - Bad completion code for: " <<
                                   uuid << " IPMI completion code: " << uint32_t(cc));
}

void log_bad_completion_code(COMPLETION_CODE cc, const std::string& command) {
    log_error(GET_LOGGER("compute-discovery"), command << " - Bad IPMI completion code: " << uint32_t(cc));
}

std::vector<ChassisRef> get_chassis_refs(const std::string& parent_manager_uuid) {
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();

    std::vector<ChassisRef> chassis_ref_vec{};
    auto chassis_uuids_vec = chassis_manager.get_keys(parent_manager_uuid);

    for (const auto& chassis_uuid : chassis_uuids_vec) {
        auto chassis = chassis_manager.get_entry_reference(chassis_uuid);
        chassis_ref_vec.push_back(std::move(chassis));
    }
    return chassis_ref_vec;
}

bool discover_rrc_network_interface(System& system) {
    NetworkInterface interface{system.get_uuid()};
    attribute::Status status;
    bool ret{true};

    try {
        const Manager& manager = CommonComponents::get_instance()->
            get_module_manager().get_entry(system.get_parent_uuid());
        auto mac_address = netlink_base::utils::
            get_neighbor_mac_address(manager.get_switch_port_identifier());
        status.set_state(enums::State::Enabled);
        interface.set_factory_mac_address(mac_address);
        interface.set_mac_address(mac_address);
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("compute-discovery"),
                  "Cannot read MAC address from Netlink for system "
                  << system.get_uuid() << " : " << e.what());
        status.set_state(enums::State::Disabled);
        ret = false;
    }

    interface.set_status(status);
    ComputeComponents::get_instance()->
            get_network_interface_manager().add_entry(std::move(interface));
    return ret;
}

}
}
}
