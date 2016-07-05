/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file switch_port_info.cpp
 *
 * @brief Switch port parameters
 * */

#include "netlink/socket.hpp"
#include "netlink/message.hpp"
#include "netlink/utils.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "api/netlink/vlan_message.hpp"
#include "api/netlink/port_message.hpp"
#include "api/netlink/port_attribute_message.hpp"
#include "api/netlink/add_lag_port_message.hpp"
#include "api/netlink/del_lag_port_message.hpp"
#include "api/netlink/ethtool.hpp"
#include "api/netlink/sysfs.hpp"
#include "network_config.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "api/lldp/client.hpp"
#include "api/lldp/port_desc_tlv.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"

#include <sstream>
#include <net/if.h>

using namespace netlink_base;
using namespace agent::network;
using namespace agent::network::api::netlink;
using namespace agent::network::hw::fm10000;

const char SwitchPortInfo::CPU_PORT[] = "sw0p0";

SwitchPortInfo::SwitchPortInfo(const PortIdentifier& port) :
    m_port_identifier{port} {
    set_is_present(0 != if_nametoindex(port.c_str()));
    set_identifier(port);
}

SwitchPortInfo::~SwitchPortInfo() {}

void SwitchPortInfo::get_switch_port_link_state() {
    Socket socket{};
    Message message{get_port_identifier()};

    /* connect to the netlink */
    socket.connect();
    message.set_type(RTM_GETLINK);
    message.set_flags(NLM_F_REQUEST);
    socket.send_rtnl_message(message);
    socket.receive_link_state(message);
    set_link_state(message.get_link_state());
    set_operational_state(message.get_operational_state());
}

void SwitchPortInfo::set_switch_port_link_state() {
    Socket socket{};
    Message message{get_port_identifier()};

    if (State::Unknown == get_link_state_enum()) {
        THROW(agent_framework::exceptions::Fm10000Error, "network",
              "unknown port state");
    }

    socket.connect();
    message.set_type(RTM_SETLINK);
    message.set_flags(NLM_F_REPLACE|NLM_F_REQUEST|NLM_F_ACK);
    message.set_link_state(get_link_state_enum());
    socket.send_rtnl_message(message);
}

void SwitchPortInfo::set_switch_port_attr(SwAttr swattr, SwAttrValue value) {
    Socket socket{};
    PortAttributeMessage message{get_port_identifier()};

    socket.connect();
    message.set_attribute(swattr, value);
    socket.send_message(message);
    socket.receive_message(message);
}

uint32_t SwitchPortInfo::get_switch_port_speed() {
    Ethtool ethtool(get_port_identifier());
    return ethtool.get_speed();
}

void SwitchPortInfo::set_switch_port_speed(uint32_t speed) {
    Ethtool ethtool(get_port_identifier());
    return ethtool.set_speed(speed);
}

void SwitchPortInfo::get_switch_port_attribute(PortAttributeType attr,
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
            value.set(get_switch_port_max_frame_size());
            break;
        case AUTOSENSE:
            value.set(get_switch_port_autoneg());
            break;
        case VLANLIST:
            get_switch_vlan_list();
            break;
        case MACADDRESS:
            value.set(get_switch_port_mac_address());
            break;
        case DEFAULTVID:
            get_switch_port_default_vid();
            value.set(uint32_t(get_default_vlan_id()));
            break;
        case NEIGHBORINFO:
            value.set(get_neigh_port_identifier());
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
        case IPV4ADDRESS:
        case IPV6ADDRESS:
        case NEIGHBORMAC:
        case VLANENABLE:
            throw std::runtime_error("unsupported port attribute");
        default:
            throw std::runtime_error("unknown port attribute");
        }
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Cannot get port configuration on port ")
              + get_port_identifier() + ". Error message: " + error.what());
    }
}

