/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file lag_port_message.cpp
 *
 * @brief Switch Lag Port configuration
 * */

#include "api/netlink/lag_port_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <net/if.h>
#include <netlink/attr.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

LagPortMessage::LagPortMessage(const string& lag, const string& port) :
    LagMessage(lag, port) {
    auto index = if_nametoindex(port.c_str());
    if (0 == index) {
        throw NlExceptionInvalidIfName(get_member_name());
    }
    set_index(index);
    set_nlhdr_flags(NLM_F_REPLACE | NLM_F_REQUEST | NLM_F_ACK);
    set_nlhdr_type(RTM_SETLINK);
    set_family(AF_UNSPEC);
}

LagPortMessage::~LagPortMessage() { }

void LagPortMessage::prepare_link_message(struct nl_msg* msg) {
    const string& lag_name{get_ifname()};
    int lag_ifi{0}; // 0 means interface will be removed from LAG.

    if (!lag_name.empty()) {
        if (!iface_exists()) {
            throw NlExceptionInvalidIfName(lag_name);
        }
        lag_ifi = get_ifindex();
    }

    if (0 > nla_put(msg, IFLA_MASTER, sizeof(int), &lag_ifi)) {
        throw NlException("Cannot put IFLA_MASTER attribute");
    }
}
