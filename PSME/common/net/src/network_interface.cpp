//
// NetworkInterface.cpp
//
// $Id: //poco/1.4/Net/src/NetworkInterface.cpp#9 $
//
// Library: Net
// Package: NetCore
// Module:  NetworkInterface
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
 * @file network_interface.cpp
 *
 * @brief NetworkInterface interface
 * */

#include "net/network_interface.hpp"
#include "net/datagram_socket.hpp"
#include "net/socket_impl.hpp"

#include <safe-string/safe_lib.hpp>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
extern "C" {
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>
}

std::ostream& operator<<(std::ostream& os, const net::NetworkInterface::HWAddress& v) {
    std::ios state(0);
    state.copyfmt(os);
    for (net::NetworkInterface::HWAddress::size_type i = 0; i < v.size(); ++i) {
        if (i > 0) {
            os << net::NetworkInterface::MAC_SEPARATOR;
        }
        os << std::hex << std::setw(2) << std::setfill('0') << unsigned(v[i]);
    }
    os.copyfmt(state);
    return os;
}

namespace net {

class NetworkInterfaceImpl final {
public:
    /*! @brief Index value of not initialized network interface */
    static constexpr unsigned NO_INDEX = -1;

    using AddressTuple = NetworkInterface::AddressTuple;
    using AddressList = NetworkInterface::AddressList;
    using Type = NetworkInterface::Type;
    using AddressType = NetworkInterface::AddressType;

    unsigned get_index() const;
    const std::string& get_name() const;
    const IpAddress& get_first_address(AddressFamily family) const;
    void add_address(const AddressTuple& rAddress);
    const IpAddress& get_address(AddressList::size_type index) const;
    const NetworkInterface::AddressList& get_address_list() const;
    bool has_address(const IpAddress& rAddress) const;
    const IpAddress& get_subnet_mask(AddressList::size_type index) const;
    const IpAddress& get_broadcast_address(AddressList::size_type index) const;
    const IpAddress& get_dest_address(AddressList::size_type index) const;
    const NetworkInterface::HWAddress& get_hw_address() const;
    bool supports_ipv4() const;
    bool supports_ipv6() const;

    void set_name(const std::string& rName);
    void set_hw_address(const NetworkInterface::HWAddress& addr);
    void set_hw_address(const void *addr, std::size_t len);

    unsigned get_mtu() const;
    Type get_type() const;

    bool is_broadcast() const;
    bool is_loopback() const;
    bool is_multicast() const;
    bool is_p2p() const;
    bool is_running() const;
    bool is_up() const;

    void set_flags(unsigned flags);
    void set_up(bool up);
    void set_mtu(unsigned mtu);
    void set_type(Type type);
    void set_index(unsigned index);

private:
    std::string m_name{};
    AddressList m_address_list{};
    unsigned m_index{NO_INDEX};
    bool m_broadcast{false};
    bool m_loopback{false};
    bool m_multicast{false};
    bool m_p2p{false};
    bool m_up{false};
    bool m_running{false};
    unsigned m_mtu{0};
    Type m_type{Type::OTHER};

