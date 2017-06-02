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



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;

namespace {
void delete_acl_rule(const DeleteAclRule::Request& request, DeleteAclRule::Response&) {
    auto network_manager = NetworkComponents::get_instance();
    auto& rule_manager = network_manager->get_acl_rule_manager();

    // check if given ACL rule UUID exists
    const string& rule_uuid = request.get_rule();
    const auto rule = rule_manager.get_entry(rule_uuid);
    if (!rule.get_rule_id().has_value()) {
        log_error(GET_LOGGER("fm10000"),
                  "Failed to delete ACL because rule ID is unknown for ACL rule: " << rule_uuid);
        throw agent_framework::exceptions::InvalidValue("Failed to delete ACL rule because rule ID is unknown.");
    }

    const auto& acl_uuid = rule.get_parent_uuid();
    const auto acl_name = network_manager->get_acl_manager().get_entry(acl_uuid).get_name();

    try {
        api::Acl::delete_acl_rule(acl_name.value(),
                                  uint16_t(rule.get_rule_id()));
    }
    catch (const std::experimental::bad_optional_access&) {
        // This code should never execute
        log_error(GET_LOGGER("fm10000"),
                  "Failed to delete ACL rule from switch: empty ACL name (memory corruption possible)");
    }
    catch (const std::exception& e) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              "Failed to delete ACL rule from switch: " + std::string(e.what()));
    }

    rule_manager.remove_entry(rule_uuid);
}
}

REGISTER_COMMAND(DeleteAclRule, ::delete_acl_rule
);
