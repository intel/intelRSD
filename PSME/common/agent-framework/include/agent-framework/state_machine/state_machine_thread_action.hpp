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

#ifndef AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_THREAD_ACTION_HPP
#define AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_THREAD_ACTION_HPP

#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/state_machine/state_machine_transition.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace state_machine {

class StateMachineThreadAction {
public:
    StateMachineThreadAction() = default;
    StateMachineThreadAction(const StateMachineThreadAction&) = default;
    StateMachineThreadAction& operator=(const StateMachineThreadAction&) = default;
    StateMachineThreadAction(StateMachineThreadAction&&) = default;
    StateMachineThreadAction& operator=(StateMachineThreadAction&&) = default;

    /*! Default destructor. */
    virtual ~StateMachineThreadAction();

    virtual void execute(const std::string& uuid,
                         const enums::State state,
                         const enums::Transition trans);
};

}
}
#endif /* AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_THREAD_HPP */

