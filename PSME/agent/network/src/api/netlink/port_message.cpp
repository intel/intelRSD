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
 * @file port_message.cpp
 *
 * @brief Switch port netlink message class
 *
 * */

/* Internal headers */
#include "api/netlink/port_message.hpp"

/* C/C++ standard headers */
#include <net/if.h>

using namespace netlink_base;
using namespace agent::network::api::netlink;

PortMessage::PortMessage(const IfName& ifname) : Message(ifname) {
    set_flags(NLM_F_REQUEST);
    set_type(RTM_GETLINK);
}

PortMessage::~PortMessage() {}

Message::Pointer PortMessage::prepare_netlink_msg() const {
    struct ifinfomsg ifi{};

    /* Allocate the netlink message */
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));
    if (!msg) {
        return nullptr;
    }

    /* Appending iface parameter details to netlink message msg */
    ifi.ifi_family = AF_PACKET;
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }

    /* add interface name into the message */
    if (0 > nla_put(msg.get(), IFLA_IFNAME, int(m_ifname.length()), m_ifname.c_str())) {
        return nullptr;
    }

    /* insert TEXT_FILTER_VF mask */
    if (0 > nla_put_u32(msg.get(), IFLA_EXT_MASK, RTEXT_FILTER_VF)) {
        return nullptr;
    }

    return msg;
}

void PortMessage::parse_netlink_hdr(NetlinkHeader& nlhdr) {
    struct ifinfomsg* ifm = static_cast<struct ifinfomsg*>(nlhdr.get_data());
    struct nlattr* nl_attr_table[IFLA_MAX + 1];
    int len = int(nlhdr.get_len());
    char ifname[IF_NAMESIZE]{};

    /* check netlink message type */
    if (RTM_NEWLINK != nlhdr.get_type() && RTM_DELLINK != nlhdr.get_type ()) {
        throw std::runtime_error("Incorrect netlink message type");
    }

    /* check netlink message length */
    len -= int(nlmsg_size(sizeof(*ifm)));
    if (0 > len) {
        throw std::runtime_error("Incorrect netlink message length");
    }

    /* check family info */
    if (AF_UNSPEC != ifm->ifi_family) {
        return;
    }

    /* parse nla attributes */
    nla_parse(nl_attr_table, IFLA_MAX, (struct nlattr*)IFLA_RTA(ifm),
              len, nullptr);

    /* get interface name */
    if (nullptr == if_indextoname(uint32_t(ifm->ifi_index), ifname)) {
        throw std::runtime_error("Failed to get interface index");
    }

    if (nl_attr_table[IFLA_MASTER]) {

        /* get master port ifindex */
        uint32_t master_index = nla_get_u32(nl_attr_table[IFLA_MASTER]);

        /* get master name */
        if (nullptr == if_indextoname(master_index, ifname)) {
            throw std::runtime_error("Failed to get master name");
        }
        m_master = ifname;
        m_member = true;
    }
}
