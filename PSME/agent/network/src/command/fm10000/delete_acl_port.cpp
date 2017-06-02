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

#include <vector>
#include <net/if.h>



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;
using namespace std;

namespace {

void unbind_all_ports(const string& acl_name) {
    api::Acl acl;
    try {
        acl.unbind_acl_from_all_ports(acl_name);
    }
    catch (const std::exception& e) {
        THROW(agent_framework::exceptions::Fm10000Error, "network-agent",
              "Failed unbinding ACL " + acl_name + " from all ports: " + e.what());
    }
}


void bind_ports(const string& acl_name,
                const vector <string>& port_identifiers) {
    api::Acl acl;
    try {
        acl.bind_acl_to_ports(acl_name, port_identifiers);
    }
    catch (const std::exception& e) {
        THROW(agent_framework::exceptions::Fm10000Error, "network-agent",
              "Failed binding ACL " + acl_name + " to ports: " + e.what());
    }
}


void check_if_rules_exist(const string& acl_uuid) {
    auto network_manager = NetworkComponents::get_instance();
    auto& rule_manager = network_manager->get_acl_rule_manager();

    if (0 == rule_manager.get_keys(acl_uuid).size()) {
        THROW(agent_framework::exceptions::InvalidValue, "fm10000",
              "Cannot unbind from ports when ACL has no rules. "
                  "You must create rules first.");
    }
}


void delete_acl_port(const DeleteAclPort::Request& request, DeleteAclPort::Response&) {
    auto network_manager = NetworkComponents::get_instance();
    auto& acl_manager = network_manager->get_acl_manager();
    auto& port_manager = network_manager->get_port_manager();
    auto& acl_port_manager = network_manager->get_acl_port_manager();
    vector <string> port_identifiers{};
    vector <string> port_to_remove_uuids{};
    const auto& acl_uuid = request.get_acl();

    // get ACL name
    const auto acl_name = acl_manager.get_entry(acl_uuid).get_name();

    // check if all ports to remove are bound to ACL
    for (const auto& port_uuid : request.get_ports()) {
        if (!acl_port_manager.entry_exists(acl_uuid, port_uuid)) {
            log_error(GET_LOGGER("network-agent"), "Port " << port_uuid << " is not bound to ACL " << acl_uuid);
            throw agent_framework::exceptions::InvalidValue("Requested port is not bound to ACL.");
        }
        port_to_remove_uuids.push_back(port_uuid);
    }

    // check if there is at least one rule
    // note: without rules bind_ports would fail on the switch.
    check_if_rules_exist(acl_uuid);

    auto acl_model = acl_manager.get_entry(acl_uuid);
    for (const auto& port_uuid : acl_port_manager.get_children(acl_model.get_uuid())) {
        const auto& it = std::find(port_to_remove_uuids.begin(), port_to_remove_uuids.end(), port_uuid);
        if (it == port_to_remove_uuids.end()) {
            const auto port = port_manager.get_entry(port_uuid);
            port_identifiers.push_back(port.get_port_identifier());
        }
    }

    try {
        unbind_all_ports(acl_name.value());
        bind_ports(acl_name.value(), port_identifiers);
    }
    catch (const std::experimental::bad_optional_access&) {
        // This code should never execute
        log_error(GET_LOGGER("fm10000"),
                  "Failed to delete ACL port: empty ACL name (memory corruption possible)");
    }

    // update the model
    for (const auto& port_uuid : request.get_ports()) {
        acl_port_manager.remove_entry(acl_uuid, port_uuid);
    }
}
}

REGISTER_COMMAND(DeleteAclPort, ::delete_acl_port
);
