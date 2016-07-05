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
 * @file lag_message.cpp
 *
 * @brief Switch Lag configuration
 * */

#include "api/netlink/lag_message.hpp"

#include <net/if.h>

using namespace netlink_base;
using namespace agent::network::api::netlink;

LagMessage::LagMessage(const IfName& lag, const IfName& port) :
    Message(lag), m_port(port) { }

LagMessage::~LagMessage() { }

Message::Pointer LagMessage::prepare_netlink_msg() const {
    struct ifinfomsg ifi{};
    struct nlattr *lagspec{nullptr};
    const IfName& ifname{m_ifname};

    /* Allocate the lag netlink message */
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));
    if (!msg) {
        return nullptr;
    }

    ifi.ifi_family = AF_UNSPEC;
    ifi.ifi_change = IFI_CHANGE_RESERVED;

    /* Appending iface parameter details to netlink message msg */
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }

    if (0 > nla_put(msg.get(), IFLA_IFNAME, int(ifname.length()),
                    ifname.c_str())) {
        return nullptr;
    }

    /* Setup lag attributes */
    lagspec = nla_nest_start(msg.get(), IFLA_LINKINFO);
    if (!lagspec) {
        return nullptr;
    }

    if (0 > nla_put(msg.get(), IFLA_INFO_KIND, sizeof(TEAM), TEAM)) {
        return nullptr;
    }
    nla_nest_end(msg.get(), lagspec);

    return msg;
}

