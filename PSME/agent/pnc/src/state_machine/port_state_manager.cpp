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
using PT = PortTransition;
using PSM = PortStateManager;

/*
 * How this state machine works:
 * - At the beginning there are two events sent to initialize state machine. First event gets the binding state. Port
 *   may be either bound to the management partition, other partition or unbound (in that case it is instantly bound
 *   to the host).
 * - Second event sets the link status. If it is up - some discovery may be done (full or out-of-band, depending on
 *   on the partition the port is bound to).
 * - If the port could have been bound to the management partition it is fully discovered and left in the Standby state.
 *   In this state we cannot monitor the link state (port has to be bound somewhere for it to be checked). The same
 *   happens in the NotPresent state - port is not bound, so the only possibility for the state change is getting one
 *   of the hotswap events.
 * - If the port is bound to any partition - it's link status may be checked. So we do not have to rely on hotswap
 *   events, link status is enough. Each time link goes down we remove data, and when it goes up - we do out-of-band
 *   discovery.
 * - In case port was unbound from the partition it was 'blocking' its discovery - it is instantly in-band discovered
 */

PortStateManager::PortStateManager(PortStateWorkerPtr worker, const std::string& id, const std::string& switch_uuid,
        const std::string& port_uuid):
    m_worker(worker),
    sm(id, PS::Initial, PortStateMachine::TransitionTable( {
        // First event initialization transitions
        PT{ PS::Initial,         PE::IsBoundToHost,    PS::InitBoundHost                                              },
        PT{ PS::Initial,         PE::IsBoundToOther,   PS::InitBoundOther                                             },
        PT{ PS::Initial,         PE::IsUnbound,        PS::InitBoundHost,   wrap(this, &PSM::action_bind)             },
        // Second event initialization transitions
        PT{ PS::InitBoundHost,   PE::DevicePresent,    PS::Standby,         wrap(this, &PSM::action_full_unbind)      },
        PT{ PS::InitBoundHost,   PE::DeviceNotPresent, PS::NotPresent,      wrap(this, &PSM::action_unbind)           },
        PT{ PS::InitBoundOther,  PE::DevicePresent,    PS::OobInUse,        wrap(this, &PSM::action_oob)              },
        PT{ PS::InitBoundOther,  PE::DeviceNotPresent, PS::NotPresentInUse                                            },
        // Unbind transitions
        PT{ PS::OobInUse,        PE::WasUnbound,       PS::Standby,         wrap(this, &PSM::action_ib_bind_unbind)   },
        PT{ PS::NotPresentInUse, PE::WasUnbound,       PS::NotPresent                                                 },
        // Hotswap transitions
        PT{ PS::OobInUse,        PE::HotUnplug,        PS::NotPresentInUse, wrap(this, &PSM::action_remove)           },
        PT{ PS::NotPresentInUse, PE::HotPlug,          PS::OobInUse,        wrap(this, &PSM::action_oob)              },
        PT{ PS::Standby,         PE::HotUnplug,        PS::NotPresent,      wrap(this, &PSM::action_remove)           },
        PT{ PS::NotPresent,      PE::HotPlug,          PS::Standby,         wrap(this, &PSM::action_full_bind_unbind) } }
    )),
    m_switch_uuid(switch_uuid),
    m_port_uuid(port_uuid) {}

PortStateManager::~PortStateManager() {}

void PortStateManager::init_binding(bool is_bound, bool is_bound_to_host) {
    log_debug(GET_LOGGER("port-state-manager"), "StateMachine: updating binding state: bound = " << is_bound
        << ", bound to host = " << is_bound_to_host);
    m_prev_bound = is_bound;
    PortEvent ev = is_bound ? (is_bound_to_host ? PE::IsBoundToHost : PE::IsBoundToOther) : PE::IsUnbound;
    sm.send_event(ev);
}

void PortStateManager::init_presence(bool is_present) {
    log_debug(GET_LOGGER("port-state-manager"), "StateMachine: updating device presence: dev present = " << is_present);
    m_prev_present = is_present;
    PortEvent ev = is_present ? PE::DevicePresent : PE::DeviceNotPresent;
    sm.send_event(ev);
}

