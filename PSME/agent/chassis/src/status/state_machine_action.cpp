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

#include "logger/logger_factory.hpp"
#include "status/state_machine_action.hpp"
#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"
#include "agent-framework/eventing/events_queue.hpp"

using namespace agent::chassis;
using namespace agent_framework::state_machine;
using namespace agent_framework::module;

using ChassisComponents = agent_framework::module::ChassisManager;

namespace {
    bool is_enabled(const agent_framework::state_machine::enums::State state) {
        return agent_framework::state_machine::enums::State::ENABLED == state;
    }
}

void StateMachineAction::execute(const std::string& module,
                                 const agent_framework::state_machine::enums::State state,
                                 const agent_framework::state_machine::enums::Transition) {
    const auto is_up = ::is_enabled(state);

    ChassisComponents::get_instance()->
        get_module_manager().get_entry_reference(module)->set_presence(is_up);
}
