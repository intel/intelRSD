/*!
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
 *
 * @file port_attribute_message.hpp
 *
 * @brief Switch port attribute netlink message class
 * */

#ifndef AGENT_NETWORK_NETLINK_PORT_ATTRIBUTE_MESSAGE_HPP
#define AGENT_NETWORK_NETLINK_PORT_ATTRIBUTE_MESSAGE_HPP

/* Internal headers */
#include "netlink/message.hpp"

/* C/C++ standard headers */

namespace agent {
namespace network {
namespace api {
namespace netlink {

/*! Port Attribute Message class */
class PortAttributeMessage: public netlink_base::Message {
public:
    using IfName = Message::IfName;
    using Attribute = uint32_t;
    using AttributeValue = uint32_t;

    /*!
     * @brief Default constructor.
     */
    PortAttributeMessage() = delete;

    /*!
     * @brief Default constructor.
     * @param[in] ifname Interface name.
     */
    PortAttributeMessage(const IfName& ifname);

    /*!
     * @brief Default destructor.
     */
    virtual ~PortAttributeMessage();

    /*!
     * @brief Prepare port netlink message.
     * @return Netlink message ready to be sent.
     */
    Pointer prepare_netlink_msg() const;

    /*!
     * @brief Set port attribute.
     * @param[in] attr Switch port attribute.
     * @param[in] value Switch port attribute value.
     */
    void set_attribute(Attribute attr, AttributeValue value) {
        m_attribute = attr;
        m_attribute_value = value;
    }

private:
    /* switch port attributes */
    Attribute m_attribute{};
    AttributeValue m_attribute_value{};

};

} /* netlink */
} /* api */
} /* network */
} /* agent */

#endif /* AGENT_NETWORK_NETLINK_PORT_ATTRIBUTE_MESSAGE_HPP */