void PortStateManager::update(bool is_present, bool is_bound, bool is_bound_to_host, bool is_being_erased) {

    log_debug(GET_LOGGER("port-state-manager"),
        "State machine - current state = " << sm.get_current_state().to_string());
    // if we are still in initial state - try initializing
    if (!sm.is_initialized()) {
        init_binding(is_bound, is_bound_to_host);
        init_presence(is_present);
    }
    // in other cases - just process the data
    else {
        generate_events(is_present, is_bound, is_being_erased);
    }
}

void PortStateManager::generate_events(bool is_present, bool is_bound, bool is_being_erased) {

    log_debug(GET_LOGGER("port-state-manager"), "StateMachine: updating states: device present = " << is_present
        << ", is bound = " << is_bound << ", is being erased = " << is_being_erased);

    // while drive being erased, port may be unbound -> we need to account for it
    bool is_used = is_bound || is_being_erased;
    if (!is_used && m_prev_bound) {
        sm.send_event(PE::WasUnbound);
    }
    else if (m_prev_present != is_present) {
        sm.send_event(is_present ? PE::HotPlug : PE::HotUnplug);
    }

    m_prev_present = is_present;
    m_prev_bound = is_used;
}

bool PortStateManager::action_oob(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: oob discovery action started ...");
        m_device_uuid = m_worker->oob_discovery(m_switch_uuid, m_port_uuid);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: oob discovery action finished");
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"), "StateMachine: oob discovery action failed: " << e.what());
        return false;
    }
    return true;
}

bool PortStateManager::action_remove(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: remove action started ...");
        m_worker->remove(m_switch_uuid, m_port_uuid);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: remove action finished");
        m_device_uuid = std::string{};
        return true;
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"), "StateMachine: remove action failed: " << e.what());
        return false;
    }
}

bool PortStateManager::action_ib_bind_unbind(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: bind/ib discovery/unbind action started ...");
        uint8_t bridge_id = m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        m_worker->ib_discovery(m_switch_uuid, m_port_uuid, bridge_id, m_device_uuid);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: bind/ib discovery/unbind action finished");
        return true;
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"),
            "StateMachine: bind/ib discovery/unbind action failed: " << e.what());
        return false;
    }
}

bool PortStateManager::action_bind(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: bind action started ...");
        m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: bind action finished");
        return true;
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"), "StateMachine: bind action failed: " << e.what());
        return false;
    }
}

bool PortStateManager::action_unbind(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: unbind action started ...");
        uint8_t bridge_id = m_worker->get_bridge_id(m_switch_uuid, m_port_uuid);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: unbind action finished");
        return true;
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"), "StateMachine: unbind action failed: " << e.what());
        return false;
    }
}

bool PortStateManager::action_full_unbind(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: full discovery/unbind action started ...");
        uint8_t bridge_id = m_worker->get_bridge_id(m_switch_uuid, m_port_uuid);
        m_worker->full_discovery(m_switch_uuid, m_port_uuid, bridge_id);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: full discovery/unbind action finished");
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"),
            "StateMachine: full discovery/unbind action failed: " << e.what());
        return false;
    }
    try {
        m_device_uuid = m_worker->get_drive_by_dsp_port(m_port_uuid);
    }
    catch (std::exception e) {
        log_debug(GET_LOGGER("port-state-manager"),
            "StateMachine: no drive for device: " << e.what());
    }
    return true;
}

bool PortStateManager::action_full_bind_unbind(const PortTransition&) {
    try {
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: bind/full discovery/unbind action started ...");
        uint8_t bridge_id = m_worker->bind_to_host(m_switch_uuid, m_port_uuid);
        m_worker->full_discovery(m_switch_uuid, m_port_uuid, bridge_id);
        m_worker->unbind_from_host(m_switch_uuid, bridge_id);
        log_debug(GET_LOGGER("port-state-manager"), "StateMachine: bind/full discovery/unbind action finished");
    }
    catch (std::exception e) {
        log_error(GET_LOGGER("port-state-manager"),
            "StateMachine: bind/full discovery/unbind action failed: " << e.what());
        return false;
    }
    try {
        m_device_uuid = m_worker->get_drive_by_dsp_port(m_port_uuid);
    }
    catch (std::exception e) {
        log_debug(GET_LOGGER("port-state-manager"),
            "StateMachine: no drive for device: " << e.what());
    }
    return true;
}
