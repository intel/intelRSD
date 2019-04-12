/*!
 * @brief Simple Bmc FSM.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file agent-framework/state_machine/bmc_state_machine.hpp
 */

#pragma once

#include "enum_state_machine.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"
#include "logger/logger_factory.hpp"

namespace agent_framework {
namespace state_machine {

/*! Enum representing the state of the BMC */
ENUM(BmcState, unsigned,
    ABSENT,
    OFFLINE,
    ONLINE
);

/*! Possible events sent to the machine. */
ENUM(BmcEvent, unsigned,
    Inserted,
    Extracted,
    BecameOnline,
    BecameOffline
);

/*!
 * @brief Bmc state machine with state changes described by TransitionTable.
 *
 * <!--
 * @startuml
 * [*] -> Absent
 * Absent : Resource is not present or not detected
 * Absent --> Offline : inserted
 * Offline --> Online : became_online
 * Offline: Resource is present but cannot be used
 * Offline --> Absent : extracted
 *
 * Online --> Absent : extracted
 * Online --> Offline : became_offline
 * @enduml
 * -->
 */
class BmcStateMachine : protected EnumStateMachine<BmcState, BmcEvent> {
public:
    /* Always false functor */
    static constexpr bool always_false() { return false; }

    /*! Functor returning current presence status */
    using ReadPresenceFn = std::function<bool()>;

    /*! Functor returning current online status */
    using ReadOnlineStatusFn = std::function<bool()>;

    /*!
     * @brief Constructor
     * @param id Identifier used mainly for logging
     * @param read_presence Functor returning current presence status
     * @param read_online_status Functor returning online status
     */
    BmcStateMachine(const std::string& id,
                    ReadPresenceFn read_presence = always_false,
                    ReadOnlineStatusFn read_online_status = always_false)
     : m_id{id}, m_read_presence{read_presence}, m_read_online_status{read_online_status}
       , EnumStateMachine{BmcState::ABSENT, BmcStateMachine::TransitionTable{
         {BmcState::ABSENT,  BmcEvent::Inserted,     BmcState::OFFLINE, wrap(this, &BmcStateMachine::on_insertion)},
         {BmcState::OFFLINE, BmcEvent::BecameOnline, BmcState::ONLINE,  wrap(this, &BmcStateMachine::on_become_online)},
         {BmcState::OFFLINE, BmcEvent::Extracted,    BmcState::ABSENT,  wrap(this, &BmcStateMachine::on_extraction)},
         {BmcState::ONLINE,  BmcEvent::BecameOffline,BmcState::OFFLINE, wrap(this, &BmcStateMachine::on_become_offline)},
         {BmcState::ONLINE,  BmcEvent::Extracted,    BmcState::ABSENT,  wrap(this, &BmcStateMachine::on_extraction)}
    }} {}

    BmcStateMachine(const BmcStateMachine&) = delete;
    BmcStateMachine& operator=(const BmcStateMachine&) = delete;
    ~BmcStateMachine() override;


    /*!
     * @brief Current state getter.
     * @return Current state.
     */
    BmcState get_state() const {
        return get_current_state();
    }

    /*!
     * @brief State machine identifier getter (used mainly for logging).
     *
     * Currently BMC IP address is used as identifier.
     *
     * @return State machine identifier.
     */
    const std::string& get_id() const {
        return m_id;
    }

    /*!
     * @brief Updates current state.
     *
     * Based on read_presence and read_online_status output generates
     * state machine events.
     */
    void update_state() {
        try {
            update_presence();
            update_online_state();
        }
        catch (const std::exception& e) {
            log_error("bmc", " (" << m_id << ") update state failed: " << e.what());
        }
        catch (...) {
            log_error("bmc", " (" << m_id << ") update state failed.");
        }
    }

protected:
    bool do_transition(const Transition& transition) override {
        log_info("bmc", " (" << m_id << ") state transition "
                << transition.init_state.to_string() << " --> " << transition.end_state.to_string()
                << " caused by " << transition.event.to_string());
        const auto result = EnumStateMachine::do_transition(transition);
        log_info("bmc", " (" << m_id
                << ") transition status: " << (result ? "OK" : "FAILED"));
        return result;
    }

    void do_on_event_action(const BmcEvent& event) override {
        log_info("bmc", " (" << m_id << ") in state: "
            << get_current_state().to_string() << " received event: " << event.to_string());
    }

    /*!
     * @brief Action executed on insertion event.
     * @return true on success, false otherwise.
     */
    virtual bool on_insertion(const Transition&) {
        return true;
    }

    /*!
     * @brief Action executed on extraction event.
     * @return true on success, false otherwise.
     */
    virtual bool on_extraction(const Transition&) {
        return true;
    }

    /*!
     * @brief Action executed on become online event.
     * @return true on success, false otherwise.
     */
    virtual bool on_become_online(const Transition&) {
        return true;
    }

    /*!
     * @brief Action executed on become offline event.
     * @return true on success, false otherwise.
     */
    virtual bool on_become_offline(const Transition&) {
        return true;
    }

private:

    void update_presence() {
        auto is_present = m_read_presence();
        if (is_present != m_is_present) {
            // send_event calls action which may throw
            // we update m_is_online only when action succeeds (causing a state change)
            is_present ? send_event(BmcEvent::Inserted)
                         : send_event(BmcEvent::Extracted);
            m_is_present = is_present;
        }
    }

    void update_online_state() {
        if (m_is_present) {
            auto is_online = m_read_online_status();
            if (is_online != m_is_online) {
                // send_event calls action which may throw
                // we update m_is_online only when action succeeds (causing a state change)
                is_online ? send_event(BmcEvent::BecameOnline)
                            : send_event(BmcEvent::BecameOffline);
                m_is_online = is_online;
            }
        }
        else {
            if (m_is_online) {
                log_info("bmc", " (" << m_id
                    << ") online changed to false");
            }
            m_is_online = false;
        }
    }

    std::string m_id{};
    bool m_is_present{false};
    bool m_is_online{false};

    ReadPresenceFn m_read_presence;
    ReadOnlineStatusFn m_read_online_status;
};

} // state_machine
} // agent_framework
