//
// SocketAddress.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/SocketAddress.h#2 $
//
// Library: Net
// Package: NetCore
// Module:  SocketAddress
//
// Definition of the SocketAddress class.
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
 * @file socket_address.hpp
 *
 * @brief SocketAddress interface
 * */

#pragma once

#include "ipaddress.hpp"
#include <ostream>


namespace net {
/*!
 * Structure large enough to hold any socket address
 */
struct SockaddrStorage {
    /*! Constructor */
    SockaddrStorage();

    /*! Copy Constructor */
    SockaddrStorage(const SockaddrStorage& other);

    /*! Assignment operator */
    SockaddrStorage& operator=(const SockaddrStorage& other);

    struct sockaddr_storage addr_storage;
    net_socklen_t addr_len;
    struct sockaddr* const addr;
};

/*!
 * @brief This class represents socket address.
 *
 * It can be IP socket address (IP address + port number) or
 * local (unix) socket address.
 */
class SocketAddress final {
public:
    /*!
     * @brief Constructor
     * Creates a wildcard (zero address and port) IPv4 SocketAddress.
     */
    SocketAddress();

    /*!
     * @brief Constructor
     * @param address Socket address.
     * @param port Socket port number.
     */
    SocketAddress(const IpAddress& address, std::uint16_t port);

    /*!
     * @brief Constructor
     * Creates a wildcard IPv4 address and given port.
     * @param port Socket port number.
     */
    explicit SocketAddress(std::uint16_t port);

    /*!
     * @brief Constructor
     * @param address IPv4 dotted decimal or IPv6 hex string address.
     * @param port Socket port number.
     */
    SocketAddress(const std::string& address, std::uint16_t port);

    /*!
     * @brief Constructor
     * @param sockaddr_storage SockaddrStorage holding socket address.
     */
    explicit SocketAddress(const SockaddrStorage& sockaddr_storage);

    /*!
     * Creates a SocketAddress from a socket name.
     *
     * Socket name is an IP address and port or an absolute path
     * in case of UNIX_LOCAL socket. Address and port must
     * be separated by a colon. In case of an IPv6 address,
     * the address part must be enclosed in brackets.
     * In case of local socket it's absolute path starting with a slash.
     *
     * Examples:
     *     192.168.0.1:80
     *     [::ffff:192.168.1.200]:2040
     *     /path/to/unix.socket
     *
     * @param[in] socket_name String representation of socket name.
     */
    explicit SocketAddress(const std::string& socket_name);

    /*!
     * Creates a SocketAddress of the given family from a
     * string representation of the address.
     *
     * @param family AddressFamily.
     * @param socket_name String representation of socket name.
     */
    SocketAddress(AddressFamily family, const std::string& socket_name);

    /*! @brief Copy constructor. */
    SocketAddress(const SocketAddress& socket_address) = default;

    /*! @brief Assignment operator. */
    SocketAddress& operator=(const SocketAddress& socket_address) = default;

    /*!
     * @brief Gets host IP address.
     * @return Host IP address.
     */
    IpAddress get_host() const;

    /*!
     * @brief Gets port number.
     * @return Port number.
     */
    std::uint16_t get_port() const;

    /*!
     * @brief Gets length of internal native socket address structure.
     * @return Length of internal native socket address structure.
     */
    net_socklen_t length() const;

    /*!
     * @brief Gets pointer to internal native socket address structure.
     * @return Pointer to internal native socket address structure.
     */
    const struct sockaddr* addr() const;

    /*!
     * @return address family (AF_INET, AF_INET6, AF_UNIX) of the address.
     */
    int af() const;

    /*!
     * @return string representation of the address.
     */
    std::string to_string() const;

    /*!
     * @return Address family of the host's address.
     */
    AddressFamily family() const;

    bool operator<(const SocketAddress& socket_address) const;
    bool operator==(const SocketAddress& socket_address) const;
    bool operator!=(const SocketAddress& socket_address) const;

private:
    void init(const IpAddress& address, std::uint16_t port);
    void init(AddressFamily family, const std::string& address);
    void init(const std::string& sock_name);

#if defined(NET_OS_FAMILY_UNIX)
    void init_unix_local(const std::string& path);
#endif
    SockaddrStorage address_storage{};
};
} // namespace net


std::ostream& operator<<(std::ostream& ostr, const net::SocketAddress& address);
