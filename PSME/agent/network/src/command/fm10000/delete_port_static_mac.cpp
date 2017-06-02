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
#include "api/static_mac.hpp"

#include <stdexcept>

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework;
using namespace agent::network;
using namespace std;

namespace {
    inline void remove_static_mac(const string& port, const string& address,
                                  uint16_t vlan_id) {
        try {
            api::StaticMac::remove(port, address, vlan_id);
        }
        catch (const runtime_error &e) {
            THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
                  string("Failed to remove Static MAC on port: ") + e.what());
        }
    }

    void delete_port_static_mac(const DeletePortStaticMac::Request& request, DeletePortStaticMac::Response&) {
        auto network_manager = NetworkComponents::get_instance();
        const auto& port_manager = network_manager->get_port_manager();
        auto& static_mac_manager = network_manager->get_static_mac_manager();

        // check if given Static MAC UUID exists
        const auto& static_mac_uuid = request.get_static_mac();
        StaticMac static_mac = static_mac_manager.get_entry(static_mac_uuid);

        auto port = port_manager.get_entry(static_mac.get_parent_uuid());
        string mac_address = static_mac.get_address();
        uint16_t vlan_id = uint16_t(static_mac.get_vlan_id());

        ::remove_static_mac(port.get_port_identifier(), mac_address, vlan_id);

        static_mac_manager.remove_entry(static_mac_uuid);
    }
}

REGISTER_COMMAND(DeletePortStaticMac, ::delete_port_static_mac);
