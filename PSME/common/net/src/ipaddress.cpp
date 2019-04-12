//
// IPAddress.cpp
//
// $Id: //poco/1.4/Net/src/IPAddress.cpp#5 $
//
// Library: Net
// Package: NetCore
// Module:  IPAddress
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
 * @file ipaddress.cpp
 *
 * @brief IPAddress interface
 * */

#include "net/ipaddress.hpp"

#include <safe-string/safe_lib.hpp>
#include <cstring>

namespace net {

namespace {
/*
 * Returns the length of the mask (number of bits set in val).
 * The val should be either all zeros or two contiguos areas of 1s and 0s.
 * The algorithm ignores invalid non-contiguous series of 1s and treats val
 * as if all bits between most significant and last non-zero bit are set to 1.
 **/
template <typename T>
unsigned mask_bits(T val, unsigned size) {
    unsigned count = 0;
    if (val) {
        val = (val ^ (val - 1)) >> 1;
        for (count = 0; val; ++count) {
            val >>= 1;
        }
    }
    else {
        count = size;
    }
    return size - count;
}

constexpr const std::uint32_t ALL_ONES = -1;
}

IpAddress::IpAddress() {
    ::memset_s(&m_addr, sizeof(m_addr), 0);
}

IpAddress::IpAddress(const IpAddress& other) : m_family(other.m_family) {
    ::memcpy_s(&m_addr, sizeof(m_addr), &other.m_addr, sizeof(other.m_addr));
}

IpAddress::IpAddress(const in_addr& ipv4) {
    ::memset_s(&m_addr, sizeof(m_addr), 0);
    m_addr.ipv4 = ipv4;
}

#ifdef NET_HAVE_IPv6
IpAddress::IpAddress(const in6_addr& ipv6) : m_family{AddressFamily::IPv6} {
    m_family = AddressFamily::IPv6;
    m_addr.ipv6 = ipv6;
}
#endif

IpAddress::~IpAddress() { }

int IpAddress::get_native_address_family() const {
#ifdef NET_HAVE_IPv6
    if (AddressFamily::IPv6 == m_family) {
        return AF_INET6;
    }
#endif
    return AF_INET;
}

net_socklen_t IpAddress::length() const {
    switch (m_family) {
        case AddressFamily::IPv4:
            return sizeof(in_addr);
#ifdef NET_HAVE_IPv6
        case AddressFamily::IPv6:
            return sizeof(in6_addr);
#endif
        case AddressFamily::UNIX_LOCAL:
        default:
            break;
    }
    throw NetException("Invalid address");
}

IpAddress& IpAddress::operator=(const IpAddress& other) {
    m_family = other.m_family;
    ::memcpy_s(&m_addr, sizeof(m_addr), &other.m_addr, sizeof(other.m_addr));
    return *this;
}

bool IpAddress::operator==(const IpAddress &other) const {
    if (m_family != other.m_family) {
        return false;
    }
    int diff{0};
    if (AddressFamily::IPv4 == m_family) {
        if (0 == ::memcmp_s(&m_addr.ipv4, sizeof(m_addr.ipv4), &other.m_addr.ipv4,
                            sizeof(other.m_addr.ipv4), &diff)) {
            return 0 == diff;
        }
    }
#ifdef NET_HAVE_IPv6
    if (AddressFamily::IPv6 == m_family) {
        if (0 == ::memcmp_s(&m_addr.ipv6, sizeof(m_addr.ipv6), &other.m_addr.ipv6,
                            sizeof(other.m_addr.ipv6), &diff)) {
            return 0 == diff;
        }
    }
#endif
    return false;
}

bool IpAddress::operator!=(const IpAddress &other) const {
    return !((*this) == other);
}

bool IpAddress::operator>(const IpAddress &other) const {
    return (*this) != other && !((*this) < other);
}

bool IpAddress::operator<(const IpAddress &other) const {
    // IPv4 is less than IPv6
#ifdef NET_HAVE_IPv6
    if (m_family != other.m_family) {
        if (AddressFamily::IPv4 == m_family
            && AddressFamily::IPv6 == other.m_family) {
            return true;
        }
        return false;
    }
#endif
    switch (m_family) {
        case AddressFamily::IPv4:
        {
            return (ntohl)(m_addr.ipv4.s_addr) < (ntohl)(other.m_addr.ipv4.s_addr);
        }
#ifdef NET_HAVE_IPv6
        case AddressFamily::IPv6:
        {
            int diff{0};
            if (0 == ::memcmp_s(&m_addr.ipv6, sizeof(m_addr.ipv6), &other.m_addr.ipv6,
                                sizeof(other.m_addr.ipv6), &diff)) {
                return diff < 0;
            }
            break;
        }
#endif
        case AddressFamily::UNIX_LOCAL:
        default:
            break;
    }
    return false;
}

IpAddress IpAddress::operator&(const IpAddress& other) const {
    if (get_address_family() == other.get_address_family()) {
        if (AddressFamily::IPv4 == get_address_family()) {
            auto result = ipv4_address();
            result.s_addr &= other.ipv4_address().s_addr;
            return IpAddress(result);
        }
#ifdef NET_HAVE_IPv6
        else if (AddressFamily::IPv6 == get_address_family()) {
            auto result = ipv6_address();
            result.s6_addr32[0] &= other.ipv6_address().s6_addr32[0];
            result.s6_addr32[1] &= other.ipv6_address().s6_addr32[1];
            result.s6_addr32[2] &= other.ipv6_address().s6_addr32[2];
            result.s6_addr32[3] &= other.ipv6_address().s6_addr32[3];
            return IpAddress(result);
        }
#endif
    }
    throw NetException("Incompatible addresses.");
}

IpAddress IpAddress::operator|(const IpAddress& other) const {
    if (get_address_family() == other.get_address_family()) {
        if (AddressFamily::IPv4 == get_address_family()) {
            auto result = ipv4_address();
            result.s_addr |= other.ipv4_address().s_addr;
            return IpAddress(result);
        }
#ifdef NET_HAVE_IPv6
        else if (AddressFamily::IPv6 == get_address_family()) {
            auto result = ipv6_address();
            result.s6_addr32[0] |= other.ipv6_address().s6_addr32[0];
            result.s6_addr32[1] |= other.ipv6_address().s6_addr32[1];
            result.s6_addr32[2] |= other.ipv6_address().s6_addr32[2];
            result.s6_addr32[3] |= other.ipv6_address().s6_addr32[3];
            return IpAddress(result);
        }
#endif
    }
    throw NetException("Incompatible addresses.");
}

IpAddress IpAddress::operator ^ (const IpAddress& other) const {
    if (get_address_family() == other.get_address_family()) {
        if (AddressFamily::IPv4 == get_address_family()) {
            auto result = ipv4_address();
            result.s_addr ^= other.ipv4_address().s_addr;
            return IpAddress(result);
        }
#ifdef NET_HAVE_IPv6
        else if (AddressFamily::IPv6 == get_address_family()) {
            auto result = ipv6_address();
            result.s6_addr32[0] ^= other.ipv6_address().s6_addr32[0];
            result.s6_addr32[1] ^= other.ipv6_address().s6_addr32[1];
            result.s6_addr32[2] ^= other.ipv6_address().s6_addr32[2];
            result.s6_addr32[3] ^= other.ipv6_address().s6_addr32[3];
            return IpAddress(result);
        }
#endif
    }
    throw NetException("Incompatible addresses.");
}

IpAddress IpAddress::operator~() const {
    if (get_address_family() == AddressFamily::IPv4) {
        auto result = ipv4_address();
        result.s_addr ^= ALL_ONES;
        return IpAddress(result);
    }
#ifdef NET_HAVE_IPv6
    else if (get_address_family() == AddressFamily::IPv6) {
        auto result = ipv6_address();
        result.s6_addr32[0] ^= ALL_ONES;
        result.s6_addr32[1] ^= ALL_ONES;
        result.s6_addr32[2] ^= ALL_ONES;
        result.s6_addr32[3] ^= ALL_ONES;
        return IpAddress(result);
    }
#endif
    throw NetException("Invalid address");
}

unsigned IpAddress::prefix_length() const {
    if (get_address_family() == AddressFamily::IPv4) {
        return mask_bits((ntohl)(ipv4_address().s_addr), 32);
    }
#ifdef NET_HAVE_IPv6
    else if (get_address_family() == AddressFamily::IPv6) {
        unsigned bits = 0;
        unsigned bit_pos = 128;
        for (int i = 3; i >= 0; --i) {
            unsigned address = (ntohl)(m_addr.ipv6.s6_addr32[i]);
            if ((bits = mask_bits(address, 32))) {
                return (bit_pos - (32 - bits));
            }
            bit_pos -= 32;
        }
        return 0;
    }
#endif
    throw NetException("Invalid address");
}

bool IpAddress::is_any_address() const {
    if (get_address_family() == AddressFamily::IPv4) {
        return ipv4_address().s_addr == 0;
    }
#ifdef NET_HAVE_IPv6
    else if (get_address_family() == AddressFamily::IPv6) {
        return *this == IpAddress(in6addr_any);
    }
#endif
    throw NetException("Invalid address");
}

const void* IpAddress::addr() const {
    return &m_addr;
}

in_addr IpAddress::ipv4_address() const {
    return m_addr.ipv4;
}

#ifdef NET_HAVE_IPv6
in6_addr IpAddress::ipv6_address() const {
    return m_addr.ipv6;
}
#endif

std::string IpAddress::to_string() const {
    if (AddressFamily::IPv4 != m_family
#ifdef NET_HAVE_IPv6
        && AddressFamily::IPv6 != m_family
#endif
       ) {
        return std::string();
    }
#ifdef NET_HAVE_IPv6
    char buf[INET6_ADDRSTRLEN] = {0};
#else
    char buf[INET_ADDRSTRLEN] = {0};
#endif
    if (::inet_ntop(get_native_address_family(), addr(), buf, sizeof(buf))) {
        return std::string(buf);
    }
    return std::string();
}

IpAddress IpAddress::from_sockaddr(const struct sockaddr& sockaddr) {
    const auto address_family = sockaddr.sa_family;
    if (AF_INET == address_family) {
        return IpAddress(reinterpret_cast<const struct sockaddr_in*>(&sockaddr)->sin_addr);
    }
#ifdef NET_HAVE_IPv6
    else if (AF_INET6 == address_family) {
        return IpAddress(reinterpret_cast<const struct sockaddr_in6*>(&sockaddr)->sin6_addr);
    }
#endif
    else throw NetException("Invalid address");
}

IpAddress IpAddress::from_string(const std::string& address) {
    in_addr addr;
    if (inet_pton(AF_INET, address.c_str(), &addr)) {
        return IpAddress(addr);
    }
#ifdef NET_HAVE_IPv6
    else {
        in6_addr addr6;
        if (inet_pton(AF_INET6, address.c_str(), &addr6)) {
            return IpAddress(addr6);
        }
    }
#endif
    throw NetException("Invalid address");
}

} // namespace net

std::ostream& operator<<(std::ostream& ostr, const net::IpAddress& addr) {
    ostr << addr.to_string();
    return ostr;
}
