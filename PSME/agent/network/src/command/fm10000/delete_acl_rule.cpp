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

#include <stdexcept>

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;
using namespace agent::network::hw::fm10000;

namespace {
    void delete_acl_rule(const DeleteAclRule::Request& request,
                         DeleteAclRule::Response&) {
        auto network_manager = NetworkManager::get_instance();
        auto& rule_manager = network_manager->get_acl_rule_manager();
        auto controller = NetworkControllerManager::get_network_controller();

        // check if given ACL rule UUID exists
        const string& rule_uuid = request.get_rule();
        const auto rule = rule_manager.get_entry(rule_uuid);
        if (!rule.get_rule_id().has_value()) {
            THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
                  "Failed to delete ACL rule " + rule_uuid +
                  ", unknown rule id.");
        }
        const auto& acl_uuid = rule.get_parent_uuid();

        try {
            api::Acl::delete_acl_rule(controller->get_acl_name(acl_uuid),
                                      uint16_t(rule.get_rule_id()));
        }
        catch (const std::runtime_error& e) {
            THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
                  "Failed to delete ACL rule " + rule_uuid +
                  " from switch: " + e.what());
        }

        rule_manager.remove_entry(rule_uuid);
    }
}

REGISTER_COMMAND(DeleteAclRule, ::delete_acl_rule);
