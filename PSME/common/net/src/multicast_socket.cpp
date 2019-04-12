//
// MulticastSocket.cpp
//
// $Id: //poco/1.4/Net/src/MulticastSocket.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  MulticastSocket
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
 * @file multicast_socket.cpp
 *
 * @brief MulticastSocket interface
 * */

#include "net/multicast_socket.hpp"
#include "net/network_interface.hpp"
#include "net/socket_impl.hpp"

#include <cstring>


// some Unix variants don't have IPV6_ADD_MEMBERSHIP/IPV6_DROP_MEMBERSHIP
#if defined(IPV6_JOIN_GROUP) && !defined(IPV6_ADD_MEMBERSHIP)
#define IPV6_ADD_MEMBERSHIP  IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif


namespace net {

MulticastSocket::MulticastSocket() {
}

MulticastSocket::MulticastSocket(AddressFamily family) : DatagramSocket(family) {
#if defined(NET_OS_FAMILY_UNIX)
    if (AddressFamily::UNIX_LOCAL == family) {
        throw NetException("Cannot create a MulticastSocket with UNIX_LOCAL socket");
    }
#endif
}

MulticastSocket::MulticastSocket(const SocketAddress& address, bool reuse_address)
    : DatagramSocket(address, reuse_address) {
}

MulticastSocket::MulticastSocket(const Socket& socket) : DatagramSocket(socket) {
}

MulticastSocket::~MulticastSocket() {
}

MulticastSocket& MulticastSocket::operator=(const Socket& socket) {
    DatagramSocket::operator=(socket);
    return *this;
}

void MulticastSocket::set_interface(const NetworkInterface& interface) {
    if (get_address().af() == AF_INET) {
        impl()->set_option(IPPROTO_IP, IP_MULTICAST_IF, interface.get_first_address(AddressFamily::IPv4));
    }
#ifdef NET_HAVE_IPv6
    else if (get_address().af() == AF_INET6) {
        impl()->set_option(IPPROTO_IPV6, IPV6_MULTICAST_IF, interface.get_index());
    }
#endif
    else {
        throw NetException("Unsupported address family.");
    }
}

NetworkInterface MulticastSocket::get_interface() const {
    try {
        struct in_addr addr;
        net_socklen_t len = sizeof(addr);
        impl()->get_raw_option(IPPROTO_IP, IP_MULTICAST_IF, &addr, len);
        return NetworkInterface::for_address(IpAddress(addr));
    }
    catch (std::exception&) {
#ifdef NET_HAVE_IPv6
        int ix;
        impl()->get_option(IPPROTO_IPV6, IPV6_MULTICAST_IF, ix);
        return NetworkInterface::for_index(ix);
#else
        throw;
#endif
    }
}

void MulticastSocket::set_loopback(bool flag) {
    if (get_address().af() == AF_INET) {
        unsigned char uflag = flag ? 1 : 0;
        impl()->set_option(IPPROTO_IP, IP_MULTICAST_LOOP, uflag);
    }
    else {
#ifdef NET_HAVE_IPv6
        unsigned uflag = flag ? 1 : 0;
        impl()->set_option(IPPROTO_IPV6, IPV6_MULTICAST_LOOP, uflag);
#endif
    }
}

bool MulticastSocket::is_loopback() const {
    bool flag = false;
    if (get_address().af() == AF_INET) {
        unsigned char uflag;
        impl()->get_option(IPPROTO_IP, IP_MULTICAST_LOOP, uflag);
        flag = uflag != 0;
    }
    else {
#ifdef NET_HAVE_IPv6
        unsigned uflag;
        impl()->get_option(IPPROTO_IPV6, IPV6_MULTICAST_LOOP, uflag);
        flag = uflag != 0;
#endif
    }
    return flag;
}

void MulticastSocket::set_ttl(unsigned char value) {
    if (get_address().af() == AF_INET) {
        impl()->set_option(IPPROTO_IP, IP_MULTICAST_TTL, value);
    }
    else {
#ifdef NET_HAVE_IPv6
        impl()->set_option(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, value);
#endif
    }
}

unsigned char MulticastSocket::get_ttl() const {
    unsigned char ttl(0);
    if (get_address().af() == AF_INET) {
        impl()->get_option(IPPROTO_IP, IP_MULTICAST_TTL, ttl);
    }
    else {
#ifdef NET_HAVE_IPv6
        impl()->get_option(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
#endif
    }
    return ttl;
}

void MulticastSocket::join_group(const IpAddress& group_address, const NetworkInterface& interface) {
    if (group_address.get_native_address_family() == AF_INET) {
        struct ip_mreqn mr;
        mr.imr_multiaddr = group_address.ipv4_address();
        mr.imr_address.s_addr = 0;
        mr.imr_ifindex = interface.get_index();
        impl()->set_raw_option(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mr, sizeof(mr));
    }
    else {
#ifdef NET_HAVE_IPv6
        struct ipv6_mreq mr;
        mr.ipv6mr_multiaddr = group_address.ipv6_address();
        mr.ipv6mr_interface = interface.get_index();
        impl()->set_raw_option(IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(mr));
#endif
    }
}

void MulticastSocket::leave_group(const IpAddress& group_address, const NetworkInterface& interface) {
    if (group_address.get_native_address_family() == AF_INET) {
        struct ip_mreqn mr;
        mr.imr_multiaddr = group_address.ipv4_address();
        mr.imr_address.s_addr = 0;
        mr.imr_ifindex = interface.get_index();
        impl()->set_raw_option(IPPROTO_IP, IP_DROP_MEMBERSHIP, &mr, sizeof(mr));
    }
    else {
#ifdef NET_HAVE_IPv6
        struct ipv6_mreq mr;
        mr.ipv6mr_multiaddr = group_address.ipv6_address();
        mr.ipv6mr_interface = interface.get_index();
        impl()->set_raw_option(IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, &mr, sizeof(mr));
#endif
    }
}

} // namespace net
