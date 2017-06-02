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
 *
 *
 * @file state_machine_module.hpp
 * @brief State Machine Module thread.
 * */

#pragma once
#include "agent-framework/state_machine/state_machine_thread_action.hpp"
#include "agent-framework/state_machine/state_thread_entry.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>

/*! Psme namespace */
namespace agent_framework {
namespace state_machine {

using StateThreadEntrySharedPtr = std::shared_ptr<StateThreadEntry>;
using StateThreadEntryVec = std::vector<StateThreadEntrySharedPtr>;

    /*! State Machine Module thread. */
class StateMachineModuleThread {
public:
    /*!
     * Default constructor.
     * */
    explicit StateMachineModuleThread(StateThreadEntrySharedPtr& entry, StateMachineThreadAction& action);

    /*! Default destructor. */
    ~StateMachineModuleThread();

    /*!
     * @brief Start state machine thread
     */
    void start();

    /*!
     * @brief Returns running flag
     * @return true if thread is running
     */
    bool is_running();

    /*!
     * @brief Sets State Thread Entry
     * @param entry State Thread Entry
     */
    void set_entry(const StateThreadEntrySharedPtr& entry);

    /*!
     * @brief Gets State Thread Entry
     * @return State Thread Entry
     */
    StateThreadEntrySharedPtr get_entry();

    void remove_entry(const std::string& module_uuid);

private:
    void task();

    std::thread m_thread{};
    std::condition_variable m_condition{};
    std::mutex m_mutex{};
    std::atomic<bool> m_is_running{false};
    std::mutex m_entry_mutex{};
    StateThreadEntrySharedPtr m_entry;
    StateMachineThreadAction& m_action;
};

/*! State Machine Thread unique pointer */
using StateMachineModuleThreadUniquePtr = std::unique_ptr<StateMachineModuleThread>;

}
}

