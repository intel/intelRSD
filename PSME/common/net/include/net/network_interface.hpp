//
// NetworkInterface.h
//
// $Id: //poco/1.4/Net/include/Poco/Net/NetworkInterface.h#4 $
//
// Library: Net
// Package: Sockets
// Module:  NetworkInterface
//
// Definition of the NetworkInterface class.
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
 * @file network_interface.hpp
 *
 * @brief NetworkInterface interface
 * */

#pragma once

#include "ipaddress.hpp"

#include <memory>
#include <vector>
#include <tuple>
#include <map>
#include <ostream>


namespace net {

class NetworkInterfaceImpl;

/*!
 * @brief Represents a network interface.
 *
 * Provides static member functions for network interface enumeration.
 * NetworkInterface is used by MulticastSocket to specify interfaces
 * for multicast traffic.
 */
class NetworkInterface final {
public:
    using List = std::vector<NetworkInterface>;
    using Map = std::map<unsigned, NetworkInterface>;
    using AddressTuple = std::tuple<IpAddress, IpAddress, IpAddress>;
    using AddressList = std::vector<AddressTuple>;
    using AddressIterator = AddressList::iterator;
    using ConstAddressIterator = AddressList::const_iterator;
    using HWAddress = std::vector<unsigned char>;

    /*! @brief Enumeration of address types present in AddressTuple */
    enum AddressType {
        IP_ADDRESS,
        SUBNET_MASK,
        BROADCAST_ADDRESS
    };

    /*! @brief NetworkInterface types enumeration */
    enum class Type {
        ETHERNET_CSMACD,
        ISO88025_TOKENRING,
        FRAMERELAY,
        PPP,
        SOFTWARE_LOOPBACK,
        ATM,
        IEEE80211,
        TUNNEL,
        IEEE1394,
        OTHER
    };

    /*! @brief Separator used in string representation of MAC address */
    static constexpr char MAC_SEPARATOR = ':';

    /*!
     * @brief Searches for the network interface with the specified name.
     *
     * @param name Name of network interface.
     * @return Found NetworkInterface on success. Throws error if not found.
     */
    static NetworkInterface for_name(const std::string& name);

    /*!
     * @brief Searches for the network interface with the given address.
     *
     * @param address Address of the network interface.
     * @return Found NetworkInterface on success. Throws error if not found.
     */
    static NetworkInterface for_address(const IpAddress& address);

    /*!
     * @brief Searches for the network interface for the given interface index.
     *
     * @param index Network interface index.
     * @return Found NetworkInterface on success. Throws error if not found.
     */
    static NetworkInterface for_index(unsigned index);

    /*!
     * @brief Enumerates system network interfaces and returns them in a map.
     * Key of the map is system index of network interface.
     *
     * @return A map containing system network interfaces.
     */
    static Map get_interfaces_map();

    /*!
     * @brief Returns list of system network interfaces.
     *
     * @return A list containing system network interfaces.
     */
    static List get_interfaces();

    /*! Default constructor */
    NetworkInterface();

    /*!
     * @brief Less-than operator
     * Compares interface indices.
     */
    bool operator<(const NetworkInterface& other) const {
        return this->get_index() < other.get_index();
    }

    /*!
     * @brief Equal operator
     * Compares interface indices.
     */
    bool operator==(const NetworkInterface& other) const {
        return this->get_index() == other.get_index();
    }

    /*!
     * @return The interface OS index.
     */
    unsigned get_index() const;

    /*!
     * @return The interface name.
     */
    const std::string& get_name() const;

    /*!
     * @brief Gets the first IP address of this interface.
     *
     * @param family Address family.
     * @return The IP address of this interface. Throws error if not found.
     */
    const IpAddress& get_first_address(AddressFamily family) const;

    /*!
     * Tests presence of given IP address.
     *
     * @param address IpAddress checked for presence.
     * @return true if given IpAddress is present, false otherwise.
     */
    bool has_address(const IpAddress& address) const;

    /*!
     * @brief Gets IP address of this interface.
     *
     * @param index The index in address list of this interface.
     * @return The IP address of this interface at index position.
     */
    const IpAddress& get_address(AddressList::size_type index = 0) const;

    /*!
     * @brief Gets address list assigned to the interface.
     * @return The list of IP addresses bound to the interface.
     */
    const AddressList& get_address_list() const;

    /*!
     * @brief Gets subnet mask of this interface.
     *
     * @param index The index in address list of this interface.
     * @return The subnet mask of this interface.
     */
    const IpAddress& get_subnet_mask(AddressList::size_type index = 0) const;

    /*!
     * @brief Gets broadcast address of this interface.
     *
     * @param index The index in address list of this interface.
     * @return The broadcast address of this interface.
     */
    const IpAddress& get_broadcast_address(AddressList::size_type index = 0) const;

    /*!
     * @brief Gets destination address of p2p interface.
     *
     * @param index The index in address list of this interface.
     * @return The destination address of p2p interface.
     */
    const IpAddress& get_dest_address(AddressList::size_type index = 0) const;

    /*!
     * @brief Gets hardware address (usually MAC) of the interface.
     *
     * @return The hardware address (usually MAC) of the interface.
     */
    const HWAddress& get_hw_address() const;

    /*!
     * @brief Gets MTU (Maximum Transmission Unit) of this interface.
     *
     * @return MTU of this interface.
     */
    unsigned get_mtu() const;

    /*!
     * @brief Gets type of the interface.
     *
     * @return MIB IfType of the interface.
     */
    NetworkInterface::Type get_type() const;

    /*!
     * @brief Returns whether a network interface supports IP (IPv4 or IPv6).
     *
     * @return true if the interface supports IP (IPv4 or IPv6).
     */
    bool supports_ip() const;

    /*!
     * @brief Returns whether a network interface supports IPv4.
     *
     * @return true if the interface supports IPv4.
     */
    bool supports_ipv4() const;

    /*!
     * @brief Returns whether a network interface supports IPv6.
     *
     * @return true if the interface supports IPv6.
     */
    bool supports_ipv6() const;

    /*!
     * @brief Returns whether a network interface supports broadcast.
     *
     * @return true if the interface supports broadcast.
     */
    bool supports_broadcast() const;

    /*!
     * @brief Returns whether a network interface supports multicast.
     *
     * @return true if the interface supports multicast.
     */
    bool supports_multicast() const;

    /*!
     * @brief Returns whether a network interface is a loopback.
     *
     * @return true if the interface is loopback.
     */
    bool is_loopback() const;

    /*!
     * @brief Returns whether a network interface is a point to point interface.
     *
     * @return true if the interface is point to point interface.
     */
    bool is_p2p() const;

    /*!
     * @brief Returns whether a network interface is running.
     *
     * @return true if the interface is running.
     */
    bool is_running() const;

    /*!
     * @brief Returns whether a network interface is up.
     *
     * @return true if the interface is up.
     */
    bool is_up() const;

private:
    NetworkInterface(std::shared_ptr<NetworkInterfaceImpl> impl);

    std::shared_ptr<NetworkInterfaceImpl> m_impl;
};

} // namespace net

std::ostream& operator<<(std::ostream& os, const net::NetworkInterface::HWAddress& v);
