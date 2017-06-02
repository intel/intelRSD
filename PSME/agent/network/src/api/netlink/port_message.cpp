/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file port_message.cpp
 *
 * @brief Port netlink message class
 *
 * */

#include "api/netlink/port_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <netlink/msg.h>
#include <net/if.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

PortMessage::PortMessage(const string& ifname) : LinkMessage{ifname} {
    /* set-up Netlink attributes */
    set_nlhdr_flags(NLM_F_REQUEST);
    set_nlhdr_type(RTM_GETLINK);

    /* set-up Link attributes */
    set_family(AF_PACKET);
}

PortMessage::~PortMessage() {}

void PortMessage::prepare_link_message(struct nl_msg* msg) {
    /* validate interface */
    if (!iface_exists()) {
        throw NlExceptionInvalidIfName(get_ifname());
    }

    /* put iface name into the message */
    if (0 > nla_put(msg, IFLA_IFNAME, int((get_ifname().length())),
        get_ifname().c_str())) {
        throw NlException("Cannot put iface name to the message");
    }

    /* insert TEXT_FILTER_VF mask */
    if (get_filter_mask() &&
        (0 > nla_put_u32(msg, IFLA_EXT_MASK, get_filter_mask()))) {
        throw NlException("Cannot put filter mask to the message");
    }
}

void PortMessage::process_link_message(struct nl_msg* msg, const struct ifinfomsg* ifm) {
    struct nlattr* attrs[IFLA_MAX + 1];
    char ifname[IF_NAMESIZE]{};

    /* parse nla attributes */
    int err = nlmsg_parse(nlmsg_hdr(msg), sizeof(struct ifinfomsg), attrs, IFLA_MAX, nullptr);
    if (0 != err) {
        throw NlException("Failed to parse Netlink route response");
    }

    /* get interface name */
    if (nullptr == if_indextoname(uint32_t(ifm->ifi_index), ifname)) {
        throw NlException("Failed to get interface index");
    }

    if (attrs[IFLA_MASTER]) {

        /* get master port ifindex */
        uint32_t master_index = nla_get_u32(attrs[IFLA_MASTER]);

        /* get master name */
        if (nullptr == if_indextoname(master_index, ifname)) {
            throw NlException("Failed to get master name");
        }
        m_master = ifname;
        m_member = true;
    }
}
