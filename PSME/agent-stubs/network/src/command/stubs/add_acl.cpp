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
    void add_acl(const AddAcl::Request& request,
                      AddAcl::Response& response) {
        using NetworkManager = agent_framework::module::NetworkManager;
        Acl acl {request.get_switch()};
        log_debug(GET_LOGGER("network-agent"), "Adding ACL: " << acl.get_uuid());
        acl.set_status(attribute::Status(
                        enums::State::Enabled,
                        enums::Health::OK));
        acl.add_collection(attribute::Collection(
                 enums::CollectionName::AclRules,
                 enums::CollectionType::Rules,
                 "slotMask"));
        acl.add_collection(attribute::Collection(
                 enums::CollectionName::Ports,
                 enums::CollectionType::Ports,
                 "slotMask"));
        for (const auto& port : request.get_ports()) {
            NetworkManager::get_instance()->get_acl_port_manager().add_entry(acl.get_uuid(), port);
        }
        response.set_acl(acl.get_uuid());
        NetworkManager::get_instance()->get_acl_manager().add_entry(std::move(acl));
    }
}

REGISTER_COMMAND(AddAcl, ::add_acl);
