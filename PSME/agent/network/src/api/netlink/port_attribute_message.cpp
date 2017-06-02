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
 * @file port_attribute_message.cpp
 *
 * @brief Switch port attribute netlink message class
 *
 * */

/* Internal headers */
#include "api/netlink/port_attribute_message.hpp"
#include "logger/logger_factory.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

/* C/C++ standard headers */
#include <netlink/msg.h>
#include <linux/rtnetlink.h>

struct sw_attributes {
    __u64 attribute_val;     /* Value of the attribute*/
    __s64 attribute_index;   /* Attribute index */
};

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

PortAttributeMessage::PortAttributeMessage(const string& ifname) :
        LinkMessage{ifname} {
    set_nlhdr_flags(NLM_F_REPLACE|NLM_F_REQUEST|NLM_F_ACK);
    set_nlhdr_type(RTM_SETLINK);
}

PortAttributeMessage::~PortAttributeMessage() {}

void PortAttributeMessage::prepare_link_message(struct nl_msg* msg) {
    struct sw_attributes attr_val_index{};
    struct nlattr *port_config_attr{nullptr};

    /* validate interface */
    if (!iface_exists()) {
        throw NlExceptionInvalidIfName(get_ifname());
    }

    /* add iface name into the message */
    if (0 > nla_put(msg, IFLA_IFNAME, int((get_ifname().length())),
        get_ifname().c_str())) {
        throw NlException("Cannot put iface name to the message");
    }

    /* set port attribute into the message */
    port_config_attr = nla_nest_start(msg, IFLA_SWPORT_ATTRS);
    if (!port_config_attr) {
        throw NlException("Cannot start nested sw port attr");
    }
    attr_val_index.attribute_index = -1;
    attr_val_index.attribute_val = m_attribute_value;
    if (0 > nla_put(msg, int(m_attribute), sizeof(attr_val_index),
        &attr_val_index)) {
        throw NlException("Cannot put sw port attrs to the message");
    }
    nla_nest_end(msg, port_config_attr);

    log_debug(GET_LOGGER("netlink"), "netlink switch message created for device " + get_ifname());
}
