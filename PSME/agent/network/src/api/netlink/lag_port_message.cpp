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
 * @file lag_port_message.cpp
 *
 * @brief Switch Lag Port configuration
 * */

#include "api/netlink/lag_port_message.hpp"

#include <net/if.h>

using namespace netlink_base;
using namespace agent::network::api::netlink;

LagPortMessage::LagPortMessage(const IfName& lag, const IfName& port) :
    LagMessage(lag, port) {
    set_flags(NLM_F_REPLACE | NLM_F_REQUEST | NLM_F_ACK);
    set_type(RTM_SETLINK);
}

LagPortMessage::~LagPortMessage() { }

Message::Pointer LagPortMessage::prepare_netlink_msg() const {
    struct ifinfomsg ifi{};
    const IfName& lag_name{m_ifname};
    int lag_ifi{0}; // 0 means interface will be removed from LAG.

    if (!lag_name.empty()) {
        lag_ifi = (int) if_nametoindex(lag_name.c_str());
        if (0 == lag_ifi) {
            return nullptr;
        }
    }

    /* Allocate the lag netlink message */
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));
    if (!msg) {
        return nullptr;
    }

    ifi.ifi_family = AF_UNSPEC;
    ifi.ifi_index = int(if_nametoindex(get_member_name().c_str()));
    if (0 == ifi.ifi_index) {
        return nullptr;
    }

    /* Appending iface parameter details to netlink message msg */
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }

    if (0 > nla_put(msg.get(), IFLA_MASTER, sizeof(int), &lag_ifi)) {
        return nullptr;
    }

    return msg;
}
