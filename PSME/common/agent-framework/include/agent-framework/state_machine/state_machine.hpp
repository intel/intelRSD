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
 * @file state_machine.hpp
 * @brief StateMachine
 * */

#ifndef AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_HPP
#define AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_HPP

#include "agent-framework/state_machine/module_state.hpp"
#include "agent-framework/state_machine/state_machine_transition.hpp"
#include "agent-framework/status/module_status.hpp"

#include <map>
#include <memory>
#include <mutex>

namespace agent_framework {
namespace state_machine {

using namespace agent_framework::status;

/*!
 * @brief Main class of a state machine.
 */
class StateMachine {
public:
    /*!
     * @brief Class default constructor.
     */
    StateMachine();

    /*!
     * @brief Starts %StateMachine.
     * @throw StateMachineException.
     */
    void start();

    /*!
     * @brief Stops %StateMachine.
     * @throw StateMachineException.
     */
    void stop();

    /*!
     * @brief Getter to current state of the %StateMachine.
     * @return Current state.
     */
    enums::State get_state() const;

    /*!
     * @brief Getter to current transition of the state machine.
     * @return %StateMachine transition.
     */
    enums::Transition get_transition() const;

    /*!
     * @brief Sets next state.
     *
     * @param hw_status Current hardware status
     * @param sw_status Current software status
     */
    void set_next_state(ModuleStatus::Status hw_status,
                        ModuleStatus::Status sw_status);

    /*!
     * @brief Check if current state changed
     * @return true if status changed
     */
    bool is_state_changed() const;

private:
    using StateMap = std::map<enums::State, ModuleStateSharedPtr>;

    ModuleStateSharedPtr m_curr_state{nullptr};
    StateMachineTransition m_curr_transition{
            enums::Transition::IDLE};
    StateMap m_states{};
    mutable std::mutex m_mutex{};

    ModuleStateSharedPtr get_state(const enums::State state);

    enums::State
    get_next_state(const status::ModuleStatus::Status hw_status,
                   const status::ModuleStatus::Status sw_status);

    bool is_enabled_state(const status::ModuleStatus::Status hw_status,
                          const status::ModuleStatus::Status sw_status);

    bool is_offline_state(const status::ModuleStatus::Status hw_status,
                          const status::ModuleStatus::Status sw_status);

    bool is_starting_state(const status::ModuleStatus::Status hw_status,
                          const status::ModuleStatus::Status sw_status);

};

}
}
#endif /* AGENT_FRAMEWORK_STATE_MACHINE_STATE_MACHINE_HPP */