    NetworkInterface::HWAddress m_hwaddr{};
};

bool NetworkInterfaceImpl::supports_ipv4() const {
    for (const auto& addr : m_address_list) {
        if (AddressFamily::IPv4 == std::get<NetworkInterface::IP_ADDRESS>(addr).get_address_family()) {
            return true;
        }
    }
    return false;
}

bool NetworkInterfaceImpl::supports_ipv6() const {
#ifdef NET_HAVE_IPv6
    for (const auto& addr : m_address_list) {
        if (AddressFamily::IPv6 == std::get<NetworkInterface::IP_ADDRESS>(addr).get_address_family()) {
            return true;
        }
    }
    return false;
#else
    return false;
#endif
}

unsigned NetworkInterfaceImpl::get_index() const {
    return m_index;
}

const std::string& NetworkInterfaceImpl::get_name() const {
    return m_name;
}

const IpAddress& NetworkInterfaceImpl::get_first_address(AddressFamily family) const {
    for (const auto& address_tuple : m_address_list) {
        const auto& addr = std::get<NetworkInterface::IP_ADDRESS>(address_tuple);
        if (addr.get_address_family() == family) {
            return addr;
        }
    }
    throw NetException("IpAddress of NetworkInterface not found");
}

void NetworkInterfaceImpl::add_address(const AddressTuple& address) {
    m_address_list.emplace_back(address);
}

bool NetworkInterfaceImpl::has_address(const IpAddress& rAddress) const {
    for (const auto& address_tuple : m_address_list) {
        if (rAddress == std::get<NetworkInterface::IP_ADDRESS>(address_tuple)) {
            return true;
        }
    }
    return false;
}

const IpAddress& NetworkInterfaceImpl::get_address(AddressList::size_type address_index) const {
    if (address_index < m_address_list.size()) {
        return std::get<NetworkInterface::IP_ADDRESS>(m_address_list[address_index]);
    }
    throw NetException("IpAddress of NetworkInterface not found");
}

const NetworkInterface::AddressList& NetworkInterfaceImpl::get_address_list() const {
    return m_address_list;
}

const IpAddress& NetworkInterfaceImpl::get_subnet_mask(AddressList::size_type index) const {
    if (index < m_address_list.size()) {
        return std::get<NetworkInterface::SUBNET_MASK>(m_address_list[index]);
    }
    throw NetException("IpAddress of NetworkInterface not found");
}

const IpAddress& NetworkInterfaceImpl::get_broadcast_address(AddressList::size_type index) const {
    if (index < m_address_list.size()) {
        return std::get<NetworkInterface::BROADCAST_ADDRESS>(m_address_list[index]);
    }
    throw NetException("IpAddress of NetworkInterface not found");
}

const IpAddress& NetworkInterfaceImpl::get_dest_address(AddressList::size_type index) const {
    if (!is_p2p()) {
        throw NetException("Unsupported operation");
    } else if (index < m_address_list.size()) {
        return std::get<NetworkInterface::BROADCAST_ADDRESS>(m_address_list[index]);
    }
    throw NetException("IpAddress of NetworkInterface not found");
}

const NetworkInterface::HWAddress& NetworkInterfaceImpl::get_hw_address() const {
    return m_hwaddr;
}

unsigned NetworkInterfaceImpl::get_mtu() const {
    return m_mtu;
}

NetworkInterface::Type NetworkInterfaceImpl::get_type() const {
    return m_type;
}

bool NetworkInterfaceImpl::is_broadcast() const {
    return m_broadcast;
}

bool NetworkInterfaceImpl::is_loopback() const {
    return m_loopback;
}

bool NetworkInterfaceImpl::is_multicast() const {
    return m_multicast;
}

bool NetworkInterfaceImpl::is_p2p() const {
    return m_p2p;
}

bool NetworkInterfaceImpl::is_running() const {
    return m_running;
}

bool NetworkInterfaceImpl::is_up() const {
    return m_up;
}

void NetworkInterfaceImpl::set_flags(unsigned flags) {
    m_broadcast = (flags & IFF_BROADCAST);
    m_loopback = (flags & IFF_LOOPBACK);
    m_multicast = (flags & IFF_MULTICAST);
    m_p2p = (flags & IFF_POINTOPOINT);
    m_running = (flags & IFF_RUNNING);
    m_up = (flags & IFF_UP);
}

void NetworkInterfaceImpl::set_up(bool is_up_flag) {
    m_up = is_up_flag;
}

void NetworkInterfaceImpl::set_mtu(unsigned mtu) {
    m_mtu = mtu;
}

void NetworkInterfaceImpl::set_type(Type type) {
    m_type = type;
}

void NetworkInterfaceImpl::set_index(unsigned index) {
    m_index = index;
}

void NetworkInterfaceImpl::set_name(const std::string& name) {
    m_name = name;
}

void NetworkInterfaceImpl::set_hw_address(const NetworkInterface::HWAddress& addr) {
    m_hwaddr = addr;
}

void NetworkInterfaceImpl::set_hw_address(const void* addr, std::size_t len) {
    m_hwaddr.clear();
    m_hwaddr.insert(m_hwaddr.end(),
                    static_cast<const unsigned char*>(addr),
                    static_cast<const unsigned char*>(addr) + len);
}

NetworkInterface::NetworkInterface() : m_impl{new NetworkInterfaceImpl()} {}

NetworkInterface::NetworkInterface(std::shared_ptr<NetworkInterfaceImpl> impl)
    : m_impl(impl) { }

NetworkInterface NetworkInterface::for_name(const std::string& name) {
    auto map = NetworkInterface::get_interfaces_map();

    for (const auto& nic: map) {
        if (nic.second.get_name() == name) {
            return nic.second;
        }
    }
    throw NetException("NetworkInterface '" + name + "' not found");
}

NetworkInterface NetworkInterface::for_address(const IpAddress& addr) {
    auto map = NetworkInterface::get_interfaces_map();

    for (const auto& nic : map) {
        if (nic.second.supports_ip()) {
            auto size = nic.second.get_address_list().size();
            for (AddressList::size_type i = 0; i < size; ++i) {
                if (nic.second.get_address(i) == addr) {
                    return nic.second;
                }
            }
        }
    }
    throw NetException("NetworkInterface for address " + addr.to_string() + " not found");
}

NetworkInterface NetworkInterface::for_index(unsigned i) {
    if (i != NetworkInterfaceImpl::NO_INDEX) {
        Map map = NetworkInterface::get_interfaces_map();
        auto it = map.find(i);
        if (it != map.end()) {
            return it->second;
        }
    }
    throw NetException("NetworkInterface for index " + std::to_string(i) + " not found");
}

unsigned NetworkInterface::get_index() const {
    return m_impl->get_index();
}

const std::string& NetworkInterface::get_name() const {
    return m_impl->get_name();
}

const IpAddress& NetworkInterface::get_first_address(AddressFamily family) const {
    return m_impl->get_first_address(family);
}

const NetworkInterface::AddressList& NetworkInterface::get_address_list() const {
    return m_impl->get_address_list();
}

bool NetworkInterface::has_address(const IpAddress& address) const {
    return m_impl->has_address(address);
}

const IpAddress& NetworkInterface::get_address(AddressList::size_type index) const {
    return m_impl->get_address(index);
}

const IpAddress& NetworkInterface::get_subnet_mask(AddressList::size_type index) const {
    return m_impl->get_subnet_mask(index);
}

const IpAddress& NetworkInterface::get_broadcast_address(AddressList::size_type index) const {
    return m_impl->get_broadcast_address(index);
}

const IpAddress& NetworkInterface::get_dest_address(AddressList::size_type index) const {
    return m_impl->get_dest_address(index);
}

const NetworkInterface::HWAddress& NetworkInterface::get_hw_address() const {
    return m_impl->get_hw_address();
}

unsigned NetworkInterface::get_mtu() const {
    return m_impl->get_mtu();
}

NetworkInterface::Type NetworkInterface::get_type() const {
    return m_impl->get_type();
}

bool NetworkInterface::supports_ip() const {
    return m_impl->supports_ipv4() || m_impl->supports_ipv6();
}

bool NetworkInterface::supports_ipv4() const {
    return m_impl->supports_ipv4();
}

bool NetworkInterface::supports_ipv6() const {
    return m_impl->supports_ipv6();
}

bool NetworkInterface::supports_broadcast() const {
    return m_impl->is_broadcast();
}

bool NetworkInterface::supports_multicast() const {
    return m_impl->is_multicast();
}

bool NetworkInterface::is_loopback() const {
    return m_impl->is_loopback();
}

bool NetworkInterface::is_p2p() const {
    return m_impl->is_p2p();
}

bool NetworkInterface::is_running() const {
    return m_impl->is_running();
}

bool NetworkInterface::is_up() const {
    return m_impl->is_up();
}

NetworkInterface::List NetworkInterface::get_interfaces() {
    List result{};
    Map map = get_interfaces_map();
    for (const auto& nic: map) {
        result.emplace_back(nic.second);
    }
    return result;
}

namespace {

NetworkInterface::Type from_arp_hw_type(unsigned arp_hw_type) {
    switch (arp_hw_type) {
        case ARPHRD_ETHER: return NetworkInterface::Type::ETHERNET_CSMACD;
        case ARPHRD_IEEE802: return NetworkInterface::Type::ISO88025_TOKENRING;
        case ARPHRD_DLCI: return NetworkInterface::Type::FRAMERELAY;
        case ARPHRD_PPP: return NetworkInterface::Type::PPP;
        case ARPHRD_LOOPBACK: return NetworkInterface::Type::SOFTWARE_LOOPBACK;
        case ARPHRD_ATM: return NetworkInterface::Type::ATM;
        case ARPHRD_IEEE80211: return NetworkInterface::Type::IEEE80211;
        case ARPHRD_TUNNEL:
        case ARPHRD_TUNNEL6: return NetworkInterface::Type::TUNNEL;
        case ARPHRD_IEEE1394: return NetworkInterface::Type::IEEE1394;
        default: return NetworkInterface::Type::OTHER;
    }
}

unsigned get_mtu(const std::string& if_name) {
    struct ifreq ifr;
    if (EOK != ::strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), if_name.c_str(), IFNAMSIZ)) {
        throw NetException("Failed to initialize ifreq.");
    }
    DatagramSocket ds(AddressFamily::IPv4);
    ds.impl()->ioctl(SIOCGIFMTU, &ifr);
    return ifr.ifr_mtu;
}

