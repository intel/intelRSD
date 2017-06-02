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
 * @file add_vlan_port_message.cpp
 *
 * @brief Add Vlan port message implementation
 * */

#include "api/netlink/add_vlan_port_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <netlink/msg.h>
#include <net/if.h>
#include <linux/if_bridge.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

AddVlanPortMessage::AddVlanPortMessage(const string& port, uint32_t vlan, bool tagged,
        bool pvid) : m_vlan_info{vlan, tagged, pvid}, LinkMessage{port} {
    /* set-up Netlink attributes */
    set_nlhdr_flags(NLM_F_REQUEST | NLM_F_ACK);
    set_nlhdr_type(RTM_SETLINK);

    /* set-up Link attributes */
    set_index(get_ifindex());
    set_family(PF_BRIDGE);
}

AddVlanPortMessage::~AddVlanPortMessage() { }

void AddVlanPortMessage::prepare_link_message(struct nl_msg* msg) {
    struct bridge_vlan_info info{};
    struct nlattr *vlanspec{nullptr};

    /* validate interface */
    if (!iface_exists()) {
        throw NlExceptionInvalidIfName(get_ifname());
    }

    /* Setup vlan attributes */
    info.vid = uint16_t(m_vlan_info.get_vlan_id());
    vlanspec = nla_nest_start(msg, IFLA_AF_SPEC);
    if (!vlanspec) {
        throw NlException("Cannot put IFLA_AF_SPEC attribute");
    }

    /* insert bridge flags */
    if (0 > nla_put_u16(msg, IFLA_BRIDGE_FLAGS, BRIDGE_FLAGS_SELF)) {
        throw NlException("Cannot put bridge flags to the message");
    }

    if (!m_vlan_info.get_tagged()) {
        /* set tagged flag */
        info.flags |= BRIDGE_VLAN_INFO_UNTAGGED;
    }
    if (m_vlan_info.get_pvid()) {
        /* set pvid flag */
        info.flags |= BRIDGE_VLAN_INFO_PVID;
    }

    /* put vlan data to the message */
    if (0 > nla_put(msg, IFLA_BRIDGE_VLAN_INFO, sizeof(info), &info)) {
        throw NlException("Cannot put vlan info to the message");
    }
    nla_nest_end(msg, vlanspec);
}
