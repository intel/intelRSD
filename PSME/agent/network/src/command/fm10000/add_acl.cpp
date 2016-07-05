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

#include <vector>
#include <net/if.h>

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;
using namespace std;

namespace {
    inline void create_acl(const string& acl_name) {
        try {
            api::Acl::create_acl(acl_name);
        }
        catch (const runtime_error &e) {
            THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
                  string("Failed to create ACL on switch: ") + e.what());
        }
    }

    void add_acl(const AddAcl::Request& request, AddAcl::Response& response) {
        auto network_manager = NetworkManager::get_instance();
        auto& switch_manager = network_manager->get_switch_manager();
        auto& acl_manager = network_manager->get_acl_manager();

        // check if given switch UUID exists
        switch_manager.get_entry(request.get_switch());

        // we can't bind to ports for newly created ACL without rules
        if (0 < request.get_ports().get_array().size()) {
            THROW(agent_framework::exceptions::UnsupportedParameter, "fm10000",
                  "Cannot bind to ports when creating ACL. "
                  "You must create rules first.");
        }

        model::Acl acl(request.get_switch());

        ::create_acl(acl.get_uuid());

        acl.set_status({enums::State::Enabled, enums::Health::OK});
        acl.add_collection({enums::CollectionName::AclRules,
                           enums::CollectionType::Rules, ""});
        acl.add_collection({enums::CollectionName::AclPorts,
                           enums::CollectionType::Ports, ""});
        response.set_acl(acl.get_uuid());
        acl_manager.add_entry(std::move(acl));
    }
}

REGISTER_COMMAND(AddAcl, ::add_acl);