void SwitchPortInfo::set_switch_port_attribute(PortAttributeType attr,
                                               const PortAttributeValue& value
                                               ) {
    try {
        switch (attr) {
            case LINKSPEEDMBPS:
                set_switch_port_speed(value.get_number());
                break;
            case ADMINISTRATIVESTATE:
                set_link_state(value.get_string());
                set_switch_port_link_state();
                break;
            case FRAMESIZE:
                set_switch_port_attr(IFLA_ATTR_MAX_FRAME_SIZE, value.get_number());
                break;
            case AUTOSENSE:
                set_switch_port_attr(IFLA_ATTR_AUTONEG, (value.get_bool() ? 1 : 0));
                break;
            case DEFAULTVID:
                set_switch_port_default_vid(VlanId(uint32_t(value)));
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
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Cannot set port attribute on port ")
              + get_port_identifier() + ". Error message: " + error.what());
    }
}

uint32_t SwitchPortInfo::get_switch_port_max_frame_size() {
    SysFs sysfs(get_port_identifier());
    return sysfs.get_max_frame_size();
}

bool SwitchPortInfo::get_switch_port_autoneg() {
    SysFs sysfs(get_port_identifier());
    return (0 == sysfs.get_autoneg() ? false : true);
}

void SwitchPortInfo::get_switch_vlan_list() {
    VlanList vlist{};
    Socket socket;
    socket.connect();
    InfoVlanPortMessage vlan_msg;
    socket.send_message(vlan_msg);
    socket.receive_message(vlan_msg);
    for (const auto& vlan_info : vlan_msg.get_vlan_list(get_port_identifier())) {
        vlist.push_back(std::to_string(int(vlan_info.get_vlan_id())));
    }
    set_vlan_list(vlist);
}

void SwitchPortInfo::get_switch_port_default_vid() {
    Socket socket;
    socket.connect();
    InfoVlanPortMessage vlan_msg;
    socket.send_message(vlan_msg);
    socket.receive_message(vlan_msg);
    for (const auto& vlan_info : vlan_msg.get_vlan_list(get_port_identifier())) {
        if (vlan_info.get_pvid()) {
            set_default_vlan_id(vlan_info.get_vlan_id());
            return;
        }
    }
}

void SwitchPortInfo::set_switch_port_default_vid(VlanId pvid) {
    Socket socket;
    socket.connect();
    bool valid_pvid{false};
    InfoVlanPortMessage vlan_msg;
    socket.send_message(vlan_msg);
    socket.receive_message(vlan_msg);
    /* check if given pvid is valid */
    for (const auto& vlan_info : vlan_msg.get_vlan_list(get_port_identifier())) {
        if (pvid == vlan_info.get_vlan_id()) {
            valid_pvid = true;
        }
    }
    if (valid_pvid) {
        /* change PVID on the port */
        AddVlanPortMessage add_vlan_msg(get_port_identifier(), pvid, true);
        add_vlan_msg.set_pvid(true);
        socket.send_message(add_vlan_msg);
        socket.receive_message(add_vlan_msg);
    }
    else {
        THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
              "PVID " + std::to_string(int(pvid)) + " not valid");
    }
}

string SwitchPortInfo::get_switch_port_mac_address() {
    return utils::get_neighbor_mac_address(get_port_identifier());
}

void SwitchPortInfo::add_member(const PortIdentifier& port, const PortIdentifier& member)
{
    Socket socket;

    try {
        socket.connect();

        AddLagPortMessage lag_msg(port, member);
        socket.send_message(lag_msg);
        socket.receive_message(lag_msg);
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              "Failed adding member " + member + " to a LAG port " + port + ". Error message: " + error.what());
    }
}

void SwitchPortInfo::remove_member(const PortIdentifier& member)
{
    Socket socket;

    try {
        socket.connect();

        DelLagPortMessage lag_msg(member);
        socket.send_message(lag_msg);
        socket.receive_message(lag_msg);
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              "Failed deleting member from a LAG port " + member + ". Error message: " + error.what());
    }
}

