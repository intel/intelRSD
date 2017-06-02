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
 * @file state_machine_thread.hpp
 * @brief StateMachine thread.
 * */

#pragma once
#include "agent-framework/state_machine/state_machine_module_thread.hpp"
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

/*! StateMachine thread. */
class StateMachineThread {
public:
    using StateThreadEntrySharedPtr = std::shared_ptr<StateThreadEntry>;
    using StateThreadEntryVec = std::vector<StateThreadEntrySharedPtr>;

    using StateMachineModuleThreadSharedPtr = std::shared_ptr<StateMachineModuleThread>;
    using StateMachineModuleThreadVec = std::vector<StateMachineModuleThreadSharedPtr>;
    /*!
     * Default constructor.
     * */
    explicit StateMachineThread();

    /*! Default destructor. */
    ~StateMachineThread();

    /*!
     * @brief Start state machine thread
     */
    void start();

    /*!
     * @brief Add Module Thread
     * @param module_thread Module Thread
     */
    void add_module_thread(const StateMachineModuleThreadSharedPtr& module_thread);

private:
    void task();

    std::thread m_thread{};
    std::condition_variable m_condition{};
    std::mutex m_mutex{};
    std::atomic<bool> m_is_running{false};

    std::mutex m_module_thread_mutex{};
    StateThreadEntryVec m_entries{};

    StateMachineModuleThreadVec m_module_threads{};
};

/*! State Machine Thread unique pointer */
using StateMachineThreadUniquePtr = std::unique_ptr<StateMachineThread>;

}
}

