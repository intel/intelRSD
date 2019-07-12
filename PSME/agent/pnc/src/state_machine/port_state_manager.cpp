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
 * @file state_machine/port_state_manager.cpp
 * @brief Implementation of the PortStateManager
 * */

#include "discovery/discovery_manager.hpp"
#include "discovery/discovery_utils.hpp"
#include "state_machine/port_state_manager.hpp"
#include "state_machine/wrap.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "tools/toolset.hpp"

#include <thread>
#include <chrono>



using namespace agent::pnc::state_machine;
using namespace agent::pnc::discovery;
using namespace agent::pnc::discovery::utils;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent::pnc::tools;

using PS = PortState;
using PE = PortEvent;
using PSM = PortStateManager;


/*
 * How this state machine works:
 * - At the beginning there are two events sent to initialize state machine. First event gets the binding state. Port
 *   may be either bound to the management partition, other partition or unbound (in that case it is instantly bound
 *   to the host).
 * - Second event sets the link status. If it is up - some discovery may be done (full or out-of-band, depending on
 *   on the partition the port is bound to).
 * - If the port could have been bound to the management partition it is fully discovered and left in the
 *   Full_Standby state.
 *   In this state we cannot monitor the link state (port has to be bound somewhere for it to be checked). The same
 *   happens in the NoDevice state - port is not bound, so the only possibility for the state change is getting
 *   one of the hotswap events.
 * - If the port is bound to any partition - it's link status may be checked. So we do not have to rely on hotswap
 *   events, link status is enough. Each time link goes down we remove data, and when it goes up - we do out-of-band
 *   discovery.
 * - In case port was unbound from the partition it was 'blocking' its discovery - it is instantly in-band discovered
 */

PortStateManager::PortStateManager(PortStateWorkerPtr worker, const std::string& id, const std::string& switch_uuid,
                                   const std::string& port_uuid) :
    m_worker(worker), m_sm(id, PS::Initial), m_switch_uuid(switch_uuid), m_port_uuid(port_uuid) {

    // First event initialization transitions
    m_sm.add_transition(PS::Initial, PE::BindingToMgmtHostDetected,    PS::Init_MgmtHost);
    m_sm.add_transition(PS::Initial, PE::BindingToNonMgmtHostDetected, PS::Init_NonMgmtHost);
    m_sm.add_transition(PS::Initial, PE::BindingNotDetected,           PS::Init_MgmtHost,    wrap(this, &PSM::action_bind));

    // Second event initialization transitions
    m_sm.add_transition(PS::Init_MgmtHost,    PE::DeviceFound,    PS::Full_Standby,  wrap(this, &PSM::action_full_unbind));
    m_sm.add_transition(PS::Init_MgmtHost,    PE::DeviceNotFound, PS::NoDevice,      wrap(this, &PSM::action_unbind));
    m_sm.add_transition(PS::Init_NonMgmtHost, PE::DeviceFound,    PS::Oob_Busy,      wrap(this, &PSM::action_oob));
    m_sm.add_transition(PS::Init_NonMgmtHost, PE::DeviceNotFound, PS::NoDevice_Busy);

    // Unbind transitions
    m_sm.add_transition(PS::Oob_Busy,      PE::DeviceUnbound, PS::Full_Standby, wrap(this, &PSM::action_ib_bind_unbind));
    m_sm.add_transition(PS::NoDevice_Busy, PE::DeviceUnbound, PS::NoDevice);

    // Hotswap transitions
    m_sm.add_transition(PS::Oob_Busy,      PE::DeviceHotUnplugged, PS::NoDevice,      wrap(this, &PSM::action_remove));
    m_sm.add_transition(PS::Full_Standby,  PE::DeviceHotUnplugged, PS::NoDevice,      wrap(this, &PSM::action_remove));
    m_sm.add_transition(PS::NoDevice_Busy, PE::DeviceHotPlugged,   PS::Oob_Busy,      wrap(this, &PSM::action_oob));
    m_sm.add_transition(PS::NoDevice,      PE::DeviceHotPlugged,   PS::Full_Standby,  wrap(this, &PSM::action_full_bind_unbind_recover));

    // Synchronization with model transitions
    m_sm.add_transition(PS::Full_Standby, PE::ModelBindingNotDetected, PS::Full_Standby, wrap(this, &PSM::action_bindings_recover));
    m_sm.add_transition(PS::Full_Standby, PE::IllegalBindingDetected,  PS::Full_Standby, wrap(this, &PSM::action_unbind_nonmgmt));
    m_sm.add_transition(PS::Oob_Busy,     PE::IllegalBindingDetected,  PS::Full_Standby, wrap(this, &PSM::action_unbind_nonmgmt_ib_discovery));
}


PortStateManager::~PortStateManager() {}


