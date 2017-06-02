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

#include "agent-framework/state_machine/state_machine_module_thread.hpp"
#include "agent-framework/state_machine/state_machine_exception.hpp"

#include <chrono>
#include <algorithm>

using namespace agent_framework;
using namespace agent_framework::state_machine;

namespace {
/* @TODO: This variable has to be moved to configuration class. */
/*! State Machine`s iterating interval. */
constexpr const std::size_t STATE_MACHINE_MODULE_INTERVAL_SECONDS = 10;
}

StateMachineModuleThread::StateMachineModuleThread(StateThreadEntrySharedPtr& entry, StateMachineThreadAction& action) :
                       m_entry(entry), m_action(action) {}

StateMachineModuleThread::~StateMachineModuleThread() {
    m_is_running = false;
    m_condition.notify_one();
    m_thread.join();
}

void StateMachineModuleThread::set_entry(const StateThreadEntrySharedPtr& entry) {
    std::lock_guard<std::mutex> lock{m_entry_mutex};
    m_entry = entry;
}

StateThreadEntrySharedPtr StateMachineModuleThread::get_entry() {
    std::lock_guard<std::mutex> lock{m_entry_mutex};
    return m_entry;
}


void StateMachineModuleThread::task() {
    log_info(GET_LOGGER("state-machine"), "Starting State Machine Module thread...");

    while(m_is_running) {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (m_condition.wait_for(lk, std::chrono::seconds(STATE_MACHINE_MODULE_INTERVAL_SECONDS)) == std::cv_status::timeout) {
            std::lock_guard<std::mutex> lock{m_entry_mutex};
                try {
                    m_entry->next_state();
                    if (m_entry->is_state_changed()) {
                        log_info(GET_LOGGER("agent"), "Module " << m_entry->get_module()
                                                   << " status changed to " << m_entry->get_state().to_string()
                                                   << " after event " << m_entry->get_transition().to_string());
                        m_action.execute(m_entry);
                    }
                }
                catch (const state_machine::StateMachineError& e) {
                    log_error(GET_LOGGER("agent"),
                              "Cannot set next state.");
                    log_debug(GET_LOGGER("agent"), e.what());
                }
        }
    }
    log_debug(GET_LOGGER("state-machine"), "State Machine Module thread stopped.");
}

void StateMachineModuleThread::start() {
    m_is_running = true;
    m_thread = std::thread(&StateMachineModuleThread::task, this);
}

bool StateMachineModuleThread::is_running() {
    return m_is_running;
}
