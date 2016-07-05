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
 * @file port_attribute_message.cpp
 *
 * @brief Switch port attribute netlink message class
 *
 * */

/* Internal headers */
#include "api/netlink/port_attribute_message.hpp"
#include "logger/logger_factory.hpp"

/* C/C++ standard headers */
#include <net/if.h>

struct sw_attributes {
    __u64 attribute_val;     /* Value of the attribute*/
    __s64 attribute_index;   /* Attribute index */
};

using namespace netlink_base;
using namespace agent::network::api::netlink;

PortAttributeMessage::PortAttributeMessage(const IfName& ifname) : Message(ifname) {
    set_flags(NLM_F_REPLACE|NLM_F_REQUEST|NLM_F_ACK);
    set_type(RTM_SETLINK);
}

PortAttributeMessage::~PortAttributeMessage() {}

Message::Pointer PortAttributeMessage::prepare_netlink_msg() const {
    struct ifinfomsg ifi{};
    struct sw_attributes attr_val_index{};
    struct nlattr *port_config_attr;
    Pointer msg(nlmsg_alloc_simple(m_type, int(m_flags)));
    if (!msg) {
        return nullptr;
    }

    /* Appending the ndm parameter details to netlink message msg */
    if (0 > nlmsg_append(msg.get(), &ifi, sizeof(ifi), NLMSG_ALIGNTO)) {
        return nullptr;
    }

    /* add iface name into the message */
    if (0 > nla_put(msg.get(), IFLA_IFNAME, int((m_ifname.length())), m_ifname.c_str())) {
        return nullptr;
    }

    /* set port attribute into the message */
    port_config_attr = nla_nest_start(msg.get(), IFLA_SWPORT_ATTRS);
    if (!port_config_attr) {
        return nullptr;
    }
    attr_val_index.attribute_index = -1;
    attr_val_index.attribute_val = m_attribute_value;
    if (0 > nla_put(msg.get(), int(m_attribute), sizeof(attr_val_index),
        &attr_val_index)) {
        return nullptr;
    }
    nla_nest_end(msg.get(), port_config_attr);

    log_debug(GET_LOGGER("netlink"), "netlink switch message created for device " + m_ifname);
    return msg;
}

