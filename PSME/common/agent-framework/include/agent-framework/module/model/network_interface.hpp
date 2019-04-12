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
 * @brief Network Interface model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"

namespace agent_framework {
namespace model {

/*! Network Interface */
class NetworkInterface : public Resource {
public:
    explicit NetworkInterface(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::System);
    ~NetworkInterface();

    using Ipv6Addresses = attribute::Array<attribute::Ipv6Address>;
    using Ipv4Addresses = attribute::Array<attribute::Ipv4Address>;
    using TransportProtocols = attribute::Array<enums::TransportProtocol>;

    NetworkInterface(const NetworkInterface&) = default;
    NetworkInterface& operator=(const NetworkInterface&) = default;
    NetworkInterface(NetworkInterface&&) = default;
    NetworkInterface& operator=(NetworkInterface&&) = default;

    /*!
     * @brief construct an object of class NetworkInterface from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed NetworkInterface object
     */
    static NetworkInterface from_json(const json::Json& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return NetworkInterface::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return NetworkInterface::component;
    }

    /*!
     * @brief Returns frame size
     * @return Frame size
     * */
    const OptionalField<std::uint32_t>& get_frame_size() const {
        return m_frame_size;
    }

    /*!
     * @brief Set frame size
     * @param[in] frame_size frame size
     * */
    void set_frame_size(const OptionalField<std::uint32_t>& frame_size) {
        m_frame_size = frame_size;
    }

    /*!
     * @brief return default_vlan of type std::uint32_t
     *
     * @return the default_vlan value
     */
    const OptionalField<std::uint32_t>& get_default_vlan() const {
        return m_default_vlan;
    }

    /*!
     * @brief setter for default_vlan attribute
     *
     * @param default_vlan of type std::uint32_t
     */
    void set_default_vlan(const OptionalField<std::uint32_t>& default_vlan) {
        m_default_vlan = default_vlan;
    }

    /*!
     * @brief Returns speed in Mbps
     * @return Speed in Mbps
     * */
    const OptionalField<std::uint32_t>& get_speed_mbps() const {
        return m_speed_mbps;
    }

    /*!
     * @brief Set speed in Mbps
     * @param[in] speed_mbps Speed in Mbps
     * */
    void set_speed_mbps(const OptionalField<std::uint32_t>& speed_mbps) {
        m_speed_mbps = speed_mbps;
    }

    /*!
     * @brief Get full duplex
     *
     * @return true if the NIC is in Full Duplex mode, false otherwise
     * */
    const OptionalField<bool>& get_full_duplex() const {
        return m_full_duplex;
    }

    /*!
     * @brief Set full duplex
     *
     * @param[in] full_duplex Full duplex enabled
     * */
    void set_full_duplex(const OptionalField<bool>& full_duplex){
        m_full_duplex = full_duplex;
    }

    /*!
     * @brief Returns autosense
     * @return Autosense
     * */
    const OptionalField<bool>& get_autosense() const {
        return m_autosense;
    }

    /*!
     * @brief Set autosense
     * @param[in] autosense autosense
     * */
    void set_autosense(const OptionalField<bool> autosense) {
        m_autosense = autosense;
    }

    /*!
     * @brief Sets MAC address
     * @param[in] mac_address MAC address
     * */
    void set_mac_address(const OptionalField<std::string>& mac_address) {
        m_mac_address = mac_address;
    }

    /*!
     * @brief Returns MAC address
     * @return MAC address
     * */
    const OptionalField<std::string>& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * @brief Sets MAC address
     * @param[in] factory_mac_address factory MAC address
     * */
    void set_factory_mac_address(const OptionalField<std::string>& factory_mac_address) {
        m_factory_mac_address = factory_mac_address;
    }

    /*!
     * @brief Returns factory MAC address
     * @return factory MAC address
     * */
    const OptionalField<std::string>& get_factory_mac_address() const {
        return m_factory_mac_address;
    }

    /*!
     * @brief Returns Ipv4 addresses.
     * @return Reference to Ipv4Addresses.
     * */
    const Ipv4Addresses& get_ipv4_addresses() const {
        return m_ipv4_addresses;
    }

    /*!
     * @brief Set Ipv4 addresses
     * @param[in] ip_addresses Ipv4 addresses
     * */
    void set_ipv4_addresses(const Ipv4Addresses& ip_addresses) {
        m_ipv4_addresses = ip_addresses;
    }

    /*!
     * @brief Add Ipv4 address
     * @param[in] ip_address Ipv4 address
     * */
    void add_ipv4_address(const attribute::Ipv4Address& ip_address) {
        m_ipv4_addresses.add_entry(ip_address);
    }

    /*!
     * @brief Returns Ipv6 address.
     * @return Reference to Ipv6Address.
     * */
    const Ipv6Addresses& get_ipv6_addresses() const {
        return m_ipv6_addresses;
    }

    /*!
     * @brief Set Ipv6 addresses
     * @param[in] ip_address Ipv6 addresses
     * */
    void set_ipv6_addresses(const Ipv6Addresses& ip_address) {
        m_ipv6_addresses = ip_address;
    }

    /*!
     * @brief Add Ipv6 address
     * @param[in] ip_address Ipv6 address
     * */
    void add_ipv6_address(const attribute::Ipv6Address& ip_address) {
        m_ipv6_addresses.add_entry(ip_address);
    }

    /*!
     * @brief return an object ipv of class std::string
     *
     * @return the ipv value
     */
    const OptionalField<std::string>& get_ipv6_default_gateway() const {
        return m_ipv6_default_gateway;
    }

    /*!
     * @brief setter for ipv6_default_gateway attribute
     *
     * @param ipv6_default_gateway of type std::string
     */
    void set_ipv6_default_gateway(const OptionalField<std::string>& ipv6_default_gateway) {
        m_ipv6_default_gateway = ipv6_default_gateway;
    }

    /*!
     * @brief return Max IPv6 Static Addresses
     *
     * @return Max IPv6 Static Addresses
     */
    const OptionalField<std::uint32_t>& get_max_ipv6_static_addresses() const {
        return m_max_ipv6_static_addresses;
    }

    /*!
     * @brief sets for Max IPv6 Static Addresses
     * @param max_ipv6_static_addresses Max IPv6 Static Addresses
     */
    void set_max_ipv6_static_addresses(const OptionalField<std::uint32_t>& max_ipv6_static_addresses) {
        m_max_ipv6_static_addresses = max_ipv6_static_addresses;
    }

    /*!
     * @brief Get supported transport protocols
     * @return array with supported transport protocols
     */
    const TransportProtocols& get_supported_transport_protocols() const {
        return m_supported_transport_protocols;
    }


    /*!
     * @brief Set supported transport protocols
     * @param[in] supported_transport_protocols array with supported types of ethernet protocols
     */
    void set_supported_transport_protocols(const TransportProtocols& supported_transport_protocols) {
        m_supported_transport_protocols = supported_transport_protocols;
    }


private:
    OptionalField<std::uint32_t> m_frame_size{};
    OptionalField<std::uint32_t> m_speed_mbps{};
    OptionalField<bool> m_autosense{};
    OptionalField<bool> m_full_duplex{};
    OptionalField<std::string> m_mac_address{};
    OptionalField<std::string> m_factory_mac_address{};
    Ipv4Addresses m_ipv4_addresses{};
    Ipv6Addresses m_ipv6_addresses{};
    OptionalField<std::string> m_ipv6_default_gateway{};
    OptionalField<std::uint32_t> m_max_ipv6_static_addresses{};
    OptionalField<std::uint32_t> m_default_vlan{};
    TransportProtocols m_supported_transport_protocols{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

