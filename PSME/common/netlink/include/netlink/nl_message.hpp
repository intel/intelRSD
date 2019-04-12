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
 * @file nl_message.hpp
 *
 * @brief Netlink Message
 * */

#pragma once

#include "nl_socket.hpp"

#include <linux/netlink.h>
#include <exception>

/*! forward declarations */
struct sockaddr_nl;
struct nlmsgerr;
struct nl_msg;

namespace netlink_base {

/*!
 * @brief C++ Netlink Message
 */
class NlMessage {
public:
    /*!
     * @brief Default constructor.
     *
     * @param[in] proto Message protocol
     */
    NlMessage(NlSocket::Protocol proto);

    /*!
     * @brief Copy constructor.
     *
     * @param[in] orig object to be copied.
     */
    NlMessage(const NlMessage& orig) = delete;

    /*!
     * @brief Copy operator.
     *
     * @param[in] s object to be copied
     */
    NlMessage& operator=(const NlMessage& s) = delete;

    /*!
     * @brief Set type of message.
     *
     * @param[in] type Netlink message type to be set.
     */
    void set_nlhdr_type(int type) { m_type = type; }

    /*!
     * @brief Set flags of message
     *
     * @param[in] flags Netlink flags to be set.
     */
    void set_nlhdr_flags(int flags) { m_flags = flags; }

    /*!
     * @brief Send the message.
     */
    void send();

    /*!
     * @brief Default destructor.
     */
    virtual ~NlMessage();

protected:
    /*!
     * @brief Message handler.
     */
    virtual void process_message(struct nl_msg*) { }

    /*!
     * @brief Prepare message to be sent.
     */
    virtual void prepare_message(struct nl_msg*) { }

    /*!
     * @brief Get raw socket.
     */
    struct nl_sock *get_sock() { return m_socket.get_sock(); }

private:
    static int valid_message_handler(struct nl_msg*, void*);
    static int finish_message_handler(struct nl_msg* , void*);
    static int error_message_handler(struct sockaddr_nl*,
                                        struct nlmsgerr*, void*);
    /* class members */
    std::exception_ptr m_handler_exception{};
    NlSocket m_socket;
    int m_type{};
    int m_flags{};
};

}
