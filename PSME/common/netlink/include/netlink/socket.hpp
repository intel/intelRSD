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
 * @file socket.hpp
 *
 * @brief Netlink socket class
 * */

#pragma once

#include "message.hpp"

#include <memory>

/*! forward declarations of types that are used */
struct nl_sock;

namespace netlink_base {

/*!
 * @brief Netlink socket.
 */
class Socket {
public:
    /*!
     * @brief Default constructor.
     */
    Socket();

    /*!
     * @brief Copy constructor.
     *
     * @param[in] orig object to be copied.
     */
    Socket(const Socket& orig) = delete;

    /*!
     * @brief Copy operator.
     *
     * @param[in] s object to be copied
     */
    Socket& operator= (const Socket& s) = delete;

    /*!
     * @brief Default destructor.
     */
    virtual ~Socket();

    /*!
     * @brief Connect socket.
     */
    virtual void connect();

    /*!
     * @brief Send a message through the socket.
     *
     * @param[in] message Message to be sent.
     */
    void send_message(const Message& message);

    /*!
     * @brief Send a route netlink message through the socket.
     *
     * @param[in] msg Message to be sent.
     */
    void send_rtnl_message(Message& msg);

    /*!
     * @brief Receive link state message from socket.
     *
     * @param[in] message Message to be received
     */
    void receive_link_state(Message& message);

    /*!
     * @brief Receive message from socket.
     *
     * @param[out] message Message to be received
     */
    void receive_message(Message& message);

    /*!
     * @brief Set a custom callback that is called on each received message.
     *
     * @param[in] func function to be called.
     * @param[in] arg agrument that will be passed to that function.
     *
     * @return true if the callback function was set properly, false otherwise.
     */
    bool modify_callback(nl_recvmsg_msg_cb_t func, void *arg);

    /*!
     * @brief Receive messages from socket and call the callback that was set.
     *
     * @param[out] message Message to be received
     */
    void receive_to_callback(Message& message);

    /*!
     * @brief Check for known error code and throw appropriate NetlinkException
     *
     * @param[in] error_code Error code
     */
    [[noreturn]] void throw_error(const int error_code) const;

protected:
    struct Deleter {
        void operator()(struct nl_sock* sock) const {
            nl_socket_free(sock);
        }
    };

    using Pointer = std::unique_ptr<struct nl_sock, Deleter>;
    Pointer m_sock;

    static constexpr uint16_t RECV_MESSAGE_SIZE = 3*16384;
};

}

