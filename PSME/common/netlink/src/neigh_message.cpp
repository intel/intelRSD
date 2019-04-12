/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file neigh_message.cpp
 *
 * @brief Neighbor Netlink message
 * */

#include "netlink/neigh_message.hpp"
#include "netlink/nl_exception.hpp"

#include <netlink/msg.h>
#include <linux/if_ether.h>

#include <cstring>

using std::string;
using std::ostringstream;
using namespace netlink_base;

NeighMessage::NeighMessage(const string& ifname) : RtMessage{ifname} { }

void NeighMessage::prepare_neigh_message(struct nl_msg*) { }
void NeighMessage::process_neigh_message(struct nl_msg*, const struct ndmsg*) { }

void NeighMessage::prepare_message(struct nl_msg* msg) {
    struct ndmsg ndm;
    std::memset(&ndm, 0, sizeof(ndm));

    /* setup neighbor info */
    ndm.ndm_state = m_state;
    ndm.ndm_flags = uint8_t(m_flags);
    ndm.ndm_family = uint8_t(m_family);
    ndm.ndm_ifindex = int(m_ifindex);

    /* append neighbor information to netlink message msg */
    if (0 > nlmsg_append(msg, &ndm, sizeof(ndm), NLMSG_ALIGNTO)) {
        throw NlException("Filed to setup neighbor message");
    }

    /* prepare upper layer message */
    prepare_neigh_message(msg);
}

void NeighMessage::process_message(struct nl_msg* msg) {
    struct nlmsghdr* hdr = nlmsg_hdr(msg);
    struct ndmsg* ndm = static_cast<struct ndmsg*>(nlmsg_data(hdr));

    /* check message type */
    if ((RTM_NEWNEIGH != hdr->nlmsg_type) &&
        (RTM_DELNEIGH != hdr->nlmsg_type) &&
        (RTM_GETNEIGH != hdr->nlmsg_type)) {
        throw NlException("Incorrect neighbor message type");
    }

    /* check neighbor message length */
    if (nlmsg_size(sizeof(struct ndmsg)) > nlmsg_datalen(hdr)) {
        throw NlException("Currupted neighbor message header");
    }

    /* process mesasge by upper layer */
    process_neigh_message(msg, ndm);
}

NeighMessage::~NeighMessage() { }
