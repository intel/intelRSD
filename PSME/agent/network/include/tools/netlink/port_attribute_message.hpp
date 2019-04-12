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
 *
 * @file port_attribute_message.hpp
 *
 * @brief Switch port attribute netlink message class
 * */

#pragma once

/* Internal headers */
#include "tools/netlink/link_message.hpp"

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*!
 * @brief Port Attribute Message class
 * */
class PortAttributeMessage: public agent::network::tools::netlink::LinkMessage {
public:
    using Attribute = uint32_t;
    using AttributeValue = uint32_t;

    /*!
     * @brief Default constructor
     *
     * @param[in] ifname Interface name
     * */
    PortAttributeMessage(const std::string& ifname);

    /*!
     * @brief Default destructor
     * */
    virtual ~PortAttributeMessage();

    /*!
     * @brief Set port attribute
     *
     * @param[in] attr Switch port attribute.
     * @param[in] value Switch port attribute value.
     * */
    void set_attribute(Attribute attr, AttributeValue value) {
        m_attribute = attr;
        m_attribute_value = value;
    }

protected:
    /*!
     * @brief Prepare message to be sent.
     * */
    void prepare_link_message(struct nl_msg*) override;

private:
    /* switch port attributes */
    Attribute m_attribute{};
    AttributeValue m_attribute_value{};
};

} /* netlink */
} /* tools */
} /* network */
} /* agent */

