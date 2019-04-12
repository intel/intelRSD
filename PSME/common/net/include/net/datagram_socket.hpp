//
// DatagramSocket.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/DatagramSocket.h#1 $
//
// Library: Net
// Package: Sockets
// Module:  DatagramSocket
//
// Definition of the DatagramSocket class.
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
 * @file datagram_socket.hpp
 *
 * @brief DatagramSocket interface
 * */

#pragma once

#include "socket.hpp"

namespace net {

/*!
 * @brief This class represents a socket for sending/receiving
 * datagram packets.
 */
class DatagramSocket : public Socket {
public:
    /*!
     * @brief Constructor
     * Creates an unconnected, unbound datagram socket.
     * Before the socket can be used, bind(),
     * bind6() or connect() must be called.
     */
    DatagramSocket();

    /*!
     * @brief Constructor
     * @param family AddressFamily of created socket.
     */
    explicit DatagramSocket(AddressFamily family);

    /*!
     * @brief Constructor
     * @param address Address the socket is bound to.
     * @param reuse_address Specifies if socket is reusable.
     */
    DatagramSocket(const SocketAddress& address, bool reuse_address = false);

    /*! @brief Copy constructor. */
    DatagramSocket(const DatagramSocket& socket) = default;

    /*! @brief Assignment operator. */
    DatagramSocket& operator= (const DatagramSocket& socket) = default;

    /*!
     * @brief Constructor
     * @param socket Other socket, which SocketImpl is used for initialization
     * of created socket. The SocketImpl must be a DatagramSocketImpl.
     */
    DatagramSocket(const Socket& socket);

    /*!
     * @brief Destructor
     */
    ~DatagramSocket();

    /*!
     * @brief Assignment operator.
     */
    DatagramSocket& operator=(const Socket& socket);

    /*
     * @brief Restricts incoming and outgoing packets
     * to the specified address.
     * Should not be called before bind.
     *
     * @param address Address to restrict network traffic on.
     */
    void connect(const SocketAddress& address);

    /*
     * @brief Binds a local address to the socket.
     *
     * @param address Address to bind to the socket.
     * @param reuse_address Flag for SO_REUSEADDR socket option.
     */
    void bind(const SocketAddress& address, bool reuse_address = false);

    /*
     * @brief Sends the contents of the given buffer to the socket.
     *
     * @param buffer Data to be send.
     * @param length Length of data to be send.
     * @param flags Optional flags.
     * @return The number of bytes send (may be less than specified in length).
     */
    long send_bytes(const void* buffer, size_t length, int flags = 0);

    /*
     * @brief Receives data from socket and stores it in buffer.
     * Up to length bytes are received.
     *
     * @param buffer Buffer for data.
     * @param length Length of buffer.
     * @param flags Optional flags.
     * @return The number of bytes received.
     */
    long receive_bytes(void* buffer, size_t length, int flags = 0);

    /*!
     * Sends the contents of the given buffer to the given address.
     *
     * @param buffer Data to be send.
     * @param length Length of data to be send.
     * @param address Address of the target.
     * @param flags Optional flags.
     * @return The number of bytes send (may be less than specified in length).
     */
    long send_to(const void* buffer, size_t length, const SocketAddress& address, int flags = 0);

    /*
     * @brief Receives data from socket and stores it in buffer.
     * Up to length bytes are received.
     * The address of sender is stored in address parameter.
     *
     * @param buffer Buffer for data.
     * @param length Length of buffer.
     * @param address Sender address.
     * @param flags Optional flags.
     * @return The number of bytes received.
     */
    long receive_from(void* buffer, size_t length, SocketAddress& address, int flags = 0);

    /*!
     * @brief Sets the value of the SO_BROADCAST socket option.
     * Setting this option allows sending packets to the broadcast address.
     *
     * @param flag Flag to enable/disable SO_BROADCAST socket option.
     */
    void set_broadcast(bool flag);

    /*!
     * @return The value of the SO_BROADCAST socket option.
     */
    bool is_broadcast_enabled() const;
};

} // namespace net
