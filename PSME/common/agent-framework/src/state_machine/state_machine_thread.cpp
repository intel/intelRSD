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

#include "agent-framework/state_machine/state_machine_thread.hpp"
#include "agent-framework/state_machine/state_thread_entry.hpp"
#include "agent-framework/state_machine/state_machine.hpp"
#include "agent-framework/state_machine/state_machine_exception.hpp"
#include "logger/logger_factory.hpp"

#include <chrono>
#include <algorithm>

using namespace agent_framework;
using namespace agent_framework::state_machine;

namespace {
/* @TODO: This variable has to be moved to configuration class. */
/*! State Machine`s iterating interval. */
constexpr const std::size_t STATE_MACHINE_INTERVAL_SECONDS = 10;
}

StateMachineThread::StateMachineThread(StateMachineThreadAction& action) :
                       m_action(action) {}

StateMachineThread::~StateMachineThread() {
    m_is_running = false;
    m_condition.notify_one();
    m_thread.join();
}

void StateMachineThread::add_entry(const StateThreadEntrySharedPtr& entry) {
    std::lock_guard<std::mutex> lock{m_entry_mutex};
    m_entries.emplace_back(entry);
}

void StateMachineThread::remove_entry(const std::string& module_uuid) {
    std::lock_guard<std::mutex> lock{m_entry_mutex};
    const auto it = std::find_if(m_entries.cbegin(),
                                 m_entries.cend(),
                                 [&module_uuid]
                                 (const StateThreadEntrySharedPtr& entry) {
                                       return entry->get_module() == module_uuid;
                                 });
    if (it != m_entries.cend()) {
        m_entries.erase(it);
    }
}

namespace {
std::chrono::time_point<std::chrono::system_clock> get_time() {
    return std::chrono::system_clock::now()
                    + std::chrono::seconds(STATE_MACHINE_INTERVAL_SECONDS);
}
}

void StateMachineThread::task() {
    log_info(GET_LOGGER("state-machine"),
            "Starting State Machine thread...");

    while(m_is_running) {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (m_condition.wait_until(lk, ::get_time()) == std::cv_status::timeout) {

            std::lock_guard<std::mutex> lock{m_entry_mutex};
            for (auto& entry : m_entries) {
                try {
                    entry->next_state();
                    if (entry->is_state_changed()) {
                        log_debug(GET_LOGGER("agent"), "Module " << entry->get_module()
                                                   << " status changed to " << entry->get_state().to_string()
                                                   << " after event " << entry->get_transition().to_string());
                        m_action.execute(entry->get_module(), entry->get_state(), entry->get_transition());
                    }
                }
                catch (const state_machine::StateMachineError& e) {
                    log_error(GET_LOGGER("agent"),
                              "Cannot set next state.");
                    log_debug(GET_LOGGER("agent"), e.what());
                }
            }
        }
    }

    // Exiting safty...
    log_debug(GET_LOGGER("state-machine"), "State Machine thread stopped.");
}

void StateMachineThread::start() {
    m_thread = std::thread(&StateMachineThread::task, this);
    m_is_running = true;
}