void PortStateManager::init_binding(bool is_bound, bool is_bound_to_host) {
    log_debug("port-state-manager", "StateMachine: updating binding state: bound = " << is_bound
                                                                                     << ", bound to host = "
                                                                                     << is_bound_to_host);
    m_prev_bound = is_bound;
    PortEvent ev = PE::BindingNotDetected;
    if (is_bound) {
        ev = is_bound_to_host ? PE::BindingToMgmtHostDetected : PE::BindingToNonMgmtHostDetected;
    }
    m_sm.send_event(ev);
}


void PortStateManager::init_presence(bool is_present) {
    log_debug("port-state-manager", "StateMachine: updating device presence: dev present = " << is_present);
    m_prev_present = is_present;
    PortEvent ev = is_present ? PE::DeviceFound : PE::DeviceNotFound;
    m_sm.send_event(ev);
}


void PortStateManager::update(bool is_present, bool is_bound, bool is_bound_to_host, bool is_being_erased) {

    log_debug("port-state-manager",
              "State machine - current state = " << m_sm.get_current_state().to_string());
    // if we are still in initial state - try initializing
    if (!m_sm.is_initialized()) {
        init_binding(is_bound, is_bound_to_host);
        init_presence(is_present);
    }
        // in other cases - just process the data
    else {
        generate_events(is_present, is_bound, is_bound_to_host, is_being_erased);
    }
}


void PortStateManager::generate_events(bool is_present, bool is_bound, bool is_bound_to_host, bool is_being_erased) {

    log_debug("port-state-manager", "StateMachine: updating states: device present = " << is_present
                                                                                       << ", is bound = " << is_bound
                                                                                       << ", is being erased = "
                                                                                       << is_being_erased);

    // while drive/FPGA is being erased, port may be unbound -> we need to account for it
    bool is_used = is_bound || is_being_erased;
    if (!is_used && m_prev_bound) {
        m_sm.send_event(PE::DeviceUnbound);
    }
    else if (m_prev_present != is_present) {
        m_sm.send_event(is_present ? PE::DeviceHotPlugged : PE::DeviceHotUnplugged);
    }
    // These transitions below has lower priority than transitions above and are executed next time if they overlap
    else if (!is_used && !is_bound_to_host && is_present && m_worker->is_port_in_valid_zone(m_port_uuid)) {
        m_sm.send_event(PE::ModelBindingNotDetected);
    }
    else if (is_bound && !is_bound_to_host && !is_being_erased && is_present &&
             !m_worker->is_port_in_valid_zone(m_port_uuid)) {
        m_sm.send_event(PE::IllegalBindingDetected);
    }

    m_prev_present = is_present;
    m_prev_bound = is_used;
}


bool PortStateManager::action_oob(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: oob discovery action started ...");
        m_device_uuid = m_worker->oob_discovery(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: oob discovery action finished");
    }
    catch (std::exception e) {
        log_error("port-state-manager", "StateMachine: oob discovery action failed: " << e.what());
        return false;
    }
    return true;
}


bool PortStateManager::action_remove(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: remove action started ...");
        // if endpoint is bound -> we have to restore this binding on Hot Plug
        auto bindings = m_worker->get_bindings_on_port(m_port_uuid);
        std::copy(bindings.begin(), bindings.end(), std::back_inserter(m_recoverable_bindings));
        std::sort(m_recoverable_bindings.begin(), m_recoverable_bindings.end());
        auto last = std::unique(m_recoverable_bindings.begin(), m_recoverable_bindings.end());
        m_recoverable_bindings.erase(last, m_recoverable_bindings.end());
        m_worker->remove(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: remove action finished");
        m_device_uuid = std::string{};
        return true;
    }
    catch (std::exception e) {
        log_error("port-state-manager", "StateMachine: remove action failed: " << e.what());
        return false;
    }
}


bool PortStateManager::action_ib_bind_unbind(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: bind/ib discovery/unbind action started ...");
        log_debug("port-state-manager", "StateMachine: Locking port...");
        m_worker->lock_port(m_port_uuid);
        log_debug("port-state-manager", "StateMachine: Binding...");
        uint8_t bridge_id = m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: Discovering...");
        m_worker->ib_discovery(m_switch_uuid, m_port_uuid, bridge_id, m_device_uuid);
        log_debug("port-state-manager", "StateMachine: Unbinding...");
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug("port-state-manager", "StateMachine: Unlocking port...");
        m_worker->unlock_port(m_port_uuid);
        log_debug("port-state-manager", "StateMachine: bind/ib discovery/unbind action finished");
        return true;
    }
    catch (std::exception e) {
        log_error("port-state-manager",
                  "StateMachine: bind/ib discovery/unbind action failed: " << e.what());
        return false;
    }
}


bool PortStateManager::action_bind(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: bind action started ...");
        m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: bind action finished");
        return true;
    }
    catch (std::exception e) {
        log_error("port-state-manager", "StateMachine: bind action failed: " << e.what());
        return false;
    }
}


