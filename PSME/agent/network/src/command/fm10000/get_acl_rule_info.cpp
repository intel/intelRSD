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

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::network;

namespace {
    void get_acl_rule_info(const GetAclRuleInfo::Request& request,
                           GetAclRuleInfo::Response& response) {
        auto network_manager = NetworkComponents::get_instance();
        auto& acl_rule_manager = network_manager->get_acl_rule_manager();

        response = acl_rule_manager.get_entry(request.get_uuid());
    }
}

REGISTER_COMMAND(GetAclRuleInfo, ::get_acl_rule_info);
