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
 * @file module_state_offline.hpp
 * @brief StateMachine OFFLINE State
 * */

#pragma once
#include "agent-framework/state_machine/module_state.hpp"

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace state_machine {

/*!
 * @brief %StateMachine OFFLINE state.
 */
class ModuleStateOffline final : public ModuleState {
public:
    /*!
     * @brief Class default constructor.
     */
    ModuleStateOffline() : ModuleState(enums::State::UNAVAILABLE_OFFLINE) {}

    /*!
     * @brief Class destructor.
     */
    ~ModuleStateOffline() {}

    /*!
     * @brief Implementation of virtual interface that sets next %ModuleState.
     * @param[in] transition Reference to transition object
     * @param[in] next_state %StateMachine next state.
     */
    void enter_state(StateMachineTransition& transition,
                     enums::State next_state) override;
};

}
}
