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
*/

#include "status/state_machine_action.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "discovery/discovery_manager.hpp"

using namespace agent::compute;
using namespace agent_framework::state_machine;

StateMachineAction::~StateMachineAction() { }

namespace {
    bool is_enabled(const agent_framework::state_machine::enums::State state) {
        return agent_framework::state_machine::enums::State::ENABLED == state;
    }
}

void StateMachineAction::execute(const std::string& module,
                       const agent_framework::state_machine::enums::State state,
                       const agent_framework::state_machine::enums::Transition) {
    if (is_enabled(state)) {
        auto module_ptr = ModuleManager::get_module(module);
        if (module_ptr) {
            discovery::DiscoveryManager discovery{};
            discovery.discover(*module_ptr);
        }
    }
}


