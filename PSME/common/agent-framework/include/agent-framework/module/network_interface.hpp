/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @brief Network interface
 * */

#include "agent-framework/module/oem_data.hpp"
#include "agent-framework/module/status.hpp"
#include "agent-framework/module/ipv4_address.hpp"
#include "agent-framework/module/ipv6_address.hpp"
#include "agent-framework/module/neighbor_info.hpp"
#include "agent-framework/module/resources.hpp"
#include "agent-framework/logger_ext.hpp"
#include "json/json.hpp"

#include <string>

#ifndef AGENT_FRAMEWORK_MODULE_NETWORK_INTERFACE_HPP
#define AGENT_FRAMEWORK_MODULE_NETWORK_INTERFACE_HPP

namespace agent_framework {
namespace generic {

using OEMDataConstUniquePtr = OEMData::OEMDataConstUniquePtr;

/*! Network Interface */
class NetworkInterface : public Resources {
    uint32_t m_frame_size{};
    string m_link_technology{};
    uint32_t m_speed_gbps{};
    bool m_autosense{};
    string m_mac_address{};
    bool m_vlan_enable{};
    uint32_t m_vlan_count{};
    IPv4Address m_ipv4_address{};
    IPv6Address m_ipv6_address{};
    NeighborInfo m_neighbor_info{};
    OEMDataConstUniquePtr m_oem{new OEMData};

public:
    /*! NetworkInterface unique pointer */
    using NetworkInterfaceUniquePtr = std::unique_ptr<NetworkInterface>;

    /*! Default constructor */
    NetworkInterface(){}

    /*! Creates Network Interface unique object */
    static NetworkInterfaceUniquePtr make_network_interface() {
        return NetworkInterfaceUniquePtr{ new NetworkInterface() };
    }

    /*!
     * Read Network Interface configuration
     *
     * @param[in] interface_configuration Network Interface configuration JSON
     * */
    void read_configuration(const json::Value& interface_configuration);

    /*!
     * Returns frame size
     *
     * @return Frame size
     * */
    uint32_t get_frame_size() const {
        return m_frame_size;
    }

    /*!
     * Returns link technology
     *
     * @return Link technology
     * */
    const string& get_link_technology() const {
        return m_link_technology;
    }

    /*!
     * Returns speed in Gbps
     *
     * @return Speed in Gbps
     * */
    uint32_t get_speed_gbps() const {
        return m_speed_gbps;
    }

    /*!
     * Returns autosense
     *
     * @return Autosense
     * */
    bool get_autosense() const {
        return m_autosense;
    }

    /*!
     * Sets MAC address
     *
     * @param[in] mac_address MAC address
     * */
    void set_mac_address(const std::string& mac_address) {
        m_mac_address = mac_address;
    }

    /*!
     * Returns MAC address
     *
     * @return MAC address
     * */
    const string& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * Returns VLAN enable
     *
     * @return VLAN enable
     * */
    bool get_vlan_enable() const {
        return m_vlan_enable;
    }

    /*!
     * Returns VLAN count
     *
     * @return VLAN count
     * */
    uint32_t get_vlan_count() const {
        return m_vlan_count;
    }

    /*!
     * @brief Returns IPv4 address.
     *
     * @return Reference to IPv4Address.
     * */
    const IPv4Address& get_ipv4_address() const { return m_ipv4_address; }

    /*!
     * @brief Returns IPv6 address.
     *
     * @return Reference to IPv6Address.
     * */
    const IPv6Address& get_ipv6_address() const { return m_ipv6_address; }

    /*!
     * @brief Returns neighbor info.
     *
     * @return Reference to NeighborInfo.
     * */
    const NeighborInfo& get_neighbor_info() const { return m_neighbor_info; }

    virtual ~NetworkInterface();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_NETWORK_INTERFACE_HPP */
