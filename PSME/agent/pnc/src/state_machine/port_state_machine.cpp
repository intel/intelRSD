/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file state_machine/port_state_machine.cpp
 * @brief Implementation of the PortStateMachine
 * */

#include "state_machine/port_state_machine.hpp"

using namespace agent::pnc::state_machine;

PortStateMachine::PortStateMachine(const std::string& id, const PortState& init_state,
        const PortStateMachine::TransitionTable& transition_table): m_id(id),
    EnumStateMachine(init_state, transition_table) {
}

PortStateMachine::~PortStateMachine() {}

bool PortStateMachine::do_transition(const Transition& transition) {
    log_debug("port-state-machine", "Port state machine (" << m_id << "): Transition "
        << transition.init_state.to_string() << " --> " << transition.end_state.to_string()
        << " due to event: " << transition.event.to_string());
    bool result = EnumStateMachine::do_transition(transition);
    std::string message = result ? "OK" : "FAILED";
    log_debug("port-state-machine", "Port state machine (" << m_id
        << "): Transition status: " << message);
    return result;
}

void PortStateMachine::do_on_event_action(const PortEvent& event) {
    log_debug("port-state-machine", "Port state machine (" << m_id << "): In state "
        << get_current_state().to_string() << ": received event: " << event.to_string());
}
