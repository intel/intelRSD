/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file state_machine/enum_state_machine.hpp
 * @brief Declaration of the EnumStateMachine template
 * */

#pragma once

#include <functional>
#include <vector>

#include "wrap.hpp"

namespace agent_framework {
namespace state_machine {

/*!
* Class representing a simple enum-based state machine. It uses enums to represent states and events, therefore
* states cannot really store their own data. State machine is parametrized with two types: STATE which is a type of
* machine states and EVENT representing Events type.
* */
template <typename STATE, typename EVENT>
class EnumStateMachine {
public:

    struct Transition;

    /*!
     * Type definition of the functions used in state machines - used for guards and actions. They accept one
     * parameter that describe the transition and return bool value: for actions, true means that action was
     * successful, for guards, true means that the transition may be started.
     * */
    using StateMachineFunction = std::function<bool(const Transition&)>;

    /*! Type declaration of the TransitionTable */
    using TransitionTable = std::vector<Transition>;

    /*! Struct representing single transition of the state machine */
    struct Transition final {
    public:

        /*!
         * @brief Constructor
         * @param[in] i_s Initial state before the transition
         * @param[in] e Event that triggers the transition
         * @param[in] e_s End state after the transition
         * @param[in] a Action triggered by the transition (default action = no action)
         * @param[in] g Guard checking if transition should be performed (default guard = always)
         * */
        Transition(const STATE& i_s, const EVENT& e, const STATE& e_s,
                StateMachineFunction a = do_nothing, StateMachineFunction g = always):
            init_state(i_s), event(e), end_state(e_s), action(a), guard(g) {}

        /*!
         * @brief Default action during transitions - does nothing
         * @return Always returns true
         * */
        static constexpr bool do_nothing(const Transition&) { return true; }

        /*!
         * @brief Default guard - always true
         * @return Always returns true
         * */
        static constexpr bool always(const Transition&) { return true; }

        /*! Initial state required for the transition */
        STATE init_state;

        /*! Event triggering the transition */
        EVENT event;

        /*! End state after the transition */
        STATE end_state;

        /*! Action triggered by the transition */
        StateMachineFunction action;

        /*! Additional condition for transition to take place */
        StateMachineFunction  guard;
    };

    /*!
     * @brief Constructor
     * @param[in] init_state Initial state of the machine
     * @param[in] transition_table Transition table describing transitions allowed in the
     *            state machine (default = no transitions, empty table)
     * */
    EnumStateMachine(const STATE& init_state, const TransitionTable& transition_table = {}):
        m_current_state(init_state), m_transition_table(transition_table) {}

    /*! Copy constructor */
    EnumStateMachine(const EnumStateMachine&) = default;

    /*! Move constructor */
    EnumStateMachine(EnumStateMachine&&) = default;

    /*! Copy Assignment operators */
    EnumStateMachine& operator=(const EnumStateMachine&) = default;

    /*! Move Assignment operators */
    EnumStateMachine& operator=(EnumStateMachine&&) = default;

    /*!
     * @brief Destructor
     * */
    virtual ~EnumStateMachine() {}

    /*!
     * @brief Adds a transition to the state machine
     * @param[in] init_state Initial state before the transition
     * @param[in] event Event that triggers the transition
     * @param[in] end_state End state after the transition
     * @param[in] action Action triggered by the transition (default action = no action)
     * @param[in] guard Guard checking if transition should be performed (default guard = always)
     * @return Reference to the state machine
     * */
    EnumStateMachine& add_transition(const STATE& init_state, const EVENT& event, const STATE& end_state,
            StateMachineFunction action = Transition::do_nothing, StateMachineFunction guard = Transition::always) {
        m_transition_table.emplace_back(Transition{init_state, event, end_state, action, guard});
        return *this;
    }

    /*!
     * @brief Returns current machine state
     * @return Current machine state
     * */
    STATE get_current_state() const {
        return m_current_state;
    }

    /*!
     * @brief Sends specific event to the machine
     * @param[in] event Event sent to the state machine
     * @return False if the event would cause state machine to change its state, but due to state machine action
     *         failure, state was not changed and transition did not happen.
     * */
    bool send_event(const EVENT& event) {
        do_on_event_action(event);
        for (const auto& transition : m_transition_table) {
            if (transition.init_state == m_current_state && transition.event == event && do_guard_check(transition)) {
                if (!do_transition(transition)) {
                    return false;
                }
                m_current_state = transition.end_state;
                return true;
            }
        }
        return true;
    }

protected:

    /*!
     * @brief Method used to check guards of the transition.
     *        Should be overriden in derived classes if change of the behaviour is desired.
     * @param[in] transition Information about the transition
     * @return True if the guard returned true
     * */
    virtual bool do_guard_check(const Transition& transition) {
        return transition.guard(transition);
    }

    /*!
     * @brief Method used to perform transition action.
     *        Should be overriden in derived classes if change of the behaviour is desired.
     * @param[in] transition Information about the transition
     * @return True if action was successful
     * */
    virtual bool do_transition(const Transition& transition) {
        return transition.action(transition);
    }

    /*!
     * @brief Method called as a first method in send_event call
     *        Should be overriden in derived classes if change of the behaviour is desired.
     * */
    virtual void do_on_event_action(const EVENT&) {
    }



private:

    /*! Current state of the state machine */
    STATE m_current_state{};

    /*! Transition table of the state machine */
    TransitionTable m_transition_table{};

};

}
}
