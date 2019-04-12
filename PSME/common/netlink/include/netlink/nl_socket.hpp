/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file nl_socket.hpp
 *
 * @brief Netlink Socket
 * */

#pragma once

#include <linux/netlink.h>

/*! forward declarations */
struct nl_sock;

namespace netlink_base {

/*!
 * @brief C++ Netlink Socket
 */
class NlSocket {
public:
    /*!
     * @enum Protocol
     *
     * @brief Netlink socket protocol
     *
     * @var Protocol::ROUTE
     * Routing Netlink socket
     *
     * @var Protocol::USER
     * User space Netlink socket
     *
     * */
    enum class Protocol {
        ROUTE = NETLINK_ROUTE,
        USER  = NETLINK_USERSOCK
    };

    /*!
     * @brief Default constructor.
     *
     * @param[in] proto Socket protocol
     */
    NlSocket(Protocol proto);

    /*!
     * @brief Copy constructor.
     *
     * @param[in] orig object to be copied.
     */
    NlSocket(const NlSocket& orig) = delete;

    /*!
     * @brief Copy operator.
     *
     * @param[in] s object to be copied
     */
    NlSocket& operator=(const NlSocket& s) = delete;

    /*!
     * @brief Default destructor.
     */
    virtual ~NlSocket();

    /*!
     * @brief Get raw socket.
     */
    struct nl_sock *get_sock() { return m_sock; }

    /*!
     * @brief Get protocol.
     */
    int get_proto() { return m_proto; }

private:
    struct nl_sock* m_sock;
    int m_proto;
};

}
