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
 * @file module_state.hpp
 * @brief Abstract Module State.
 * */

#ifndef AGENT_FRAMEWORK_STATE_MACHINE_MODULE_STATE_HPP
#define AGENT_FRAMEWORK_STATE_MACHINE_MODULE_STATE_HPP

#include "agent-framework/module-ref/enum/enum_builder.hpp"

#include <memory>
#include <array>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace state_machine {
namespace enums {
/*!
* @brief ENUM States of State Machine
*/
ENUM(State, uint32_t, UNKNOWN, ENABLED, STANDBY_OFFLINE, ABSENT, STARTING);
}

class StateMachineTransition;

/*!
 * @brief Virtual interafce to %StateMachine state.
 */
class ModuleState {
public:
    /*!
     * @brief Class default constructor.
     */
    ModuleState();

    /*!
     * @brief Class constructor.
     * @param[in] state Initial state.
     */
    ModuleState(enums::State state) : m_state(state) {}

    /*!
     * @brief Class destructor.
     */
    virtual ~ModuleState();

    /*!
     * @brief Virtual interface for setting next %ModuleState.
     * @param[in] transition %StateMachine transition object.
     * @param[in] new_state %StateMachine next state.
     */
    virtual void enter_state(StateMachineTransition& transition,
                             enums::State new_state) = 0;

    /*!
     * @brief Getter method for state value.
     * @return Integer value of a partucular state.
     */
    enums::State get_state() {
        return m_state;
    }

    /**
     * @brief Return state name
     * @param state module state
     * @return state name
     */
    static const char* get_state_name(enums::State state);

protected:
    /*!
     * @brief Integer value of a particular state.
     */
    enums::State m_state{enums::State::UNKNOWN};
};

using ModuleStateSharedPtr = std::shared_ptr<ModuleState>;

}
}

#endif /* AGENT_FRAMEWORK_STATE_MACHINE_MODULE_STATE_HPP */
