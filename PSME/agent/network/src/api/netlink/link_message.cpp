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
 * @file link_message.cpp
 *
 * @brief Link message class
 *
 * */

#include "api/netlink/port_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <netlink/msg.h>
#include <net/if.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

LinkMessage::LinkMessage(const string& ifname) : RtMessage{ifname} { }

LinkMessage::~LinkMessage() {}

void LinkMessage::prepare_link_message(struct nl_msg*) { }
void LinkMessage::process_link_message(struct nl_msg*, const struct ifinfomsg*) { }

void LinkMessage::prepare_ifi_message(struct ifinfomsg&) {}

void LinkMessage::prepare_message(struct nl_msg* msg) {
    struct ifinfomsg ifi{};

    /* appending the iface information to netlink message msg */
    ifi.ifi_family = uint8_t(m_family);
    ifi.ifi_index = int(m_index);
    prepare_ifi_message(ifi);
    if (0 > nlmsg_append(msg, &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        throw NlException("Cannot append link message");
    }

    /* prepare upper layer message */
    prepare_link_message(msg);
}

void LinkMessage::process_message(struct nl_msg* msg) {
    struct nlmsghdr* hdr = nlmsg_hdr(msg);
    struct ifinfomsg* ifm = static_cast<struct ifinfomsg*>(nlmsg_data(hdr));

    /* check message type */
    if ((RTM_NEWLINK != hdr->nlmsg_type) &&
        (RTM_DELLINK != hdr->nlmsg_type) &&
        (RTM_GETLINK != hdr->nlmsg_type)) {
        throw NlException("Incorrect link message type");
    }

    /* check link message length */
    if (nlmsg_size(sizeof(struct ifinfomsg)) > nlmsg_datalen(hdr)) {
        throw NlException("Currupted link message header");
    }

    /* process mesasge by upper layer */
    process_link_message(msg, ifm);
}