bool PortStateManager::action_unbind(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: unbind action started ...");
        uint8_t bridge_id = m_worker->get_bridge_id(m_switch_uuid, m_port_uuid);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug("port-state-manager", "StateMachine: unbind action finished");
        return true;
    }
    catch (std::exception e) {
        log_error("port-state-manager", "StateMachine: unbind action failed: " << e.what());
        return false;
    }
}


bool PortStateManager::action_full_unbind(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: full discovery/unbind action started ...");
        uint8_t bridge_id = m_worker->get_bridge_id(m_switch_uuid, m_port_uuid);
        m_worker->full_discovery(m_switch_uuid, m_port_uuid, bridge_id);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug("port-state-manager", "StateMachine: full discovery/unbind action finished");
    }
    catch (std::exception e) {
        log_error("port-state-manager",
                  "StateMachine: full discovery/unbind action failed: " << e.what());
        return false;
    }
    try {
        m_device_uuid = m_worker->get_device_uuid_by_dsp_port(m_port_uuid);
    }
    catch (std::exception e) {
        log_debug("port-state-manager",
                  "StateMachine: no drive for device: " << e.what());
    }
    return true;
}


bool PortStateManager::action_full_bind_unbind_recover(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: bind/full discovery/unbind/recover action started ...");
        // discovery
        uint8_t bridge_id = m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        m_worker->full_discovery(m_switch_uuid, m_port_uuid, bridge_id);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        // recover bindings
        log_debug("port-state-manager", "StateMachine: Restoring bindings...");
        auto endpoints = m_worker->get_enabled_endpoints_on_port(m_port_uuid);
        PortStateWorker::ZoneEndpointVector new_bindings{};
        for (const auto& endpoint_uuid : endpoints) {
            for (const auto& binding : m_recoverable_bindings) {
                if (std::get<1>(binding) == endpoint_uuid) {
                    log_debug("port-state-manager", "StateMachine: Binding to be recovered found!");
                    m_worker->attach_endpoint_to_zone(m_switch_uuid, endpoint_uuid, std::get<0>(binding));
                }
                else {
                    new_bindings.push_back(binding);
                }
            }
        }
        m_recoverable_bindings = std::move(new_bindings);
        log_debug("port-state-manager", "StateMachine: bind/full discovery/unbind/recover action finished");
    }
    catch (std::exception e) {
        log_error("port-state-manager",
                  "StateMachine: bind/full discovery/unbind action failed: " << e.what());
        return false;
    }
    try {
        m_device_uuid = m_worker->get_device_uuid_by_dsp_port(m_port_uuid);
    }
    catch (std::exception e) {
        log_debug("port-state-manager",
                  "StateMachine: no drive/processor for device: " << e.what());
    }
    return true;
}


bool PortStateManager::action_bindings_recover(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: bindings recovery action started ...");
        auto bindings = m_worker->get_bindings_on_port(m_port_uuid);
        // recover bindings
        log_debug("port-state-manager", "StateMachine: Restoring bindings...");
        for (const auto& binding : bindings) {
            m_worker->attach_endpoint_to_zone(m_switch_uuid, std::get<1>(binding), std::get<0>(binding));
            log_debug("port-state-manager", "StateMachine: Binding to be recovered found!");
        }
        log_debug("port-state-manager", "StateMachine: bindings recovery action finished");
    }
    catch (std::exception e) {
        log_error("port-state-manager",
                  "StateMachine: bindings recovery action failed: " << e.what());
        return false;
    }
    return true;
}


bool PortStateManager::action_unbind_nonmgmt(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: unbind non-mgmt host action started ...");
        m_worker->unbind_port(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: unbind non-mgmt host action finished");
    }
    catch (std::exception e) {
        log_error("port-state-manager",
                  "StateMachine: unbind non-mgmt host action failed: " << e.what());
        return false;
    }
    return true;
}


bool PortStateManager::action_unbind_nonmgmt_ib_discovery(const PortTransition&) {
    try {
        log_debug("port-state-manager", "StateMachine: unbind non-mgmt && ib discovery action started ...");
        log_debug("port-state-manager", "StateMachine: Locking port...");
        m_worker->lock_port(m_port_uuid);
        log_debug("port-state-manager", "StateMachine: Unbinding from non-mgmt host...");
        m_worker->unbind_port(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: Binding...");
        uint8_t bridge_id = m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        log_debug("port-state-manager", "StateMachine: Discovering...");
        m_worker->ib_discovery(m_switch_uuid, m_port_uuid, bridge_id, m_device_uuid);
        log_debug("port-state-manager", "StateMachine: Unbinding...");
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug("port-state-manager", "StateMachine: Unlocking port...");
        m_worker->unlock_port(m_port_uuid);
        log_debug("port-state-manager", "StateMachine: unbind non-mgmt && ib discovery action finished");
    }
    catch (std::exception e) {
        log_error("port-state-manager",
                  "StateMachine: unbind non-mgmt && ib discovery action failed: " << e.what());
        return false;
    }
    return true;
}