bool SwitchPortInfo::vlan_exists(VlanId vid) const {
    Socket socket;

    try {
        socket.connect();
        InfoVlanPortMessage msg;
        socket.send_message(msg);
        socket.receive_message(msg);
        for (const auto& vlan_info : msg.get_vlan_list(get_port_identifier())) {
            if (vlan_info.get_vlan_id() == vid) {
                return true;
            }
        }
    }
    catch (std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Cannot check vlan existance on port ")
              + get_port_identifier() + ". Error message: " + error.what());
    }
    return false;
}

SwitchPortInfo::PortList SwitchPortInfo::get_port_members() const {
    SysFs sysfs{};
    Socket socket{};
    PortList members{};
    try {
        socket.connect();
        for (const auto& port : sysfs.get_port_list()) {
            PortMessage port_msg{port};
            socket.send_message(port_msg);
            socket.receive_message(port_msg);
            if (port_msg.is_member()
                && port_msg.get_master() == get_port_identifier()) {
                members.push_back(port);
            }
        }
    }
    catch(const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Failed to get port members. Error message: ") + error.what());
    }
    return members;
}

bool SwitchPortInfo::is_member() const {
    Socket socket{};
    PortMessage port_msg{get_port_identifier()};
    try {
        socket.connect();
        socket.send_message(port_msg);
        socket.receive_message(port_msg);
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Failed to get port info :") + get_port_identifier() + ". Error message: " + error.what());
    }
    return port_msg.is_member();
}

SwitchPortInfo::PortIdentifier SwitchPortInfo::get_master_port() const {
    Socket socket{};
    PortMessage port_msg{get_port_identifier()};
    try {
        socket.connect();
        socket.send_message(port_msg);
        socket.receive_message(port_msg);
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Failed to get port info") + get_port_identifier() + ". Error message: " + error.what());
    }
    if (!port_msg.is_member()) {
        THROW(agent_framework::exceptions::InvalidParameters, "fm10000",
              std::string("Port is not a memeber :") + get_port_identifier());
    }
    return PortIdentifier{port_msg.get_master()};
}

SwitchPortInfo::VlanInfoList SwitchPortInfo::get_vlans() const {
    Socket socket{};
    VlanInfoList vlans{};
    InfoVlanPortMessage vlan_msg{};
    try {
        socket.connect();
        socket.send_message(vlan_msg);
        socket.receive_message(vlan_msg);
    }
    catch (const std::runtime_error& error) {
        THROW(agent_framework::exceptions::Fm10000Error, "fm10000",
              std::string("Failed to get vlan info on a port. Error message: ") + error.what());
    }
    for (const auto& vlan_info : vlan_msg.get_vlan_list(get_port_identifier())) {
        vlans.emplace_back(uint32_t(vlan_info.get_vlan_id()), vlan_info.get_tagged());
    }
    return vlans;
}

string SwitchPortInfo::get_neigh_port_identifier() const {
    string neighbor_port{};
    api::lldp::Client lldp_client{get_port_identifier()};
    try {
        /* get neighbor port information */
        auto tlv = lldp_client.get_neighbor_tlv<api::lldp::PortDescTlv>();
        neighbor_port = tlv.get_ifname();
    }
    catch (const api::lldp::exception::ApiError& e) {
        if (api::lldp::exception::ErrCode::NO_INFO == e.code()) {
            log_debug(GET_LOGGER("network-agent"),
                "Neighbor information is not available on port "
                + get_port_identifier());
            /* check LLDP admin mode */
            auto lldp_mode = lldp_client.get_mode();
            if (api::lldp::Client::Mode::DISABLED == lldp_mode) {
                log_warning(GET_LOGGER("network-agent"),
                    "Enabling LLDP on port " + get_port_identifier());
                /* try to enable LLDP on given interface */
                lldp_client.set_mode(api::lldp::Client::Mode::RXTX);
            }
        }
        else if (api::lldp::exception::ErrCode::NO_CONNECTION == e.code()) {
            log_warning(GET_LOGGER("network-agent"),
                string("LLDP agent connection is closed ") + e.what());
        }
        else {
            /* throw an API exception */
            throw;
        }
    }
    return neighbor_port;
}
