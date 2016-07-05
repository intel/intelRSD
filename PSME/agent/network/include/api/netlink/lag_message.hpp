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
 * @file lag_message.hpp
 *
 * @brief Switch Lag configuration
 * */

#ifndef AGENT_NETWORK_NETLINK_LAG_MESSAGE_HPP
#define AGENT_NETWORK_NETLINK_LAG_MESSAGE_HPP

#include "netlink/message.hpp"

namespace agent {
namespace network {
namespace api {
namespace netlink {

/*! Lag Message */
class LagMessage: public netlink_base::Message {
public:
    /*!
     * @brief Default constructor.
     * @param[in] lag Lag interface name.
     * @param[in] port Port interface name.
     */
    LagMessage(const IfName& lag, const IfName& port);

    /*!
     * @brief Default destructor.
     */
    virtual ~LagMessage();

    /*!
     * @brief Set Lag port name.
     * @param[in] port Lag port interface name.
     */
    void set_member_name(const IfName& port) { m_port = port; }

    /*!
     * @brief Get Lag port name.
     * @return m_port Lag port interface name.
     */
    IfName get_member_name() const { return m_port; }

    /*!
     * @brief Prepare add Lag netlink message.
     * @return Netlink message ready to be sent.
     */
    Pointer prepare_netlink_msg() const;

protected:

    /*! @brief Linux driver kind name used for
     *         hardware offloading of LAG feature. */
    static const constexpr char* TEAM = "team";

private:

    IfName m_port;
};

}
}
}
}

#endif /* AGENT_NETWORK_NETLINK_LAG_MESSAGE_HPP */
