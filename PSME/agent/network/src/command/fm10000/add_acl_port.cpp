/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "api/acl.hpp"
#include "hw/fm10000/network_controller_manager.hpp"

#include <vector>
#include <net/if.h>

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;
using namespace agent::network::hw::fm10000;
using namespace std;

namespace {

    void bind_ports(const string& acl_name,
                    const vector<string>& port_identifiers) {
        api::Acl acl;
        try {
            acl.bind_acl_to_ports(acl_name, port_identifiers);
        }
        catch(const std::runtime_error& e) {
            THROW(agent_framework::exceptions::Fm10000Error, "network-agent",
                  "Failed binding ACL " + acl_name + " to ports: " +
                  e.what());
        }
    }

    void validate_acl(const string& acl_uuid) {
        auto network_manager = NetworkManager::get_instance();
        auto& acl_manager = network_manager->get_acl_manager();
        auto& rule_manager = network_manager->get_acl_rule_manager();

        // check if given ACL UUID exists
        acl_manager.get_entry(acl_uuid);

        if (0 == rule_manager.get_keys(acl_uuid).size()) {
            THROW(agent_framework::exceptions::UnsupportedParameter,
                  "fm10000",
                  "Cannot bind to ports when ACL has no rules. "
                  "You must create rules first.");
        }
    }

    void add_acl_port(const AddAclPort::Request& request, AddAclPort::Response&) {
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto& acl_port_manager = network_manager->get_acl_port_manager();
        auto controller = NetworkControllerManager::get_network_controller();
        vector<string> port_identifiers{};
        const auto& acl_uuid = request.get_acl();

        validate_acl(acl_uuid);

        for (const auto& port_uuid : request.get_ports()) {
            const auto port = port_manager.get_entry(port_uuid);
            if (acl_port_manager.entry_exists(acl_uuid, port_uuid)) {
                THROW(agent_framework::exceptions::InvalidParameters,
                      "network-agent", "Port " + port_uuid +
                      " is already bound to ACL " + acl_uuid);
            }
            if (enums::PortClass::Logical == port.get_port_class()) {
                THROW(agent_framework::exceptions::InvalidParameters,
                      "network-agent", "Port " + port_uuid +
                      " is a LAG. ACL is not supported on LAG.");
            }
            port_identifiers.push_back(port.get_port_identifier());
        }

        bind_ports(controller->get_acl_name(acl_uuid), port_identifiers);

        // update the model
        for (const auto& port_uuid : request.get_ports()) {
            acl_port_manager.add_entry(acl_uuid, port_uuid, "");
        }
    }
}

REGISTER_COMMAND(AddAclPort, ::add_acl_port);
