//
// SocketAddress.cpp
//
// $Id: //poco/1.4/Net/src/SocketAddress.cpp#5 $
//
// Library: Net
// Package: NetCore
// Module:  SocketAddress
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
 * @file socket_address.cpp
 *
 * @brief SocketAddress interface
 * */

#include "net/socket_address.hpp"

#include <safe-string/safe_lib.hpp>
#include <algorithm>
#include <cstring>
#include <cstddef> // for offsetof

#include <iostream>

namespace net {

namespace {
bool is_ipv4(const SockaddrStorage& addr) {
    return sizeof(struct sockaddr_in) == addr.addr_len
           && AF_INET == addr.addr->sa_family;
}

#ifdef NET_HAVE_IPv6
bool is_ipv6(const SockaddrStorage& addr) {
    return sizeof(struct sockaddr_in6) == addr.addr_len
           && AF_INET6 == addr.addr->sa_family;
}
#endif

bool is_unix_local(const SockaddrStorage& addr) {
    return AF_UNIX == addr.addr->sa_family;
}

bool is_supported_sockaddr(const SockaddrStorage& addr) {
    return is_ipv4(addr)
#ifdef NET_HAVE_IPv6
            || is_ipv6(addr)
#endif
            || is_unix_local(addr);
}

uint16_t parse_port(const std::string& port_str) {
    try {
        auto port = std::stoul(port_str);
        if (port <= 0xFFFF) {
            return uint16_t(port);
        }
    } catch (...) { /* ignore */ }
    throw NetException("Invalid port number.");
}
}

SockaddrStorage::SockaddrStorage()
    : addr_storage(), addr_len(sizeof(addr_storage)),
      addr(reinterpret_cast<struct sockaddr*>(&addr_storage)) {
    ::memset_s(addr, addr_len, 0);
}

SockaddrStorage::SockaddrStorage(const SockaddrStorage& other)
    : addr_storage(), addr_len(other.addr_len),
      addr(reinterpret_cast<struct sockaddr*>(&addr_storage)) {
    ::memcpy_s(addr, addr_len, other.addr, other.addr_len);
}

SockaddrStorage& SockaddrStorage::operator=(const SockaddrStorage& other) {
    addr_len = other.addr_len;
    ::memcpy_s(addr, addr_len, other.addr, other.addr_len);
    return *this;
}

SocketAddress::SocketAddress() {
    init(IpAddress(), 0);
}

SocketAddress::SocketAddress(const IpAddress& address, std::uint16_t port) {
    init(address, port);
}

SocketAddress::SocketAddress(std::uint16_t port)
    : SocketAddress(IpAddress(), port) { }

SocketAddress::SocketAddress(const std::string& address, std::uint16_t port)
    : SocketAddress(IpAddress::from_string(address), port) { }

SocketAddress::SocketAddress(const std::string& socket_name) {
    init(socket_name);
}

SocketAddress::SocketAddress(const SockaddrStorage& sockaddr_storage) {
   if (!is_supported_sockaddr(sockaddr_storage)) {
       throw NetException("Unsupported address family.");
   }
   address_storage = sockaddr_storage;
}

bool SocketAddress::operator<(const SocketAddress& socketAddress) const {
    if (family() < socketAddress.family()) {
        return true;
    }
    if (family() > socketAddress.family()) {
        return false;
    }
#if defined(NET_OS_FAMILY_UNIX)
    if (family() == AddressFamily::UNIX_LOCAL) {
        return to_string() < socketAddress.to_string();
    }
#endif
    if (get_host() < socketAddress.get_host()) {
        return true;
    }
    if (get_host() > socketAddress.get_host()) {
        return false;
    }
    return (get_port() < socketAddress.get_port());
}

IpAddress SocketAddress::get_host() const {
    if (is_ipv4(address_storage)) {
        struct sockaddr_in* address = reinterpret_cast<struct sockaddr_in*>(address_storage.addr);
        return IpAddress(address->sin_addr);
    }
#ifdef NET_HAVE_IPv6
    else if (is_ipv6(address_storage)) {
        struct sockaddr_in6* address = reinterpret_cast<struct sockaddr_in6*>(address_storage.addr);
        return IpAddress(address->sin6_addr);
    }
#endif
    throw NetException("Cannot retrieve IP address");
}

std::uint16_t SocketAddress::get_port() const {
    if (is_ipv4(address_storage)) {
        return (ntohs)(reinterpret_cast<struct sockaddr_in*>(address_storage.addr)->sin_port);
    }
#ifdef NET_HAVE_IPv6
    else if (is_ipv6(address_storage)) {
        return (ntohs)(reinterpret_cast<struct sockaddr_in6*>(address_storage.addr)->sin6_port);
    }
#endif
    throw NetException("Cannot retrieve port number");
}

net_socklen_t SocketAddress::length() const {
    return address_storage.addr_len;
}

const struct sockaddr* SocketAddress::addr() const {
    return address_storage.addr;
}

int SocketAddress::af() const {
    return address_storage.addr->sa_family;
}

AddressFamily SocketAddress::family() const {
    if (is_ipv4(address_storage)) {
        return AddressFamily::IPv4;
    }
#ifdef NET_HAVE_IPv6
    else if (is_ipv6(address_storage)) {
        return AddressFamily::IPv6;
    }
#endif
    else if (is_unix_local(address_storage)) {
        return AddressFamily::UNIX_LOCAL;
    }
    throw NetException("Unknown family.");
}

std::string SocketAddress::to_string() const {
    std::string result;
    if (is_ipv4(address_storage)) {
        result.append(get_host().to_string());
        result.append(":");
        result.append(std::to_string(uint(get_port())));
    }
#ifdef NET_HAVE_IPv6
    else if (is_ipv6(address_storage)) {
        result.append("[");
        result.append(get_host().to_string());
        result.append("]");
        result.append(":");
        result.append(std::to_string(uint(get_port())));
    }
#endif
    else if (is_unix_local(address_storage)) {
        result.append(reinterpret_cast<sockaddr_un*>(address_storage.addr)->sun_path);
    }
    return result;
}

void SocketAddress::init(const IpAddress& address, std::uint16_t port) {
    if (AddressFamily::IPv4 == address.get_address_family()) {
        address_storage.addr_len = sizeof(struct sockaddr_in);
        struct sockaddr_in* sock_addr_in = reinterpret_cast<struct sockaddr_in*>(address_storage.addr);
        sock_addr_in->sin_family = AF_INET;
        ::memcpy_s(&(sock_addr_in->sin_addr), sizeof(sock_addr_in->sin_addr), address.addr(), address.length());
        sock_addr_in->sin_port = (htons)(port);
    }
#ifdef NET_HAVE_IPv6
    else if (AddressFamily::IPv6 == address.get_address_family()) {
        address_storage.addr_len = sizeof(struct sockaddr_in6);
        struct sockaddr_in6* sock_addr_in6 = reinterpret_cast<struct sockaddr_in6*>(address_storage.addr);
        sock_addr_in6->sin6_family = AF_INET6;
        ::memcpy_s(&(sock_addr_in6->sin6_addr), sizeof(struct sockaddr_in6), address.addr(), address.length());
        sock_addr_in6->sin6_port = (htons)(port);
    }
#endif
    else {
        throw NetException("Unsupported IP address family.");
    }
}

void SocketAddress::init(const std::string& sock_name) {
    if (sock_name.empty()) {
        throw NetException("Invalid socket name.");
    }

    std::string address;
    std::string port;
    std::string::const_iterator it = sock_name.begin();
    std::string::const_iterator end = sock_name.end();

#if defined(NET_OS_FAMILY_UNIX)
    if ('/' == *it) {
        init_unix_local(sock_name);
        return;
    }
#endif
    if ('[' == *it) {
        ++it;
        while (it != end && ']' != *it) {
            address += *it++;
        }
        if (it == end) {
            throw NetException("Malformed IPv6 address");
        }
        ++it;
    }
    else {
        while (it != end && ':' != *it) {
            address += *it++;
        }
    }
    if (it != end && ':' == *it) {
        ++it;
        while (it != end) port += *it++;
    }
    else {
        throw NetException("Missing port number");
    }
    init(IpAddress::from_string(address), parse_port(port));
}

#if defined(NET_OS_FAMILY_UNIX)
void SocketAddress::init_unix_local(const std::string& path) {
    std::memset(&address_storage.addr_storage, 0, sizeof(struct sockaddr_un));
    struct sockaddr_un* addr_un = reinterpret_cast<struct sockaddr_un*>(address_storage.addr);
    addr_un->sun_family = AF_UNIX;
    if (EOK != ::strncpy_s(addr_un->sun_path, sizeof(addr_un->sun_path), path.c_str(), path.size())) {
        throw NetException("Failed to initialize unix socket address.");
    }
    address_storage.addr_len = net_socklen_t(path.size() + offsetof(struct sockaddr_un, sun_path));
}
#endif

bool SocketAddress::operator==(const SocketAddress& socketAddress) const {
#if defined(NET_OS_FAMILY_UNIX)
    if (family() == AddressFamily::UNIX_LOCAL) {
        return to_string() == socketAddress.to_string();
    }
    else
#endif
        return get_host() == socketAddress.get_host() && get_port() == socketAddress.get_port();
}

bool SocketAddress::operator!=(const SocketAddress& socketAddress) const {
    return !(operator==(socketAddress));
}

} // namespace net

std::ostream& operator<<(std::ostream& ostr, const net::SocketAddress& address) {
    ostr << address.to_string();
    return ostr;
}
