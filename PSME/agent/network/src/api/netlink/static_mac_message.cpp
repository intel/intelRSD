/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file static_mac_message.cpp
 *
 * @brief Static MAC netlink message class
 *
 * */

/* Internal headers */
#include "api/netlink/static_mac_message.hpp"
#include "netlink/nl_exception.hpp"

extern "C" {
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/rtnetlink.h>
#include <netlink/attr.h>
}
#include <algorithm>

using namespace netlink_base;
using namespace agent::network::api::netlink;
using namespace std;

StaticMacMessage::StaticMacMessage(bool add, const string& port,
                                   const string& address, uint16_t vlan_id) :
    NeighMessage(port), m_vlan_id(vlan_id), m_address(address) {
    if (add) {
        set_nlhdr_type(RTM_NEWNEIGH);
        set_nlhdr_flags(NLM_F_CREATE | NLM_F_EXCL);
    }
    else {
        set_nlhdr_type(RTM_DELNEIGH);
    }
    set_ifindex(get_ifindex());
    set_family(PF_BRIDGE);
    set_state(NUD_NOARP | NUD_PERMANENT);
    set_flags(NTF_SELF);
}

StaticMacMessage::~StaticMacMessage() {}

namespace {
    void set_address(struct nl_msg* msg, string& address) {
        // according to metadata, MAC address can use both colons and dashes: aa:bb-cc:dd-ee:ff
        // however, ether_aton_r function only accepts notation with colons.
        // So, we convert the dashes to colons before checking if the address is correct
        std::replace(address.begin(), address.end(), '-', ':');

        ether_addr ether{};

        if (nullptr == ether_aton_r(address.c_str(), &ether)) {
            throw NlException("Invalid address was provided: " + address);
        }

        int err = nla_put(msg, NDA_LLADDR, ETH_ALEN, &ether);
        if (0 > err) {
            throw NlException("nla_put address failed: "
                              + std::to_string(err));
        }
    }

    void set_vlan(struct nl_msg* msg, uint16_t vlan_id) {
        int err = nla_put(msg, NDA_VLAN, sizeof(vlan_id), &vlan_id);
        if (0 > err) {
            throw NlException("nla_put vlan failed: " + std::to_string(err));
        }
    }
}

void StaticMacMessage::prepare_neigh_message(struct nl_msg* msg) {
    ::set_address(msg, m_address);
    ::set_vlan(msg, m_vlan_id);
}
