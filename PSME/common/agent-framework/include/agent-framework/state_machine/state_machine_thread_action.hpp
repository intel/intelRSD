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
#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/state_machine/state_machine_transition.hpp"
#include "agent-framework/state_machine/state_thread_entry.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace state_machine {

class StateMachineThreadAction {
public:
    using StateThreadEntrySharedPtr = std::shared_ptr<StateThreadEntry>;

    StateMachineThreadAction() = default;
    StateMachineThreadAction(const StateMachineThreadAction&) = default;
    StateMachineThreadAction& operator=(const StateMachineThreadAction&) = default;
    StateMachineThreadAction(StateMachineThreadAction&&) = default;
    StateMachineThreadAction& operator=(StateMachineThreadAction&&) = default;

    /*! Default destructor. */
    virtual ~StateMachineThreadAction();

    virtual void execute(StateThreadEntrySharedPtr entry);
protected:
    std::mutex m_mutex{};
};

}
}

