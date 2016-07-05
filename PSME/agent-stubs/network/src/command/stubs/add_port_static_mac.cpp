/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

using namespace agent_framework::model;
using namespace agent_framework::command_ref;

namespace {
    void add_port_static_mac(const AddPortStaticMac::Request& request,
                      AddPortStaticMac::Response& response) {
        using NetworkManager = agent_framework::module::NetworkManager;
        auto static_mac_json = request.to_json();
        static_mac_json[literals::Status::STATUS] = attribute::Status(
                enums::State::Enabled,
                enums::Health::OK).to_json();
        StaticMac static_mac = StaticMac::from_json(static_mac_json);
        log_debug(GET_LOGGER("network-agent"), "Adding Static MAC: " << static_mac.get_uuid());
        static_mac.set_parent_uuid(request.get_port());
        response.set_static_mac(static_mac.get_uuid());
        NetworkManager::get_instance()->get_static_mac_manager().add_entry(std::move(static_mac));
    }
}

REGISTER_COMMAND(AddPortStaticMac, ::add_port_static_mac);
