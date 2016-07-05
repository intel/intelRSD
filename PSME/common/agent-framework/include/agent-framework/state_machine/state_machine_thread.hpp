/*!
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
 *
 * @file state_machine_thread.hpp
 * @brief StateMachine thread.
 * */

#ifndef AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_THREAD_HPP
#define AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_THREAD_HPP

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
    /*!
     * Default constructor.
     * */
    explicit StateMachineThread(StateMachineThreadAction& action);

    /*! Default destructor. */
    ~StateMachineThread();

    /*!
     * @brief Start state machine thread
     */
    void start();

    void add_entry(const StateThreadEntrySharedPtr& entry);

    void remove_entry(const std::string& module_uuid);

private:
    void task();

    std::thread m_thread{};
    std::condition_variable m_condition{};
    std::mutex m_mutex{};
    std::atomic<bool> m_is_running{false};

    std::mutex m_entry_mutex{};
    StateThreadEntryVec m_entries{};

    StateMachineThreadAction& m_action;
};

/*! State Machine Thread unique pointer */
using StateMachineThreadUniquePtr = std::unique_ptr<StateMachineThread>;

}
}
#endif /* AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_THREAD_HPP */

