/*!
 * @brief Switch Port Info class implementation.
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
 * @file agent/network/src/hal/switch_port_info_impl.cpp
 */


#include "hal/switch_port_info_impl.hpp"
#include "network_config.hpp"
#include "loader/network_config.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "discovery/discovery_manager.hpp"
#include "hal/eos_sdk/eos_agent.hpp"

#include <sstream>
#include <net/if.h>

using namespace agent::network;
using namespace agent::network::hal;
using namespace agent::network::loader;
using namespace agent::network::discovery;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;


SwitchPortInfoImpl::SwitchPortInfoImpl(const PortIdentifier& port) :
    m_port_identifier{port}, m_ifindex(if_nametoindex(port.c_str())) {
    set_is_present(0 != m_ifindex);
    set_identifier(port);
}

SwitchPortInfoImpl::~SwitchPortInfoImpl() {}


SwitchPortInfo::Duplex SwitchPortInfoImpl::get_switch_port_duplex() {
    return SwitchPortInfo::Duplex::UNKNOWN;
}

void SwitchPortInfoImpl::get_switch_vlan_list() {
}

bool SwitchPortInfoImpl::vlan_exists(VlanId) const {
    return false;
}

SwitchPortInfo::PortList SwitchPortInfoImpl::get_port_members() const {
    PortList members{};
    return members;
}

bool SwitchPortInfoImpl::is_member() const {
    return false;
}

SwitchPortInfo::PortIdentifier SwitchPortInfoImpl::get_master_port() const {
    return PortIdentifier{};
}

void SwitchPortInfoImpl::get_switch_port_attribute(PortAttributeType attr,
                                                   PortAttributeValue& value) {
    try {
        switch (attr) {
            case LINKSPEEDMBPS:
                value.set(get_switch_port_speed());
                break;
            case ADMINISTRATIVESTATE:
                get_switch_port_link_state();
                value.set(get_link_state());
                break;
            case OPERATIONALSTATE:
                get_switch_port_link_state();
                value.set(get_operational_state());
                break;
            case PORTIDENTIFIER:
                value.set(get_identifier());
                break;
            case FRAMESIZE:
                break;
            case AUTOSENSE:
                break;
            case VLANLIST:
                get_switch_vlan_list();
                break;
            case MACADDRESS:
                break;
            case IPV4ADDRESS:
                break;
            case IPV4SUBNETMASK:
                value.set(m_ipv4_subnet_mask);
                break;
            case DEFAULTVID:
                get_switch_port_default_vid();
                value.set(uint32_t(get_default_vlan_id()));
                break;
            case PFC_ENABLED:
                break;
            case PFC_ENABLED_PRIORITIES:
                break;
            case LLDP_ENABLED:
                break;
            case DCBX_STATE:
                break;
            case NEIGHBORINFO:
            case STATUS:
            case PORTTYPE:
            case PORTMODE:
            case LINKTECHNOLOGY:
            case MAXSPEEDMBPS:
            case PORTWIDTH:
            case ISMANAGEMENTPORT:
            case LASTERRORCODE:
            case ERRORCLEARED:
            case LASTSTATECHANGETIME:
            case IPV6ADDRESS:
            case NEIGHBORMAC:
            case VLANENABLE:
                throw std::runtime_error("unsupported port attribute");
            default:
                throw std::runtime_error("unknown port attribute");
        }
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::NetworkError, "network-agent",
              std::string("Cannot get port configuration on port ")
              + get_port_identifier() + ". Error message: " + error.what());
    }
}

