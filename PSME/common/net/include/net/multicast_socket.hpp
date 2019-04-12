//
// MulticastSocket.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/MulticastSocket.h#1 $
//
// Library: Net
// Package: Sockets
// Module:  MulticastSocket
//
// Definition of the MulticastSocket class.
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
 * @file multicast_socket.hpp
 *
 * @brief MulticastSocket interface
 * */

#pragma once

#include "datagram_socket.hpp"

namespace net {

class NetworkInterface;

/*!
 * @brief A MulticastSocket is a (UDP) DatagramSocket for sending and receiving
 * IP multicast packets.
 * MulticastSocket has additional capabilities for joining multicast groups.
 */
class MulticastSocket : public DatagramSocket {
public:
    /*!
     * @brief Constructor
     * Creates an unconnected, unbound multicast socket.
     * Before the multicast socket can be used, bind() or connect() must be called.
     */
    MulticastSocket();

    /*!
     * @brief Constructor
     * @param family AddressFamily of created socket.
     */
    explicit MulticastSocket(AddressFamily family);

    /*!
     * @brief Constructor
     * @param address Address the socket is bound to.
     * @param reuse_address Specifies if socket is reusable.
     */
    MulticastSocket(const SocketAddress& address, bool reuse_address = false);

    /*! @brief Copy constructor. */
    MulticastSocket(const MulticastSocket& socket) = default;

    /*! @brief Assignment operator. */
    MulticastSocket& operator=(const MulticastSocket& socket) = default;

    /*!
     * @brief Constructor
     * @param socket Other socket, which SocketImpl is used for initialization
     * of created socket. The SocketImpl must be a DatagramSocketImpl.
     */
    MulticastSocket(const Socket& socket);

    /*!
     * @brief Destructor
     */
    ~MulticastSocket();

    /*!
     * @brief Assignment operator.
     */
    MulticastSocket& operator=(const Socket& socket);

    /*!
     * @brief Sets the interface used for sending multicast packets.
     *
     * @param interface NetworkInterface used for sending multicast packets.
     */
    void set_interface(const NetworkInterface& interface);

    /*!
     * @brief Gets the interface used for sending multicast packets.
     *
     * @return NetworkInterface used for sending multicast packets.
     */
    NetworkInterface get_interface() const;

    /*!
     * @brief Enables or disables loopback for multicast packets.
     *
     * Sets the value of the IP_MULTICAST_LOOP/IPV6_MULTICAST_LOOP
     * socket option.
     *
     * @param flag Enable/disable flag.
     */
    void set_loopback(bool flag);

    /*!
     * @return true if loopback for multicast packets is enabled, false
     * otherwise.
     */
    bool is_loopback() const;

    /*!
     * @brief Sets the TTL/hop limit for outgoing packets.
     *
     * Sets the value of the IP_MULTICAST_TTL/IPV6_MULTICAST_HOPS
     * socket option.
     *
     * @param value TTL value to be set.
     */
    void set_ttl(unsigned char value);

    /*!
     * @brief Gets the TTL/hop limit for outgoing packets.
     * @return TTL/hop limit for outgoing packets.
     */
    unsigned char get_ttl() const;

    /*!
     * @brief Joins the specified multicast group at the given interface.
     *
     * @param group_address Multicast group address to be joined.
     * @param interface Interface to receive multicast datagram packets.
     */
    void join_group(const IpAddress& group_address, const NetworkInterface& interface);

    /*!
     * @brief Leaves the specified multicast group at the given interface.
     *
     * @param group_address Multicast group address to leave.
     * @param interface Interface to leave.
     */
    void leave_group(const IpAddress& group_address, const NetworkInterface& interface);
};

} // namespace net
