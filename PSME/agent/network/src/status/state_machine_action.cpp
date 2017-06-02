/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
*/

#include "status/state_machine_action.hpp"
#include "discovery/discovery_manager.hpp"
#include "tree_stability/network_tree_stabilizer.hpp"
#include "agent-framework/state_machine/state_machine_exception.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"



using namespace agent::network;
using namespace agent::network::discovery;
using namespace agent_framework::state_machine;

using agent_framework::model::enums::Component;
using agent_framework::eventing::Notification;


StateMachineAction::StateMachineAction(DiscoveryManager& discover_manger) :
    m_discovery_manager{discover_manger} { }


StateMachineAction::~StateMachineAction() { }


namespace {
bool is_enabled(const enums::State state) {
    return enums::State::ENABLED == state;
}


void send_event(const std::string& module, const std::string& parent, Component type, Notification notification) {
    agent_framework::eventing::EventData edat;
    edat.set_parent(parent);
    edat.set_component(module);
    edat.set_type(type);
    edat.set_notification(notification);
    agent_framework::eventing::EventsQueue::get_instance()->push_back(edat);
    log_info(GET_LOGGER("agent"), "Sending an event for component " << edat.get_component() << "," << "("
                                  << edat.get_type() << ")" << ", notification type: " << edat.get_notification());
}

}


void StateMachineAction::execute(StateThreadEntrySharedPtr entry) {
    const auto state = entry->get_state();
    auto module = entry->get_module();
    if (is_enabled(state)) {
        try {
            /* Start discovery manager */
            m_discovery_manager.discovery(module);
            module = NetworkTreeStabilizer().stabilize(module);
            entry->update_module(module);
            entry->set_discovered(true);
            send_event(module, {}, Component::Manager, Notification::Add);
        }
        catch (const std::exception& e) {
            throw StateMachineError(e.what());
        }
    }
}
