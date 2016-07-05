/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

extern "C" {
#include <net/if.h>
#include <netinet/ether.h>
}

using namespace netlink_base;
using namespace agent::network::api::netlink;
using namespace std;

StaticMacMessage::StaticMacMessage(bool add, const string& port,
                                   const string& address, uint16_t vlan_id) :
    Message(), m_add(add), m_port(port), m_vlan_id(vlan_id),
    m_address(address) {}

StaticMacMessage::~StaticMacMessage() {}

namespace {
    void set_address(Message::Pointer& msg, const string& address) {
        ether_addr ether{};

        if (NULL == ether_aton_r(address.c_str(), &ether)) {
            throw runtime_error("Invalid address was provided: " + address);
        }

        int err = nla_put(msg.get(), NDA_LLADDR, ETH_ALEN, &ether);
        if (0 > err) {
            throw runtime_error("nla_put address failed: "
                                + std::to_string(err));
        }
    }

    void set_vlan(Message::Pointer& msg, uint16_t vlan_id) {
        int err = nla_put(msg.get(), NDA_VLAN, sizeof(vlan_id), &vlan_id);
        if (0 > err) {
            throw runtime_error("nla_put vlan failed: " + std::to_string(err));
        }
    }

    void init_netlink_data(Message::Pointer& msg, const string& port) {
        ndmsg ndm{};
        ndm.ndm_family = PF_BRIDGE;
        ndm.ndm_state = NUD_NOARP | NUD_PERMANENT;
        ndm.ndm_flags = NTF_SELF;
        int ifindex = if_nametoindex(port.c_str());
        if (0 == ifindex) {
            throw runtime_error("Invalid port identifier " + port + " " +
                                std::to_string(errno));
        }
        ndm.ndm_ifindex = ifindex;
        int ret = nlmsg_append(msg.get(), &ndm, sizeof(ndm), NLMSG_ALIGNTO);
        if (0 > ret) {
            throw runtime_error("nlmsg_append failed: " + std::to_string(ret));
        }
    }
}

Message::Pointer StaticMacMessage::prepare_netlink_msg() const {
    int type{0};
    int flags{0};

    if (m_add) {
        type = RTM_NEWNEIGH;
        flags = NLM_F_CREATE | NLM_F_EXCL;
    }
    else {
        type = RTM_DELNEIGH;
    }
    /* Allocate the netlink message */
    Pointer msg(nlmsg_alloc_simple(type, flags));
    if (!msg) {
        return nullptr;
    }
    ::init_netlink_data(msg, m_port);
    ::set_address(msg, m_address);
    ::set_vlan(msg, m_vlan_id);
    return msg;
}
