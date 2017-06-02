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
 * @file vlan_info_port_message.cpp
 *
 * @brief Vlan info class implementation
 * */

#include "api/netlink/info_vlan_port_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <netlink/msg.h>
#include <net/if.h>
#include <linux/if_bridge.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

InfoVlanPortMessage::InfoVlanPortMessage() : LinkMessage{""} {
    /* set-up Netlink attributes */
    set_nlhdr_flags(NLM_F_REQUEST | NLM_F_DUMP);
    set_nlhdr_type(RTM_GETLINK);

    /* set-up Link attributes */
    set_family(PF_BRIDGE);
}

void InfoVlanPortMessage::prepare_link_message(struct nl_msg* msg) {
    /* insert RTEXT_FILTER_BRVLAN mask */
    if (0 > nla_put_u32(msg, IFLA_EXT_MASK, RTEXT_FILTER_BRVLAN)) {
        throw NlException("Cannot put filter mask to the message");
    }
}

void InfoVlanPortMessage::process_link_message(struct nl_msg* msg, const struct ifinfomsg* ifm) {
    struct nlattr* attrs[IFLA_MAX + 1];
    char ifname[IF_NAMESIZE]{};

    /* check message family */
    if (AF_BRIDGE != ifm->ifi_family) {
        throw NlException("Invalid vlan bridge family");
    }

    /* parse nla attributes */
    int err = nlmsg_parse(nlmsg_hdr(msg), sizeof(struct ifinfomsg), attrs, IFLA_MAX, nullptr);
    if (0 != err) {
        throw NlException("Filed to parse vlan info message");
    }

    /* get interface name */
    if (nullptr == if_indextoname(uint32_t(ifm->ifi_index), ifname)) {
        throw NlException("Failed to get interface index");
    }

    /* if AF_SPEC isn't there, vlan table is not preset for this port */
    if (!attrs[IFLA_AF_SPEC]) {
        m_vlan_port_info[ string(ifname) ] = { };
        return;
    }
    else {
        VlanInfoList vlan_list{};
        struct nlattr* list = attrs[IFLA_AF_SPEC];

        int rem = nla_len(list);
        struct nlattr* nl_attr = static_cast<struct nlattr*>(nla_data(list));
        for(; nla_ok(nl_attr, rem); nl_attr = nla_next(nl_attr, &(rem))) {
            if (IFLA_BRIDGE_VLAN_INFO != nl_attr->nla_type) {
                continue;
            }
            struct bridge_vlan_info* vinfo = static_cast<struct bridge_vlan_info*>(nla_data(nl_attr));
            bool tag = !(vinfo->flags & BRIDGE_VLAN_INFO_UNTAGGED);
            bool pvid = (vinfo->flags & BRIDGE_VLAN_INFO_PVID);
            vlan_list.emplace_back(vinfo->vid, tag, pvid);
        }
        m_vlan_port_info[ string(ifname) ] = vlan_list;
    }
}

bool InfoVlanPortMessage::is_tagged(const string& port, uint32_t vlan) {
    auto port_iter = m_vlan_port_info.find(port);
    if (m_vlan_port_info.end() == port_iter) {
        throw std::runtime_error("is_tagged(): port=" + port + " not found");
    }
    const VlanInfoList& vlan_list = port_iter->second;
    for (const auto& vlan_info : vlan_list) {
        if (vlan_info.get_vlan_id() == vlan)
            return vlan_info.get_tagged();
    }
    throw std::runtime_error("is_tagged(): pair vlan="
                             + std::to_string(int(vlan))
                             + " port=" + port + " not found");
}

const InfoVlanPortMessage::VlanInfoList&
InfoVlanPortMessage::get_vlan_list(const string& ifname) const {
    /* find the port in port vlan list */
    auto port_iter = m_vlan_port_info.find(ifname);
    if (m_vlan_port_info.end() == port_iter) {
        throw std::runtime_error("get_vlan_list():"
                                 + ifname + " port not found");
    }

    /* return port vlan list */
    return port_iter->second;
}
