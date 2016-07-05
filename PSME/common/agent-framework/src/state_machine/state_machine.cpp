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

#include "agent-framework/state_machine/state_machine.hpp"
#include "agent-framework/logger_ext.hpp"

#include "agent-framework/state_machine/module_state_unknown.hpp"
#include "agent-framework/state_machine/module_state_absent.hpp"
#include "agent-framework/state_machine/module_state_enabled.hpp"
#include "agent-framework/state_machine/module_state_offline.hpp"
#include "agent-framework/state_machine/module_state_starting.hpp"

#include "agent-framework/state_machine/state_machine_exception.hpp"

#include "agent-framework/status/module_software_status.hpp"
#include "agent-framework/status/module_hardware_status.hpp"

using namespace agent_framework;
using namespace agent_framework::state_machine;

StateMachine::StateMachine() :
    m_states{
        {enums::State::UNKNOWN, std::make_shared<ModuleStateUnknown>()},
        {enums::State::ABSENT, std::make_shared<ModuleStateAbsent>()},
        {enums::State::ENABLED, std::make_shared<ModuleStateEnabled>()},
        {enums::State::OFFLINE, std::make_shared<ModuleStateOffline>()},
        {enums::State::STARTING, std::make_shared<ModuleStateStarting>()}
    }{}


void StateMachine::start() {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_curr_state = get_state(enums::State::UNKNOWN);
    if (!m_curr_state) {
        throw StateMachineError{"Invalid start state"};
    }
}

void StateMachine::stop() {
    log_info(GET_LOGGER("agent"), "State Machine stopped.");
}

void StateMachine::set_next_state(const status::ModuleStatus::Status hw_status,
                        const status::ModuleStatus::Status sw_status) {
    std::lock_guard<std::mutex> lock{m_mutex};
    enums::State next_state = get_next_state(hw_status, sw_status);

    m_curr_state->enter_state(m_curr_transition, next_state);
    m_curr_state = m_states[next_state];
}

bool StateMachine::is_state_changed() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_curr_transition.get_transition() !=
            enums::Transition::IDLE;
}


enums::State
StateMachine::get_next_state(const status::ModuleStatus::Status hw_status,
                      const status::ModuleStatus::Status sw_status) {
    // Determine next state
    if (is_enabled_state(hw_status, sw_status)) {
        return enums::State::ENABLED;
    }

    if (is_starting_state(hw_status, sw_status)) {
        return enums::State::STARTING;
    }

    if (is_offline_state(hw_status, sw_status)) {
        return enums::State::OFFLINE;
    }

    return enums::State::ABSENT;
}

bool
StateMachine::is_enabled_state(const status::ModuleStatus::Status hw_status,
                      const status::ModuleStatus::Status sw_status) {
    return hw_status == status::ModuleStatus::Status::PRESENT &&
           sw_status == status::ModuleStatus::Status::PRESENT;
}

bool
StateMachine::is_offline_state(const status::ModuleStatus::Status hw_status,
                      const status::ModuleStatus::Status sw_status) {
    return hw_status == status::ModuleStatus::Status::PRESENT &&
           sw_status == status::ModuleStatus::Status::NOT_PRESENT;
}

bool
StateMachine::is_starting_state(const status::ModuleStatus::Status hw_status,
                      const status::ModuleStatus::Status sw_status) {
    return hw_status == status::ModuleStatus::Status::DETERMINING &&
           sw_status == status::ModuleStatus::Status::NOT_PRESENT;
}


ModuleStateSharedPtr StateMachine::get_state(const enums::State state) {
    auto it = m_states.find(state);
    if (it != m_states.cend()) {
        return it->second;
    }
    return nullptr;
}

enums::State StateMachine::get_state() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_curr_state->get_state();
}

enums::Transition StateMachine::get_transition() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_curr_transition.get_transition();
}
