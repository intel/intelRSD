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
 * @file fdb_info_message.cpp
 *
 * @brief FDB Info Netlink message
 * */

#include "netlink/fdb_info_message.hpp"
#include "netlink/nl_exception.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <netlink/msg.h>
#include <linux/if_ether.h>
#include <linux/neighbour.h>

#include <cstring>
#include <iomanip>
#include <sstream>

using std::string;
using std::ostringstream;
using namespace netlink_base;

FdbInfoMessage::FdbInfoMessage(const string& ifname) :
        NeighMessage{ifname} {
    /* set-up Netlink attributes */
    set_nlhdr_flags(NLM_F_DUMP | NLM_F_REQUEST);
    set_nlhdr_type(RTM_GETNEIGH);

    /* set-up Neigbor attributes */
    set_ifindex(get_ifindex());
    set_family(PF_BRIDGE);
    set_state(NUD_NOARP);
#ifdef NTF_SELF
    set_flags(NTF_SELF);
#endif
}

void FdbInfoMessage::prepare_neigh_message(struct nl_msg*) {
    /* validate interface */
    if (!iface_exists()) {
        throw NlExceptionInvalidIfName(get_ifname());
    }
}

void FdbInfoMessage::process_neigh_message(struct nl_msg* msg, const struct ndmsg* ndm) {
    struct nla_policy neigh_policy[NDA_MAX + 1]{};
    struct nlattr* attrs[NDA_MAX + 1]{};
    EntryType type{EntryType::UNKNOWN};
    ostringstream mac{};
    uint16_t vlan{0};

    /* parse message attributes */
#ifdef NDA_VLAN
    neigh_policy[NDA_VLAN].type = NLA_U16;
#endif
    neigh_policy[NDA_LLADDR].maxlen = ETH_ALEN;
    neigh_policy[NDA_LLADDR].minlen = ETH_ALEN;
    int err = nlmsg_parse(nlmsg_hdr(msg), sizeof(struct ndmsg),
                                    attrs, NDA_MAX, neigh_policy);
    if (0 != err) {
        throw NlException("Filed to parse Netlink neighbor response");
    }

    /* get mac address */
    if (attrs[NDA_LLADDR]) {
        string trailing{};
        mac << std::setfill('0') << std::uppercase << std::hex;
        auto addr_ptr = static_cast<uint8_t*>(nla_data(attrs[NDA_LLADDR]));
        for (int i = 0; i < ETH_ALEN; ++i) {
            mac << trailing << std::setw(2) << int(addr_ptr[i]);
            trailing = ":";
        }
    }
    else {
        throw NlException("MAC address is missing");
    }

    /* get entry type */
    if (ndm->ndm_state & NUD_PERMANENT) {
        type = EntryType::STATIC;
    }
    else if (ndm->ndm_state & NUD_REACHABLE) {
        type = EntryType::DYNAMIC;
    }

#ifdef NDA_VLAN
    /* get vlan id (optional) */
    if (attrs[NDA_VLAN]) {
        vlan = nla_get_u16(attrs[NDA_VLAN]);
    }
#endif

    /* store the information */
    m_entries.emplace_back(mac.str(), vlan, type);
}

FdbInfoMessage::~FdbInfoMessage() { }
