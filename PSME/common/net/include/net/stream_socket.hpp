//
// StreamSocket.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/StreamSocket.h#1 $
//
// Library: Net
// Package: Sockets
// Module:  StreamSocket
//
// Definition of the StreamSocket class.
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//
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
 * @file stream_socket.hpp
 *
 * @brief StreamSocket interface
 * */

#pragma once

#include "socket.hpp"
#include "socket_address.hpp"
#include "socket_impl.hpp"

namespace net {

class StreamSocketImpl;

/*!
 * Interface to a client stream socket.
 */
class StreamSocket : public Socket {
public:
    /*!
     * Constructor. Creates uninitialized StreamSocket.
     * Before send or receive connect must be called.
     */
    StreamSocket();

    /*!
     * Constructor. Creates StreamSocket and connects it the specified address.
     * @param[in] address SocketAddress where connection is established.
     */
    explicit StreamSocket(const SocketAddress& address);

    /*!
     * Constructor. Creates unconnected StreamSocket of given AddressFamily.
     * Useful if some socket options need be set prior to connection.
     * @param[in] family AddressFamily of created stream socket.
     */
    explicit StreamSocket(AddressFamily family);

    /*!
     * Constructor. Creates StreamSocket with SocketImpl of another Socket.
     * @param[in] socket Another Socket.
     */
    explicit StreamSocket(const Socket& socket);

    /*!
     * Destructor.
     */
    virtual ~StreamSocket();

    /*!
     * Assignment operator.
     */
    StreamSocket& operator=(const StreamSocket& socket) = default;
    StreamSocket(const StreamSocket& socket) = default;

    /*!
     * Assignment operator.
     */
    StreamSocket& operator=(const Socket& socket);

    /*!
     * Initializes the socket (if not initialized) and connects
     * to the server at given address.
     * @param[in] address Peer address we try to connect to.
     */
    void connect(const SocketAddress& address);

    /*!
     * Initializes the socket (if not initialized) and connects
     * to the server at given address.
     * @param[in] address Peer address we try to connect to.
     * @param[in] timeout Interval connect is waiting to establish connection.
     */
    void connect(const SocketAddress& address, const Duration& timeout);

    /*!
     * Initializes the socket (if not initialized) and connects
     * to the server at given address.
     * Before opening the connection the socket is set to nonblocking mode.
     * @param[in] address Peer address we try to connect to.
     */
    void connect_non_blocking(const SocketAddress& address);

    /*!
     * Makes further receptions on socket will be disallowed.
     */
    void shutdown_receive();

    /*!
     * Makes further transmissions on socket will be disallowed.
     */
    void shutdown_send();

    /*!
     * Makes further receptions and transmissions on socket will be disallowed.
     */
    void shutdown();

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
     * @return The number of bytes received. Returned zero value means connection
     * was shutdown by peer.
     * @throw std::runtime_exception if receive timeout has been set and nothing
     * was received within that interval.
     */
    long receive_bytes(void* buffer, size_t length, int flags = 0);

    /*!
     * Sends one byte of urgent data through the socket.
     * The preferred way for a socket to receive urgent data
     * is by enabling the SO_OOBINLINE option.
     *
     * @param[in] data Byte sent with the MSG_OOB flag set.
     */
    void send_urgent(unsigned char data);

    /*!
     * Creates the Socket from the given SocketImpl.
     * The socket takes ownership of the SocketImpl.
     *
     * @param[in] impl Pointer to SocketImpl, which must be a StreamSocketImpl.
     */
    explicit StreamSocket(SocketImpl::Ptr impl);
};

} // namespace net
