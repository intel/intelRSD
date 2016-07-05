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
 * @file state_machine_transition.hpp
 * @brief %StateMachine abstract transition
 */

#ifndef AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_TRANSITION_HPP
#define AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_TRANSITION_HPP

#include "agent-framework/module-ref/enum/enum_builder.hpp"

#include <memory>
#include <cstdio>
#include <array>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace state_machine {
namespace enums {
/*!
* @brief ENUM Transition of State Machine
*/
ENUM(Transition, uint32_t, IDLE, EXTRACTION, INSERTION, WENT_DOWN, CAME_UP,
     DISCOVERY_UP, DISCOVERY_DOWN, DISCOVERY_MISSING);
}

/*!
 * @brief Virtual interafce to %StateMachine transition.
 */
class StateMachineTransition {
public:
    /*!
     * @brief Default class constructor.
     */
    StateMachineTransition() = default;

    /*!
     * @brief Class constructor.
     * @param[in] transition Initial transition.
     */
    StateMachineTransition(const enums::Transition transition) :
        m_transition(transition) {}

    /*!
     * @brief Getter method for transition value.
     * @return Integer value of a partucular transition.
     */
    enums::Transition get_transition() const {
        return m_transition;
    }

    /*!
     * @brief Setter method for transition value.
     * @param transition Transition value to set.
     */
    void set_transition(const enums::Transition transition) {
        m_transition = transition;
    }

    /**
     * @brief Gets transition name
     *
     * @param transition state machine transition
     *
     * @return transition name
     */
    static const char* get_transition_name(const enums::Transition transition);

private:
    enums::Transition m_transition{enums::Transition::IDLE};
};

}
}
#endif /* AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_TRANSITION_HPP */


