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
 * @file vlan_message.cpp
 *
 * @brief Switch vlan configuration
 * */

#include "api/netlink/vlan_message.hpp"

#include <cstdio>
#include <sstream>
#include <memory>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/if_bridge.h>

using namespace netlink_base;
using namespace agent::network::api::netlink;

VlanPortMessage::VlanPortMessage(const IfName& port,
                                 Vlan vlan, bool tagged) :
    Message{port}, m_vlan_id(vlan), m_tagged{tagged} {
}

VlanPortMessage::~VlanPortMessage() { }

AddVlanPortMessage::AddVlanPortMessage(const IfName& port,
                                       Vlan vlan, bool tagged) :
    VlanPortMessage(port, vlan, tagged) {
    set_flags(NLM_F_REQUEST | NLM_F_ACK);
    set_type(RTM_SETLINK);
}

AddVlanPortMessage::~AddVlanPortMessage() { }

Message::Pointer AddVlanPortMessage::prepare_netlink_msg() const {
    struct ifinfomsg ifi{};
    struct bridge_vlan_info info{};
    struct nlattr *vlanspec = nullptr;

    /* Allocate the vlan netlink message */
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));
    if (!msg) {
        return nullptr;
    }

    /* Set bridge interface family */
    ifi.ifi_family = PF_BRIDGE;

    /* Set interface name */
    ifi.ifi_index = int(if_nametoindex(m_ifname.c_str()));
    if (0 == ifi.ifi_index) {
        return nullptr;
    }
    /* Appending iface parameter details to netlink message msg */
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }
    /* Setup vlan attributes */
    info.vid = get_vlan_id();
    vlanspec = nla_nest_start(msg.get(), IFLA_AF_SPEC);
    if (!vlanspec) {
        return nullptr;
    }
    if (0 > nla_put_u16(msg.get(), IFLA_BRIDGE_FLAGS, BRIDGE_FLAGS_SELF)) {
        return nullptr;
    }
    if (!get_tagged()) {
        /* set tagged flag */
        info.flags |= BRIDGE_VLAN_INFO_UNTAGGED;
    }
    if (get_pvid()) {
        /* set pvid flag */
        info.flags |= BRIDGE_VLAN_INFO_PVID;
    }
    if (0 > nla_put(msg.get(), IFLA_BRIDGE_VLAN_INFO, sizeof(info),
        &info)) {
        return nullptr;
    }
    nla_nest_end(msg.get(), vlanspec);

    return msg;
}

DelVlanPortMessage::DelVlanPortMessage(const IfName& port, Vlan vlan) :
    AddVlanPortMessage(port, vlan, true) {
    set_type(RTM_DELLINK);
    set_flags(0);
}

DelVlanPortMessage::~DelVlanPortMessage() { }

InfoVlanPortMessage::InfoVlanPortMessage() :
    VlanPortMessage({}, 0, false) {
    set_flags(NLM_F_REQUEST | NLM_F_DUMP);
    set_type(RTM_GETLINK);
}

Message::Pointer InfoVlanPortMessage::prepare_netlink_msg() const {
    struct ifinfomsg ifi{};

    /* Allocate the vlan netlink message */
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));
    if (!msg) {
        return nullptr;
    }

    /* Set bridge interface family */
    ifi.ifi_family = PF_BRIDGE;

    /* Set interface info */
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }
    if (0 > nla_put_u32(msg.get(), IFLA_EXT_MASK, RTEXT_FILTER_BRVLAN)) {
        return nullptr;
    }

    return msg;
}

void InfoVlanPortMessage::parse_netlink_hdr(NetlinkHeader& nlhdr) {
    struct ifinfomsg* ifm = static_cast<struct ifinfomsg*>(nlhdr.get_data());
    int len = int(nlhdr.get_len());
    struct nlattr* nl_attr_table[IFLA_MAX + 1];
    char ifname[IF_NAMESIZE]{};
    struct bridge_vlan_info* vinfo = nullptr;
    VlanInfoList vlan_list{};

    if (RTM_NEWLINK != nlhdr.get_type()) {
        throw std::runtime_error("Not RTM_NEWLINK message");
    }
    len -= int(nlmsg_size(sizeof(*ifm)));
    if (0 > len) {
        throw std::runtime_error("Wrong netlink message length");
    }
    if (AF_BRIDGE != ifm->ifi_family) {
        return;
    }

    nla_parse(nl_attr_table, IFLA_MAX, (struct nlattr*)IFLA_RTA(ifm),
              len, nullptr);

    /* get interface name */
    if (nullptr == if_indextoname(uint32_t(ifm->ifi_index), ifname)) {
        throw std::runtime_error("Failed to get interface index");
    }

    /* if AF_SPEC isn't there, vlan table is not preset for this port */
    if (!nl_attr_table[IFLA_AF_SPEC]) {
        m_vlan_port_info[ IfName(ifname) ] = { };
        return;
    }
    else {
        vlan_list.clear();
        struct nlattr* list = nl_attr_table[IFLA_AF_SPEC];

        int rem = nla_len(list);
        struct nlattr* nl_attr = (struct nlattr*)nla_data(list);
        for(; nla_ok(nl_attr, rem); nl_attr = nla_next(nl_attr, &(rem))) {
            if (IFLA_BRIDGE_VLAN_INFO != nl_attr->nla_type) {
                continue;
            }
            vinfo = (struct bridge_vlan_info*)nla_data(nl_attr);
            bool tag = !(vinfo->flags & BRIDGE_VLAN_INFO_UNTAGGED);
            bool pvid = (vinfo->flags & BRIDGE_VLAN_INFO_PVID);
            vlan_list.emplace_back(vinfo->vid, tag, pvid);
        }
        m_vlan_port_info[ IfName(ifname) ] = vlan_list;
    }
}

bool InfoVlanPortMessage::is_tagged(const IfName& port, Vlan vlan) {
    VlanPortInfoMap::iterator port_iter = m_vlan_port_info.find(port);
    if (m_vlan_port_info.end() == port_iter) {
        throw std::runtime_error("is_tagged(): port=" + port + " not found");
    }
    const VlanInfoList& vlan_list = port_iter->second;
    for (const auto& vlan_info : vlan_list) {
        if (vlan_info.get_vlan_id() == vlan)
            return vlan_info.get_tagged();
    }
    throw std::runtime_error("is_tagged(): pair vlan="
                             + std::to_string(int(vlan))
                             + " port=" + port + " not found");
}

const InfoVlanPortMessage::VlanInfoList&
InfoVlanPortMessage::get_vlan_list(const IfName& ifname) const {
    /* find the port in port vlan list */
    auto port_iter = m_vlan_port_info.find(ifname);
    if (m_vlan_port_info.end() == port_iter) {
        throw std::runtime_error("get_vlan_list():"
                                 + ifname + " port not found");
    }

    /* return port vlan list */
    return port_iter->second;
}
