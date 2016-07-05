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
#include "discovery/discovery_manager.hpp"
#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/eventing/events_queue.hpp"

using namespace agent::compute;
using namespace agent_framework::state_machine;
using namespace agent_framework::module;

using ComputeComponents = agent_framework::module::ComputeManager;
using State = agent_framework::state_machine::enums::State;
using Transition = agent_framework::state_machine::enums::Transition;

namespace {
    bool is_enabled(State state) {
        return State::ENABLED == state;
    }

    ::agent_framework::eventing::Notification select_notification(const bool flag) {
        return flag ? ::agent_framework::eventing::Notification::Add :
                      ::agent_framework::eventing::Notification::Remove;
    }

    void generate_event(const std::string& module, const bool is_up) {
        ::agent_framework::eventing::EventData edat;
        edat.set_component(module);
        edat.set_type(::agent_framework::model::enums::Component::Manager);
        edat.set_notification(select_notification(is_up));
        ::agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    }
}

using agent_framework::discovery::Discovery;
using DiscoveryUnique = std::unique_ptr<Discovery>;

void StateMachineAction::execute(const std::string& module,
                                 State state, Transition) {

    const auto is_up = ::is_enabled(state);
    ComputeComponents::get_instance()->
            get_module_manager().get_entry_reference(module)->set_presence(is_up);

    if (is_up) {
        try {
            discovery::DiscoveryManager().discovery(module);
        } catch(const std::runtime_error& ec) {
            log_error(GET_LOGGER("agent"), ec.what());
        }
    }
    generate_event(module, is_up);
}