std::unique_ptr<NetworkInterfaceImpl> iface_from_ifaddrs(const unsigned index,
        const ifaddrs* iface) {
    std::unique_ptr<NetworkInterfaceImpl> intf{new NetworkInterfaceImpl()};
    intf->set_index(index);
    intf->set_name(iface->ifa_name);
    intf->set_flags(iface->ifa_flags);
    intf->set_mtu(get_mtu(intf->get_name()));

    if (AF_PACKET == iface->ifa_addr->sa_family) {
        struct sockaddr_ll* sll = reinterpret_cast<struct sockaddr_ll*>(iface->ifa_addr);
        intf->set_hw_address(sll->sll_addr, sll->sll_halen);
        intf->set_type(from_arp_hw_type(sll->sll_hatype));
    }
    return intf;
}

NetworkInterface::AddressTuple address_from_ifaddrs(const ifaddrs* piface) {
    NetworkInterface::AddressTuple address;
    if (AF_INET == piface->ifa_addr->sa_family
#ifdef NET_HAVE_IPv6
        || AF_INET6 == piface->ifa_addr->sa_family
#endif
        ) {

        std::get<NetworkInterface::IP_ADDRESS>(address)
                = IpAddress::from_sockaddr(*(piface->ifa_addr));
        std::get<NetworkInterface::SUBNET_MASK>(address)
                = IpAddress::from_sockaddr(*(piface->ifa_netmask));

        if (piface->ifa_flags & IFF_BROADCAST && piface->ifa_broadaddr) {
            std::get<NetworkInterface::BROADCAST_ADDRESS>(address)
                    = IpAddress::from_sockaddr(*(piface->ifa_broadaddr));
        }
        else if (piface->ifa_flags & IFF_POINTOPOINT && piface->ifa_dstaddr) {
            std::get<NetworkInterface::BROADCAST_ADDRESS>(address)
                    = IpAddress::from_sockaddr(*(piface->ifa_dstaddr));
        }
    }
    return address;
}

