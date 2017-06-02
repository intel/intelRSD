/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/acl.hpp"

#include <stdexcept>
#include <iostream>



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;

namespace {
void delete_acl(const DeleteAcl::Request& request, DeleteAcl::Response&) {
    auto network_manager = NetworkComponents::get_instance();
    auto& acl_manager = network_manager->get_acl_manager();
    auto& acl_rule_manager = network_manager->get_acl_rule_manager();
    auto& acl_port_manager = network_manager->get_acl_port_manager();

    // check if given ACL UUID exists and retrieve its name
    const auto& acl_uuid = request.get_acl();
    const auto acl_name = acl_manager.get_entry(request.get_acl()).get_name();

    try {
        api::Acl::delete_acl(acl_name.value());
    }
    catch (const std::experimental::bad_optional_access&) {
        // This code should never execute
        log_error(GET_LOGGER("fm10000"),
                  "Failed reading ACL rule id from switch: empty ACL name (memory corruption possible)");
    }
    catch (const std::exception& e) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              "Failed to delete ACL " + acl_uuid + " from switch: " +
              e.what());
    }

    acl_rule_manager.remove_by_parent(acl_uuid);
    acl_port_manager.remove_parent(acl_uuid);
    acl_manager.remove_entry(acl_uuid);
}
}

REGISTER_COMMAND(DeleteAcl, ::delete_acl
);
