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
 * @file message.cpp
 *
 * @brief Netlink message
 * */

#include "netlink/message.hpp"

#include <net/if.h>
#include <linux/if_bridge.h>


using namespace netlink_base;

Message::Message() {}

Message::Message(const IfName& ifname) : m_ifname{ifname} {
}

Message::~Message() {}

bool Message::prepare_getneigh_msg(Message::Pointer &msg) {
    struct ndmsg ndm{};
    m_ifindex = (int) if_nametoindex(m_ifname.c_str());

    ndm.ndm_family = PF_BRIDGE;
    ndm.ndm_ifindex = m_ifindex;
    if (0 > nlmsg_append(msg.get(), &ndm, sizeof(ndm), NLMSG_ALIGNTO)) {
        return false;
    }

    return true;
}

Message::Pointer Message::prepare_rtnl_msg() {
    struct ifinfomsg ifi{};
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));

    if (!msg) {
        return nullptr;
    }

    if (RTM_GETNEIGH == m_type) {
        if (prepare_getneigh_msg(msg)) {
            return msg;
        } else {
            return nullptr;
        }
    }

    ifi.ifi_family = AF_UNSPEC;
    if (State::Unknown != m_link_state) {
        /* prepare message for SET operation */
        ifi.ifi_type = ARPHRD_ETHER;
        ifi.ifi_change |= IFF_UP;
        if (State::Up == m_link_state) {
            ifi.ifi_flags = IFF_UP | IFF_RUNNING;
        }
        else {
            ifi.ifi_flags &= static_cast<unsigned int>(~IFF_UP);
        }
    }
    /* Appending the ndm parameter details to netlink message msg */
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }

    if (0 > nla_put(msg.get(), IFLA_IFNAME, int((m_ifname.length())), m_ifname.c_str())) {
        return nullptr;
    }

    log_debug(GET_LOGGER("netlink"), "netlink message created for device " + m_ifname);
    return msg;
}

void Message::parse_attr(struct rtattr *tb[], unsigned int max,
                         struct rtattr *rta, int len)
{
    while (RTA_OK(rta, len)) {
        unsigned short type = rta->rta_type;
        if ((type <= max) && (nullptr == tb[type])) {
            tb[type] = rta;
        }
        rta = RTA_NEXT(rta,len);
    }
}

#define RTA(r) ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))

int Message::read_neighbour(struct nl_msg *nlmsg, void *arg) {
    struct nlmsghdr *hdr;
    struct rtattr *tb[NDA_MAX + 1]{};
    struct ndmsg *data;
    Message *message = (Message*) arg;

    if (nullptr == nlmsg) {
        return NL_OK;
    }

    if (nullptr == message) {
        return NL_OK;
    }

    hdr = nlmsg_hdr(nlmsg);
    data = static_cast<struct ndmsg*> (nlmsg_data(hdr));

    if (hdr->nlmsg_type == NLMSG_DONE) {
        message->set_last();
        return NL_OK;
    }

    if (hdr->nlmsg_type != RTM_NEWNEIGH) {
        return NL_OK;
    }

    if (data->ndm_family != AF_BRIDGE) {
        return NL_OK;
    }

    if (message->get_ifindex() != data->ndm_ifindex) {
        return NL_OK;
    }

    parse_attr(tb, NDA_MAX, RTA(data),
               (int) hdr->nlmsg_len - (int) NLMSG_LENGTH(sizeof(*data)));

    if (tb[NDA_LLADDR] && (0 == message->get_address().length())) {
        unsigned char *addr = static_cast<unsigned char *>
            (RTA_DATA(tb[NDA_LLADDR]));
        std::stringstream ss;
        const char *sep = "";
        ss.fill('0');
        for (unsigned i = 0; i < ETH_ALEN; ++i) {
            ss << sep;
            sep = ":";
            ss << std::uppercase << std::setw(2) << std::hex <<
                static_cast<unsigned int>(addr[i]);
        }

        message->set_address(ss.str());
    }

    return NL_OK;
}

