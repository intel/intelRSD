/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file switch_port_info_impl.cpp
 *
 * @brief Switch port parameters
 * */

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

SwitchPortInfoImpl::SwitchPortInfoImpl(const PortIdentifier& port) :
    m_port_identifier{port}, m_ifindex(if_nametoindex(port.c_str())) {
    set_is_present(0 != m_ifindex);
    set_identifier(port);
}

SwitchPortInfoImpl::~SwitchPortInfoImpl() {}

void SwitchPortInfoImpl::get_switch_port_link_state() {
    bool admin_up =
        EosAgent::get_instance()->is_admin_enabled(get_port_identifier());
    bool operational_up =
        EosAgent::get_instance()->is_operational_up(get_port_identifier());
    set_link_state(admin_up ? SwitchPortInfo::State::Up :
                   SwitchPortInfo::State::Down );
    set_operational_state(operational_up ? SwitchPortInfo::State::Up :
                          SwitchPortInfo::State::Down);
}

void SwitchPortInfoImpl::set_switch_port_link_state() {
    if (State::Unknown == get_link_state_enum()) {
        THROW(agent_framework::exceptions::NetworkError, "network",
              "Unknown port state");
    }
    EosAgent::get_instance()->set_admin_enable(get_port_identifier(),
                                               get_link_state_enum() == State::Up ? true : false);
}

void SwitchPortInfoImpl::set_switch_port_attr(SwAttr, SwAttrValue) {
}

uint32_t SwitchPortInfoImpl::get_switch_port_speed() {
    return EosAgent::get_instance()->get_speed(get_port_identifier());
}

void SwitchPortInfoImpl::set_switch_port_speed(uint32_t) {
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
        case MACADDRESS: //TODO
            break;
        case IPV4ADDRESS: // TODO
            break;
        case IPV4SUBNETMASK:
            value.set(m_ipv4_subnet_mask);
            break;
        case DEFAULTVID:
            get_switch_port_default_vid();
            value.set(uint32_t(get_default_vlan_id()));
            break;
        case NEIGHBORINFO:
            //value.set(get_neigh_port_identifier());
           // break;
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

uint32_t SwitchPortInfoImpl::get_switch_port_max_frame_size() {
    return 0;
}

bool SwitchPortInfoImpl::get_switch_port_autoneg() {
    //TODO
    return false;
}

void SwitchPortInfoImpl::set_switch_port_autoneg(bool) {
}

SwitchPortInfo::Duplex SwitchPortInfoImpl::get_switch_port_duplex() {
    // TODO
    return SwitchPortInfo::Duplex::UNKNOWN;
}

void SwitchPortInfoImpl::get_switch_vlan_list() {
}

void SwitchPortInfoImpl::get_switch_port_default_vid() {
    string port = get_port_identifier();
    uint16_t default_vlan = EosAgent::get_instance()->get_default_vlan(port);
    if (0 != default_vlan) {
        set_default_vlan_id(default_vlan);
    }
}

void SwitchPortInfoImpl::add_member(const PortIdentifier&,
                                    const PortIdentifier&)
{
}

void SwitchPortInfoImpl::remove_member(const PortIdentifier&)
{
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

SwitchPortInfo::VlanInfoList SwitchPortInfoImpl::get_vlans() const {
    VlanInfoList vlans{};
    string port = get_port_identifier();
    for (uint32_t vid : EosAgent::get_instance()->get_tagged_vlans(port)) {
        vlans.emplace_back(vid, true);
    }
    uint32_t default_vlan = EosAgent::get_instance()->get_default_vlan(port);
    if (0 != default_vlan) {
        vlans.emplace_back(default_vlan, false);
    }
    return vlans;
}
