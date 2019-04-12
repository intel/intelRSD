/*!
 * @brief Get OOB NIC Boot Options command declaration.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_oob_nic_boot_options.hpp
 */

#pragma once



#include "get_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI GetOOBNIC Boot Options.
 */
class GetOobNicBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobNicBootOptions(uint8_t index = 0);


    /*! Copy constructor */
    GetOobNicBootOptions(const GetOobNicBootOptions&) = default;


    /*! Assignment operator */
    GetOobNicBootOptions& operator=(const GetOobNicBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobNicBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetOobNicBootOptions";
    }
};

}

namespace response {

/*!
 * @brief Response message for IPMI GetOOBNIC Boot option.
 */
class GetOobNicBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobNicBootOptions();


    /*! Copy constructor */
    GetOobNicBootOptions(const GetOobNicBootOptions&) = default;


    /*! Assignment operator */
    GetOobNicBootOptions& operator=(const GetOobNicBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobNicBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;


    bool get_dhcp_enabled() const { return m_dhcp_enabled; }


    IPVersion get_dhcp_version() const { return m_dhcp_version; }


    const std::string& get_address() const { return m_address; }


    const std::string& get_subnet() const { return m_subnet; }


    const std::string& get_gateway() const { return m_gateway; }


    const std::string& get_primary_dns() const { return m_primary_dns; }


    const std::string& get_secondary_dns() const { return m_secondary_dns; }


    uint16_t get_hostname_offset() const { return m_hostname_offset; }


    uint16_t get_hostname_length() const { return m_hostname_length; }


    bool get_vlan_enabled() const { return m_vlan_enabled; }


    uint16_t get_vlan_id() const { return m_vlan_id; }


    const std::string& get_mac_address() const { return m_mac_address; }


    size_t v2_payload_size() override { return V2_RESPONSE_SIZE; }


    std::string what() override { return "NIC"; }

    virtual const char* get_command_name() const override {
        return "GetOobNicBootOptions";
    }

private:

    void set_dhcp_enabled(bool enabled) { m_dhcp_enabled = enabled; }


    void set_dhcp_version(IPVersion version) { m_dhcp_version = version; }


    void set_address(const std::string& address) { m_address = address; }


    void set_subnet(const std::string& address) { m_subnet = address; }


    void set_gateway(const std::string& address) { m_gateway = address; }


    void set_primary_dns(const std::string& address) { m_primary_dns = address; }


    void set_secondary_dns(const std::string& address) { m_secondary_dns = address; }


    void set_hostname_offset(uint16_t offset) { m_hostname_offset = offset; }


    void set_hostname_length(uint16_t len) { m_hostname_length = len; }


    void set_vlan_enabled(bool enabled) { m_vlan_enabled = enabled; }


    void set_vlan_id(uint16_t id) { m_vlan_id = id; }


    void set_mac_address(const std::string& address) { m_mac_address = address; }


    bool m_dhcp_enabled{false};
    IPVersion m_dhcp_version{};
    std::string m_address{};
    std::string m_subnet{};
    std::string m_gateway{};
    std::string m_primary_dns{};
    std::string m_secondary_dns{};
    uint16_t m_hostname_offset{};
    uint16_t m_hostname_length{};
    bool m_vlan_enabled{false};
    uint16_t m_vlan_id{};
    std::string m_mac_address{};

    static constexpr size_t OFFSET_DHCP_ENABLED = OFFSET_INDEX + 1;
    static constexpr size_t OFFSET_DHCP_VERSION = OFFSET_DHCP_ENABLED + 1;
    static constexpr size_t OFFSET_IP_ADDRESS = OFFSET_DHCP_VERSION + 1;
    static constexpr size_t OFFSET_SUBNET = OFFSET_IP_ADDRESS + IP_BYTES;
    static constexpr size_t OFFSET_GATEWAY = OFFSET_SUBNET + IP_BYTES;
    static constexpr size_t OFFSET_PRIMARY_DNS = OFFSET_GATEWAY + IP_BYTES;
    static constexpr size_t OFFSET_SECONDARY_DNS = OFFSET_PRIMARY_DNS + IP_BYTES;
    static constexpr size_t OFFSET_HOSTNAME = OFFSET_SECONDARY_DNS + IP_BYTES;
    static constexpr size_t OFFSET_HOSTNAME_LEN = OFFSET_HOSTNAME + 2;
    static constexpr size_t OFFSET_VLAN_ENABLED = OFFSET_HOSTNAME_LEN + 2;
    static constexpr size_t OFFSET_VLAN_ID = OFFSET_VLAN_ENABLED + 1;
    static constexpr size_t OFFSET_MAC_ADDRESS = OFFSET_VLAN_ID + 2;
    static constexpr size_t V2_RESPONSE_SIZE = OFFSET_MAC_ADDRESS + MAC_BYTES;
};

}

}
}
}
