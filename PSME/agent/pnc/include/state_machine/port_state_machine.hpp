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
 * @file state_machine/port_state_machine.hpp
 * @brief Class responsible for controlling the port discovery process
 * */

#pragma once

#include "state_machine/enum_state_machine.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"

namespace agent {
namespace pnc {
namespace state_machine {

/*! Enum representing the state of the port */
ENUM(PortState, unsigned,
    // state name               // discovery state        // port binding state
    Initial,                    // Unknown                // Unknown
    Init_MgmtHost,              // Unknown                // Bound to the Management Partition
    Init_NonMgmtHost,           // Unknown                // Bound to any NON-management partition
    Oob_Busy,                   // Out-of-band discovery  // Bound to any NON-management partition
    Full_Standby,               // Full discovery         // Unknown (may change while agent is working)
    NoDevice,                   // No data                // Unbound
    NoDevice_Busy               // No data                // Bound to any NON-management partition
);

/*! Possible events sent to the machine. */
ENUM(PortEvent, unsigned,
    BindingToMgmtHostDetected,    // Port binding initialization event, port is bound to the management partition
    BindingToNonMgmtHostDetected, // Port binding initialization event, port is bound to any NON-management partition
    BindingNotDetected,           // Port binding initialization event, port was unbound
    DeviceFound,                  // Device presence initialization event, device is present on the port
    DeviceNotFound,               // Device presence initialization event, no devices are present on the port
    DeviceUnbound,                // Update event, previously bound port was unbound
    DeviceHotPlugged,             // Update event, hotplug detected
    DeviceHotUnplugged,           // Update event, hotunplug detected
    ModelBindingNotDetected,      // Synchronization event, binding from model does not exists on switch
    IllegalBindingDetected        // Synchronization event, binding on switch is illegal (does not exist in model)
);

/*! Declaration of the PortStateMachine class */
class PortStateMachine : public agent::pnc::state_machine::EnumStateMachine<PortState, PortEvent> {
public:

    /*!
     * Constructor
     * @param[in] id Id of the state machine, used for logging
     * @param[in] init_state Initial state of the machine
     * @param[in] transition_table Transition table of the machine
     * */
    PortStateMachine(const std::string& id, const PortState& init_state,
        const PortStateMachine::TransitionTable& transition_table = {});

    /*! Copy constructor */
    PortStateMachine(const PortStateMachine&) = default;

    /*! Move constructor */
    PortStateMachine(PortStateMachine&&) = default;

    /*! Copy assignment operator */
    PortStateMachine& operator=(const PortStateMachine&) = default;

    /*! Move assignment operator */
    PortStateMachine& operator=(PortStateMachine&&) = default;

    /*! Destructor */
    virtual ~PortStateMachine();

    /*!
     * @brief Returns true if the machine was initialized
     * @return True if in any state other than Initial
     * */
    bool is_initialized() const {
        return PortState::Initial != get_current_state();
    }

protected:

    /*!
     * @brief Overriden version that includes logging
     * @param[in] transition Information about the transition
     * @return True if action was successful
     * */
    virtual bool do_transition(const Transition& transition) override;

    /*!
     * @brief Method called as a first method in send_event call, logs event
     * @param[in] event Event that was sent to the machine
     * */
    virtual void do_on_event_action(const PortEvent& event) override;

private:
    const std::string m_id{};
};

/*! Declaration of the Drive State Transition */
using PortTransition = PortStateMachine::Transition;

}
}
}
