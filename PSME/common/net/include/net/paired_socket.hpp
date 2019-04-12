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
 * @file paired_socket.hpp
 *
 * @brief PairedSocket interface
 * */

#pragma once

#include "socket.hpp"
#include "socket_address.hpp"

namespace net {

/*! Represents one end of Socket pair created via socketpair call. */
class PairedSocket : public Socket {
public:
    /*! Factory method. Creates pair of connected unix datagram sockets.*/
    static std::pair<PairedSocket, PairedSocket> create_socket_pair();

    /*!
     * @brief Destructor.
     */
    ~PairedSocket();

    /*!
     * @brief Copy constructor.
     */
    PairedSocket(const PairedSocket& socket) = default;

    /*!
     * Sends the contents of the given buffer to the socket.
     *
     * @param buffer Data to be send.
     * @param length Length of data to be send.
     * @param flags Optional flags.
     * @return The number of bytes send (may be less than specified in length).
     */
    long send_bytes(const void* buffer, size_t length, int flags = 0);

    /*!
     * Receives data from socket and stores it in buffer.
     * Up to length bytes are received.
     *
     * @param buffer Buffer for data.
     * @param length Length of buffer.
     * @param flags Optional flags.
     * @return The number of bytes received.
     */
    long receive_bytes(void* buffer, size_t length, int flags = 0);

private:
    PairedSocket(net_socket_t fd);

    PairedSocket& operator=(const PairedSocket& socket) = delete;
};

/*! Pair of Sockets created via socketpair. */
using PairedSockets = std::pair<PairedSocket, PairedSocket>;

} // namespace net
