/*!
 * @brief Test fixture for iSCSI MDR builders.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file builder_fixture.hpp
 */

#pragma once

namespace fixture {

static const uint16_t connection_wait = 0xABCD;
static const uint8_t retry_count = 0xEF;

/*!
 * @brief Class representing single data field
 * */
template<typename T>
class OptionalField {
public:
    /* Enable copy and move construction */
    OptionalField() = default;
    OptionalField(const OptionalField&) = default;
    OptionalField(OptionalField&&) = default;
    ~OptionalField() = default;

    /* Enable assignment and move assignment */
    OptionalField& operator=(const OptionalField& in_optional_field) = default;
    OptionalField& operator=(OptionalField&& in_optional_field) = default;

    /*!
     * @brief Converting constructor
     *
     * @param[in] in_value Value used to construct OptionalField object
     * */
    template<typename V>
    OptionalField(const V& in_value) : value(in_value) {}

    /*!
     * @brief Assignment operator
     */
    template<typename V>
    OptionalField& operator=(const V& rhs) {
        value = rhs;
        return *this;
    }

    /*!
     * @brief ObjectField to template parameter conversion operator
     *
     * @return Value held by OptionalField object; may throw.
     * */
    operator T() const {
        return value;
    }

    /*!
     * @brief Checks if an OptionalField object has a value assigned to it.
     *
     * @return True if yes, false otherwise.
     * */
    bool has_value() const {
        return true;
    }
private:
    T value;
};

/*! IscsiBoot attribute */
class IscsiBoot {
public:
    /*!
     * @brief Get IP Address Type
     * @return IP Address Type
     * */
    uint32_t get_ip_address_type() const {
        return 1; // IPv4
    }

    /*!
     * @brief Get initiator address
     * @return initiator IP address
     * */
    std::string get_initiator_address() const {
        return "192.168.2.33";
    }

    /*!
     * @brief Get initiator name
     * @return initiator name
     * */
    std::string get_initiator_name() const {
        return "Some initiator name";
    }

    /*!
     * @brief Get default gateway for initiator
     * @return default gateway for initiator
     * */
    std::string get_initiator_default_gateway() const {
        return "192.168.2.34";
    }

    /*!
     * @brief Get the netmask for initiator
     * @return the netmask for initiator
     * */
    std::string get_initiator_netmask() const {
        return "255.255.255.0";
    }

    /*!
     * @brief Get flag determining if target info is taken from DHCP
     * @return flag determining if target info is taken from DHCP
     * */
    bool get_target_info_via_dhcp() const {
        return true;
    }

    /*!
     * @brief Get flag determining whether initiator IP and mask is taken from DHCP
     * @return flag determining whether initiator IP and mask is taken from DHCP
     * */
    bool get_ip_mask_dns_via_dhcp() const {
        return false;
    }

    /*!
     * @brief Get flag determining if router advertisement is enabled
     * @return flag determining if router advertisement is enabled
     * */
    bool get_router_advertisement_enabled() const {
        return true;
    }

    /*!
     * @brief Get primary target address
     * @return primary target address
     * */
    std::string get_primary_target_address() const {
        return "192.168.2.33";
    }

    /*!
     * @brief Get primary_target_name
     * @return primary_target_name
     * */
    std::string get_primary_target_name() const {
        return "Some target name";
    }

    /*!
     * @brief Get primary target port
     * @return primary target port
     * */
    OptionalField<std::uint32_t> get_primary_target_port() const {
        return 8080;
    }

    /*!
     * @brief Get primary LUN
     * @return primary LUN
     * */
    OptionalField<std::uint32_t> get_primary_lun() const {
        return 12;
    }

    /*!
     * @brief Get flag determining if primary target vlan is enabled
     * @return flag determining if primary target vlan is enabled
     * */
    bool get_primary_vlan_enable() const {
        return true;
    }

    /*!
     * @brief Get primary target VLAN Id
     * @return primary target VLAN Id
     * */
    OptionalField<std::uint32_t> get_primary_vlan_id() const {
        return 123;
    }

    /*!
     * @brief Get primary target DNS IP address
     * @return primary target DNS IP address
     * */
    std::string get_primary_dns() const {
        return "192.168.2.34";
    }

    /*!
     * @brief Get secondary target's DNS IP address
     * @return secondary target's DNS IP address
     * */
    std::string get_secondary_dns() const {
        return "192.168.2.38";
    }

    /*!
     * @brief Get authentication method
     * @return authentication method
     * */
    uint32_t get_authentication_method() const {
        return 0; // None
    }

    /*!
     * @brief Get CHAP username
     * @return CHAP username
     * */
    std::string get_chap_username() const {
        return "Some CHAP name";
    }

    /*!
     * @brief Get CHAP secret
     * @return CHAP secret
     * */
    std::string get_chap_secret() const {
        return "Some CHAP ******";
    }

    /*!
     * @brief Get CHAP username for 2-way authentication
     * @return CHAP username for 2-way authentication
     * */
    std::string get_mutual_chap_username() const {
        return "Some CHAP mutual name";
    }

    /*!
     * @brief Get CHAP secret for 2-way authentication
     * @return CHAP secret for 2-way authentication
     * */
    std::string get_mutual_chap_secret() const {
        return "Some CHAP mutual ******";
    }
};

class NetworkDeviceFunction {
public:
    /*!
     * @brief Get flag determining whether the network device function is enabled
     * @return flag determining whether the network device function is enabled
     * */
    bool get_device_enabled() const {
        return true;
    }

    /*!
     * @brief Get the MAC address of the network device
     * @return the MAC address of the network device
     * */
    std::string get_mac_address() const {
        return "02:42:33:61:52:1a";
    }

    /*!
     * @brief Get the object containing iSCSI boot parameters
     * @return the object containing iSCSI boot parameters
     * */
    const IscsiBoot& get_iscsi_boot() const {
        return m_boot;
    }
private:
    IscsiBoot m_boot{};
};

} // fixture
