//
// IPAddress.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/IPAddress.h#2 $
//
// Library: Net
// Package: NetCore
// Module:  IPAddress
//
// Definition of the IPAddress class.
//
// Copyright (c) 2005-2011, Applied Informatics Software Engineering GmbH.
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
 * @file ipaddress.hpp
 *
 * @brief IPAddress interface
 * */

#pragma once

#include "socket_defs.hpp"
#include <stdexcept>
#include <ostream>

namespace net {

/*!
 * Represents an internet (IPv4 or IPv6) host address.
 */
class IpAddress {
public:
    static IpAddress from_sockaddr(const struct sockaddr& sockaddr);
    static IpAddress from_string(const std::string& address);

    /*! Constructor. Creates any IPv4 IP address. */
    IpAddress();

    /*!
     * Constructor. Creates IP address from in_addr structure.
     * @param[in] ipv4 Reference to in_addr structure.
     */
    explicit IpAddress(const struct in_addr& ipv4);

#ifdef NET_HAVE_IPv6
    /*!
     * Constructor. Creates IP address from in6_addr structure.
     * @param[in] ipv6 Reference to in6_addr structure.
     */
    explicit IpAddress(const struct in6_addr& ipv6);
#endif

    /*! Copy constructor */
    IpAddress(const IpAddress& other);

    /*! Destructor */
    virtual ~IpAddress();

    /*! Assignment operator */
    IpAddress& operator=(const IpAddress& other);

    bool operator==(const IpAddress& other) const;
    bool operator!=(const IpAddress& other) const;
    bool operator<(const IpAddress& other) const;
    bool operator>(const IpAddress& other) const;
    IpAddress operator & (const IpAddress& addr) const;
    IpAddress operator | (const IpAddress& addr) const;
    IpAddress operator ^ (const IpAddress& addr) const;
    IpAddress operator ~ () const;

    /*!
     * Returns address family enumeration.
     * @return Address family enumeration.
     */
    AddressFamily get_address_family() const {
        return m_family;
    }

    /*!
     * Returns native address family value.
     * @return Native address family value.
     */
    int get_native_address_family() const;

    /*!
     * Returns pointer to internal address representation.
     * @return Pointer to internal address representation.
     */
    const void* addr() const;

    /*!
     * Returns address as in_addr structure.
     * @return Address as in_addr structure.
     */
    in_addr ipv4_address() const;

#ifdef NET_HAVE_IPv6
    /*!
     * Returns address as in6_addr structure.
     * @return Address as in6_addr structure.
     */
    in6_addr ipv6_address() const;
#endif

    /*!
     * @return Number of bytes needed to store the raw address.
     */
    net_socklen_t length() const;

    /*!
     * @return String representation of IpAddress.
     */
    std::string to_string() const;

    /*!
     * @return IpAddress prefix length.
     */
    unsigned prefix_length() const;

    /*!
     * Tests for any address type (all zeros).
     * @return true if it's any address, false otherwise.
     */
    bool is_any_address() const;

private:
    AddressFamily m_family{AddressFamily::IPv4};

    union {
        in_addr ipv4;
#ifdef NET_HAVE_IPv6
        in6_addr ipv6;
#endif
    } m_addr{};
};

} // namespace net

std::ostream& operator << (std::ostream& ostr, const net::IpAddress& addr);
