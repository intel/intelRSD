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
 * @file state_machine/port_state_manager.hpp
 * @brief Class responsible for controlling the drive discovery process
 * */

#pragma once



#include "state_machine/port_state_machine.hpp"
#include "state_machine/port_state_worker.hpp"
#include "discovery/discovery_manager.hpp"



namespace agent {
namespace pnc {
namespace state_machine {

/*! Class handling the port discovery process */
class PortStateManager final {
public:

    /*!
     * @brief Constructor
     * @param[in] worker Worker used to perform transision actions
     * @param[in] id Identifier of the port state manager, used for logging
     * @param[in] switch_uuid Uuid of the switch that is a parent for a provided dsp port
     * @param[in] port_uuid Donwstream port uuid that will be managed by the created manager
     * */
    PortStateManager(PortStateWorkerPtr worker, const std::string& id, const std::string& switch_uuid,
                     const std::string& port_uuid);


    /*!
     * Copy constructor is deleted!
     * PortStateManager initializes state manager with references to itself - copying is not allowed
     */
    PortStateManager(const PortStateManager&) = delete;


    /*!
     * Move constructor is deleted!
     * PortStateManager initializes state manager with references to itself - moving is not allowed
     */
    PortStateManager(PortStateManager&&) = delete;


    /*!
     * Copy assignment operator is deleted!
     * PortStateManager initializes state manager with references to itself - copying is not allowed
     */
    PortStateManager& operator=(const PortStateManager&) = delete;


    /*!
     * Move assignment operator is deleted!
     * PortStateManager initializes state manager with references to itself - moving is not allowed
     */
    PortStateManager& operator=(PortStateManager&&) = delete;


    /*! Destructor */
    ~PortStateManager();


    /*!
     * @brief Initializes binding information - should be called first
     * @param[in] is_bound Is the managed port bound to any partiotion
     * @param[in] is_bound_to_host Is the managed port bound to the management partition
     */
    void init_binding(bool is_bound, bool is_bound_to_host);


    /*!
     * @brief Initializes link presence - should be called after init_binding
     * @param[in] is_present Is any device present on the port
     */
    void init_presence(bool is_present);


    /*!
     * @brief Updates the state of the manager - generates proper events depending on the current state
     * @param[in] is_present Is any of the devices present on the port
     * @param[in] is_bound Is the port bound to any partition
     * @param[in] is_bound_to_host Is the port bound to management host
     * @param[in] is_being_erased Is the drive being erased
     */
    void update(bool is_present, bool is_bound, bool is_bound_to_host, bool is_being_erased);


    /*
     * @brief Returns uuid of the port that is being managed by the manager
     * @return Port uuid
     * */
    std::string get_port_uuid() const {
        return m_port_uuid;
    }


    /*
     * @brief Returns uuid of the device (i.e. Drive) that is managed by the manager
     * @return Device uuid
     * */
    std::string get_device_uuid() const {
        return m_device_uuid;
    }


    /*!
     * @brief Checks if there is a drive managed by the manager
     * @return True if there is a drive attached to the managed port
     * */
    bool is_device_present() const {
        return (PortState::Oob_Busy == m_sm.get_current_state() || PortState::Full_Standby == m_sm.get_current_state())
               && !m_device_uuid.empty();
    }


private:

    /*! Generate events for an already initialized state machine (after init_link) */
    void generate_events(bool is_present, bool is_bound, bool is_bound_to_host, bool is_being_erased);


    /*! State machine action: bind port to the management partition */
    bool action_bind(const PortTransition&);


    /*! State machine action: unbind port to the management partition */
    bool action_unbind(const PortTransition&);


    /*! State machine action: perform full discovery and unbind */
    bool action_full_unbind(const PortTransition&);


    /*! State machine action: bind to the management partition, perform full discovery, unbind and restore bindings */
    bool action_full_bind_unbind_recover(const PortTransition&);


    /*! State machine action: out-of-band discovery */
    bool action_oob(const PortTransition&);


    /*! State machine action: remove model data */
    bool action_remove(const PortTransition&);


    /*! State machine action: bind to the management partition, perform in-band discovery and unbind */
    bool action_ib_bind_unbind(const PortTransition&);


    /*! State machine action: recovers bindings */
    bool action_bindings_recover(const PortTransition&);


    /*! State machine action: unbind port from non-mgmt host */
    bool action_unbind_nonmgmt(const PortTransition&);


    /*! State machine action: unbind port from non-mgmt host, bind to the management partition, perform in-band discovery and unbind */
    bool action_unbind_nonmgmt_ib_discovery(const PortTransition&);


    /*! Worker used to implement transition actions */
    PortStateWorkerPtr m_worker;

    /*! Instance of the state machine */
    PortStateMachine m_sm;

    /*! Previous binding state */
    bool m_prev_bound{false};

    /*! Previous presence flag state */
    bool m_prev_present{false};

    /*! Uuid of the switch of the port */
    std::string m_switch_uuid{};

    /*! Uuid of the port */
    std::string m_port_uuid{};

    /*! Uuid of the device (i.e. drive) on the port */
    std::string m_device_uuid{};

    PortStateWorker::ZoneEndpointVector m_recoverable_bindings{};

};

}
}
}
