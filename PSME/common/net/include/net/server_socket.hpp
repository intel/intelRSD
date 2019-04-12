//
// ServerSocket.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/ServerSocket.h#2 $
//
// Library: Net
// Package: Sockets
// Module:  ServerSocket
//
// Definition of the ServerSocket class.
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
 * @file server_socket.hpp
 *
 * @brief ServerSocket interface
 * */

#pragma once

#include "stream_socket.hpp"

namespace net {

/*!
 * Interface to a server stream socket.
 */
class ServerSocket : public Socket {
public:
    /*!
     * Constructor. Creates uninitialized ServerSocket.
     * It must be bound to an address and put into listening state.
     */
    ServerSocket();

    /*! Constructor */
    explicit ServerSocket(const Socket& socket);

    /*!
     * Constructor. Creates server socket, assigns it to an address
     * and puts it into listening state.
     *
     * @param[in] address SocketAddress to be bound to this socket.
     * @param[in] backlog The maximum length to which the queue
     * of pending connections for Socket may grow.
     */
    ServerSocket(const SocketAddress& address, int backlog = 64);

    /*!
     * Constructor. Creates server socket, assigns it to any IPv4 address
     * on given port and puts it into listening state.
     *
     * @param[in] port Port to be bound to this socket.
     * @param[in] backlog The maximum length to which the queue
     * of pending connections for Socket may grow.
     */
    ServerSocket(std::uint16_t port, int backlog = 64);

    /*! Destructor */
    virtual ~ServerSocket();

    ServerSocket& operator=(const Socket& socket);

    /*!
     * Assigns the address to the socket.
     *
     * It is normally necessary to assign a local address using bind() to a
     * server socket, before it may receive connections.
     *
     * @param[in] address SocketAddress to be bound to this socket.
     * @param[in] reuse_address If true, sets the SO_REUSEADDR socket option.
     */
    virtual void bind(const SocketAddress& address, bool reuse_address = false);

    /*!
     * Assigns the address to the socket.
     *
     * It is normally necessary to assign a local address using bind() to a
     * server socket, before it may receive connections.
     *
     * @param[in] port Port to be bound to this socket.
     * @param[in] reuse_address If true, sets the SO_REUSEADDR socket option.
     */
    virtual void bind(std::uint16_t port, bool reuse_address = false);

#ifdef NET_HAVE_IPv6
    /*!
     * Assigns the IPv6 address to the socket.
     *
     * It is normally necessary to assign a local address using bind() to a
     * server socket, before it may receive connections.
     *
     * @param[in] address IPv6 SocketAddress to be bound to this socket.
     * @param[in] reuse_address If true, sets the SO_REUSEADDR socket option.
     * @param[in] ipv6only If true, sets IPPROTO_IPV6/IPV6_V6ONLY socket option.
     */
    virtual void bind6(const SocketAddress& address, bool reuse_address = false, bool ipv6only = false);

    /*!
     * Assigns the IPv6 address to the socket.
     *
     * It is normally necessary to assign a local address using bind() to a
     * server socket, before it may receive connections.
     *
     * @param[in] port Port to be bound to this socket.
     * @param[in] reuse_address If true, sets the SO_REUSEADDR socket option.
     * @param[in] ipv6only If true, sets IPPROTO_IPV6/IPV6_V6ONLY socket option.
     */
    virtual void bind6(std::uint16_t port, bool reuse_address = false, bool ipv6only = false);
#endif

    /*!
     * Listens for connections on a Socket.
     *
     * @param[in] backlog the maximum length to which the queue
     * of pending connections for Socket may grow.
     */
    virtual void listen(int backlog = 64);

    /*!
     * Accepts a connection on a listening socket.
     * It extracts the first connection request on the queue of pending
     * connections for the listening socket, creates and returns new connected socket.
     * If the queue is empty, waits until a connection request completes.
     *
     * @param[out] client_addr Client's SocketAddress.
     * @return New TCP socket for the connection with the client.
     */
    virtual StreamSocket accept(SocketAddress& client_addr);

    /*!
     * Accepts a connection on a listening socket.
     * It extracts the first connection request on the queue of pending
     * connections for the listening socket, creates and returns new connected socket.
     * If the queue is empty, waits until a connection request completes.
     *
     * @return New TCP socket for the connection with the client.
     */
    virtual StreamSocket accept();

protected:
    explicit ServerSocket(SocketImpl::Ptr impl);
};

} // namespace net