void SwitchPortInfoImpl::set_switch_port_attribute(PortAttributeType attr,
                                                   const PortAttributeValue& value) {
    try {
        switch (attr) {
            case LINKSPEEDMBPS:
                THROW(agent_framework::exceptions::UnsupportedField,
                      "network-agent", "Setting this attribute is not supported",
                      literals::EthernetSwitchPort::LINK_SPEED_MBPS, uint32_t(value));
                break;
            case ADMINISTRATIVESTATE:
                set_link_state(value.get_string());
                set_switch_port_link_state();
                break;
            case FRAMESIZE:
                THROW(agent_framework::exceptions::UnsupportedField,
                      "network-agent", "Setting this attribute is not supported",
                      literals::EthernetSwitchPort::FRAME_SIZE, uint32_t(value));
                break;
            case AUTOSENSE:
                THROW(agent_framework::exceptions::UnsupportedField,
                      "network-agent", "Setting this attribute is not supported",
                      literals::EthernetSwitchPort::AUTO_SENSE, uint32_t(value));
                break;
            case DEFAULTVID:
                THROW(agent_framework::exceptions::UnsupportedField,
                      "network-agent", "Setting this attribute is not supported",
                      literals::EthernetSwitchPort::DEFAULT_VLAN,
                      uint32_t(value));
                break;
            case PFC_ENABLED:
                break;
            case PFC_ENABLED_PRIORITIES:
                break;
            case LLDP_ENABLED:
                break;
            case DCBX_STATE:
                break;
            case STATUS:
            case PORTTYPE:
            case PORTMODE:
            case LINKTECHNOLOGY:
            case MAXSPEEDMBPS:
            case PORTWIDTH:
            case ISMANAGEMENTPORT:
            case LASTERRORCODE:
            case ERRORCLEARED:
            case LASTSTATECHANGETIME:
            case MACADDRESS:
            case IPV4ADDRESS:
            case IPV4SUBNETMASK:
            case IPV6ADDRESS:
            case NEIGHBORINFO:
            case NEIGHBORMAC:
            case VLANENABLE:
            case VLANLIST:
                throw std::runtime_error("unsupported port attribute");
            case OPERATIONALSTATE:
            case PORTIDENTIFIER:
                throw std::runtime_error("read-only port attribute");
            default:
                throw std::runtime_error("unknown port attribute");
        }
    }
    catch (std::runtime_error& error) {
        THROW(agent_framework::exceptions::NetworkError, "network-agent",
              std::string("Cannot set port attribute on port ")
              + get_port_identifier() + ". Error message: " + error.what() + ".");
    }
}

void SwitchPortInfoImpl::update_switch_port_lldp_enabled(bool current_lldp_state, bool new_lldp_state) {
    if (current_lldp_state == new_lldp_state) {
        return;
    }
    try {
        set_switch_port_lldp_enabled(new_lldp_state); // hardware
    }
    catch (json_rpc::JsonRpcException& e) {
        THROW(NetworkError, "network-agent", "eAPI exception on setting port QoS LLDP state: " +
                            std::string(e.what()));
    }
}

void SwitchPortInfoImpl::update_switch_port_dcbx_state(const DcbxState current_dcbx_state,
                                                       const DcbxState new_dcbx_state) {
    if (current_dcbx_state == new_dcbx_state) {
        return;
    }
    try {
        set_switch_port_dcbx_state(new_dcbx_state); // hardware
    }
    catch (json_rpc::JsonRpcException& e) {
        THROW(NetworkError, "network-agent", "eAPI exception on setting port QoS DCBX state: " +
                                             std::string(e.what()));
    }
}

void SwitchPortInfoImpl::update_switch_port_pfc_enabled(bool current_pfc_state, bool new_pfc_state) {

    if (current_pfc_state == new_pfc_state) {
        return;
    }

    try {
        set_switch_port_pfc_enabled(new_pfc_state); // hardware
    }
    catch (json_rpc::JsonRpcException& e) {
        THROW(NetworkError, "network-agent", "eAPI exception on setting port QoS PFC state: " +
                                             std::string(e.what()));
    }
}

std::tuple<std::vector<std::uint32_t>, std::vector<std::uint32_t>>
SwitchPortInfoImpl::get_priorities_to_set_to_remove(const PrioritiesList& requested,
                                                    const PrioritiesList& existing) {

    std::vector<std::uint32_t> to_set{};
    std::vector<std::uint32_t> to_remove{};
    // find priorities that are present and not requested
    for (const auto& existing_priority : existing) {
        if (std::find(requested.begin(), requested.end(), json::Json(existing_priority)) == requested.end()) {
            to_remove.push_back(existing_priority);
        }
    }
    // find priorities that are requested and not present
    for (const auto& desired_priority : requested) {
        if (std::find(existing.begin(), existing.end(), desired_priority) == existing.end()) {
            to_set.push_back(desired_priority);
        }
    }
    return std::make_tuple(to_set, to_remove);
}


void SwitchPortInfoImpl::update_switch_port_pfc_priorities(const PrioritiesList& current_pfc_priorities,
                                                           const PrioritiesList& new_pfc_priorities) {

    std::vector<std::uint32_t> priorities_to_remove{};
    std::vector<std::uint32_t> priorities_to_set{};
    std::tie(priorities_to_set, priorities_to_remove)
        = get_priorities_to_set_to_remove(new_pfc_priorities, current_pfc_priorities);
    if (priorities_to_set.empty() && priorities_to_remove.empty()) {
        return;
    }

    try {
        set_port_pfc_priorities(priorities_to_set, priorities_to_remove); // hardware
    }
    catch (json_rpc::JsonRpcException& e) {
        THROW(NetworkError, "network-agent", "eAPI exception on setting port QoS PFC priorities: " +
                                             std::string(e.what()));
    }
}
