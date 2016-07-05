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
 * @file port_message.hpp
 *
 * @brief Switch port netlink message class
 * */

#ifndef AGENT_NETWORK_NETLINK_PORT_MESSAGE_HPP
#define AGENT_NETWORK_NETLINK_PORT_MESSAGE_HPP

/* Internal headers */
#include "netlink/message.hpp"

/* C/C++ standard headers */

namespace agent {
namespace network {
namespace api {
namespace netlink {

/*! Port Message class */
class PortMessage: public netlink_base::Message {
public:
    using IfName = Message::IfName;

    /*!
     * @brief Default constructor.
     */
    PortMessage() = delete;

    /*!
     * @brief Default constructor.
     * @param[in] ifname Interface name.
     */
    PortMessage(const IfName& ifname);

    /*!
     * @brief Default destructor.
     */
    virtual ~PortMessage();

    /*!
     * @brief Prepare port netlink message.
     * @return Netlink message ready to be sent.
     */
    Pointer prepare_netlink_msg() const;

    /*!
     * @brief Parse recv netlink header.
     * @param[in] nlhdr Netlink header.
     */
    void parse_netlink_hdr(NetlinkHeader& nlhdr);

    /*!
     * @brief Get master port.
     * @return Master port.
     */
    const IfName& get_master() { return m_master; }

    /*!
     * @brief Check if the port in a member.
     * @return Is port member (true/false).
     */
    bool is_member() const { return m_member; }

private:

    /*! is the member port */
    bool m_member{false};

    /*! master interface name */
    IfName m_master{};
};

} /* netlink */
} /* api */
} /* network */
} /* agent */

#endif /* AGENT_NETWORK_NETLINK_PORT_MESSAGE_HPP */
