/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/compute_commands.hpp"

using namespace agent_framework::command_ref;
using namespace agent_framework::module;

REGISTER_COMMAND(GetManagersCollection,
                 [](const GetManagersCollection::Request&, GetManagersCollection::Response& rsp) {
        log_debug(GET_LOGGER("rpc"), "Getting collection of managers.");
        auto keys = CommonComponents::get_instance()->
            get_module_manager().get_keys();
        for (auto key : keys) {
            auto presence = CommonComponents::get_instance()->
            get_module_manager().get_entry(key).get_presence();
            if (presence) {
                rsp.add_entry(agent_framework::model::attribute::ManagerEntry{key});
            }
        }
    }
);