using IfAddrs = std::unique_ptr<struct ifaddrs, decltype(&freeifaddrs)>;

IfAddrs get_ifaddrs() {
    struct ifaddrs* p_ifaddrs = nullptr;
    if (::getifaddrs(&p_ifaddrs) < 0) {
        throw NetException("Cannot retrieve network interface list");
    }
    return IfAddrs(p_ifaddrs, &freeifaddrs);
}

}

NetworkInterface::Map NetworkInterface::get_interfaces_map() {
    Map result{};
    auto ifaces = get_ifaddrs();

    for (auto* ifa = ifaces.get(); ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        const auto family = ifa->ifa_addr->sa_family;

        if (AF_PACKET == family || AF_INET == family
#ifdef NET_HAVE_IPv6
            || AF_INET6 == family
#endif
            ) {
            const auto index = ::if_nametoindex(ifa->ifa_name);
            auto it = result.find(index);
            if (it == result.end()) {
                it = result.emplace_hint(it, index,
                        NetworkInterface(iface_from_ifaddrs(index, ifa)));
            }

            if (AF_PACKET == family) {
                continue;
            }

            if (it != result.end()) {
                it->second.m_impl->add_address(address_from_ifaddrs(ifa));
            }
        }
    }
    return result;
}

} // namespace net
