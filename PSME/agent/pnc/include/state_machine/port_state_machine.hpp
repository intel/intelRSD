/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
    // state name   // discovery state        // port binding state
    Initial,        // Unknown                // Unknown
    InitBoundHost,  // Unknown                // Bound to the Management Partition
    InitBoundOther, // Unknown                // Bound to any NON-management partition
    InitUnbound,    // Unknown                // Unbound
    OobInUse,       // Out-of-band discovery  // Bound to any NON-management partition
    Standby,        // Full discovery         // Unbound
    NotPresent,     // No data                // Unbound
    NotPresentInUse // No data                // Bound to any NON-management partition
);

/*! Possible events sent to the machine. */
ENUM(PortEvent, unsigned,
    IsBoundToHost,      // Port binding initialization event, port is bound to the management partition
    IsBoundToOther,     // Port binding initialization event, port is bound to any NON-management partition
    IsUnbound,          // Port binding initialization event, port was unbound
    DevicePresent,      // Device presence initialization event, device is present on the port
    DeviceNotPresent,   // Device presence initialization event, no devices are present on the port
    WasUnbound,         // Update event, previously bound port was unbound
    HotUnplug,          // Update event, hotplug detected
    HotPlug             // Update event, hotunplug detected
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
        const PortStateMachine::TransitionTable& transition_table);

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
    virtual void pre_event_check(const PortEvent& event) override;

private:
    const std::string m_id{};
};

/*! Declaration of the Drive State Transition */
using PortTransition = PortStateMachine::Transition;

}
}
}
