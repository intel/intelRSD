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
 * @file lag_message.cpp
 *
 * @brief Switch Lag configuration
 * */

#include "api/netlink/lag_message.hpp"
#include "netlink/nl_exception.hpp"

#include <net/if.h>
#include <linux/if_link.h>
#include <netlink/attr.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

LagMessage::LagMessage(const string& lag, const string& port) :
    LinkMessage(lag), m_port(port) {
    set_family(AF_UNSPEC);
}

LagMessage::~LagMessage() { }

void LagMessage::prepare_link_message(struct nl_msg* msg) {
    struct nlattr *lagspec{nullptr};

    if (0 > nla_put(msg, IFLA_IFNAME, int(get_ifname().length()),
                    get_ifname().c_str())) {
        throw NlException("Cannot put iface name to the message");
    }

    /* Setup lag attributes */
    lagspec = nla_nest_start(msg, IFLA_LINKINFO);
    if (!lagspec) {
        throw NlException("nla_nest_start failed");
    }

    if (0 > nla_put(msg, IFLA_INFO_KIND, sizeof(TEAM), TEAM)) {
        throw NlException("Cannot put IFLA_INFO_KIND to the message");
    }
    nla_nest_end(msg, lagspec);
}
