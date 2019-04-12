/*!
 * @brief Declaration of iSCSI boot attribute
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_boot.hpp
 */

#pragma once

#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! IscsiBoot attribute */
class IscsiBoot {
public:
    /*! Default constructor */
    explicit IscsiBoot();


    /*! Enable copy & assignment */
    IscsiBoot(const IscsiBoot&) = default;
    IscsiBoot(IscsiBoot&&) = default;

    IscsiBoot& operator=(const IscsiBoot&) = default;
    IscsiBoot& operator=(IscsiBoot&&) = default;


    /*!
     * @brief Convert IscsiBoot to JSON object
     *
     * @return JSON representation of IscsiBoot object
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class IscsiBoot from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed IscsiBoot object
     */
    static IscsiBoot from_json(const json::Json& json);


    /*! Default destructor */
    ~IscsiBoot();

    /*!
     * @brief Get IP Address Type
     * @return IP Address Type
     * */
    const enums::IPAddressType get_ip_address_type() const {
        return m_ip_address_type;
    }

    /*!
     * @brief Set IP Address Type
     * @param[in] ip_address_type the IP address type
     * */
    void set_ip_address_type(const enums::IPAddressType ip_address_type) {
        m_ip_address_type = ip_address_type;
    }

    /*!
     * @brief Get initiator address
     * @return initiator IP address
     * */
    const std::string& get_initiator_address() const {
        return m_initiator_address;
    }

    /*!
     * @brief Set initiator address
     * @param[in] initiator_address the initiator IP address
     * */
    void set_initiator_address(const std::string& initiator_address) {
        m_initiator_address = initiator_address;
    }

    /*!
     * @brief Get initiator name
     * @return initiator name
     * */
    const std::string& get_initiator_name() const {
        return m_initiator_name;
    }

    /*!
     * @brief Set initiator name
     * @param[in] initiator_name the initiator name
     * */
    void set_initiator_name(const std::string& initiator_name) {
        m_initiator_name = initiator_name;
    }

    /*!
     * @brief Get default gateway for initiator
     * @return default gateway for initiator
     * */
    const OptionalField<std::string>& get_initiator_default_gateway() const {
        return m_initiator_default_gateway;
    }

    /*!
     * @brief Set default gateway for initiator
     * @param[in] initiator_default_gateway the default gateway for initiator
     * */
    void set_initiator_default_gateway(const OptionalField<std::string>& initiator_default_gateway) {
        m_initiator_default_gateway = initiator_default_gateway;
    }

    /*!
     * @brief Get the netmask for initiator
     * @return the netmask for initiator
     * */
    const std::string& get_initiator_netmask() const {
        return m_initiator_netmask;
    }

    /*!
     * @brief Set the netmask for initiator
     * @param[in] initiator_netmask the netmask for initiator
     * */
    void set_initiator_netmask(const std::string& initiator_netmask) {
        m_initiator_netmask = initiator_netmask;
    }

    /*!
     * @brief Get flag determining if target info is taken from DHCP
     * @return flag determining if target info is taken from DHCP
     * */
    bool get_target_info_via_dhcp() const {
        return m_target_info_via_dhcp;
    }

    /*!
     * @brief Set flag determining if target info is taken from DHCP
     * @param[in] target_info_via_dhcp the flag determining if target info is taken from DHCP
     * */
    void set_target_info_via_dhcp(const bool target_info_via_dhcp) {
        m_target_info_via_dhcp = target_info_via_dhcp;
    }

    /*!
     * @brief Get flag determining whether initiator IP and mask is taken from DHCP
     * @return flag determining whether initiator IP and mask is taken from DHCP
     * */
    bool get_ip_mask_dns_via_dhcp() const {
        return m_ip_mask_dns_via_dhcp;
    }

    /*!
     * @brief Set flag determining whether initiator IP and mask is taken from DHCP
     * @param[in] ip_mask_dns_via_dhcp the flag determining whether initiator IP and mask is taken from DHCP
     * */
    void set_ip_mask_dns_via_dhcp(const bool ip_mask_dns_via_dhcp) {
        m_ip_mask_dns_via_dhcp = ip_mask_dns_via_dhcp;
    }

    /*!
     * @brief Get flag determining if router advertisement is enabled
     * @return flag determining if router advertisement is enabled
     * */
    bool get_router_advertisement_enabled() const {
        return m_router_advertisement_enabled;
    }

    /*!
     * @brief Set flag determining if router advertisement is enabled
     * @param[in] router_advertisement_enabled the flag determining if router advertisement is enabled
     * */
    void set_router_advertisement_enabled(const bool router_advertisement_enabled) {
        m_router_advertisement_enabled = router_advertisement_enabled;
    }

    /*!
     * @brief Get primary target address
     * @return primary target address
     * */
    const OptionalField<std::string>& get_primary_target_address() const {
        return m_primary_target_address;
    }

    /*!
     * @brief Set primary target address
     * @param[in] primary_target_address the primary target address
     * */
    void set_primary_target_address(const OptionalField<std::string>& primary_target_address) {
        m_primary_target_address = primary_target_address;
    }

    /*!
     * @brief Get primary_target_name
     * @return primary_target_name
     * */
    const OptionalField<std::string>& get_primary_target_name() const {
        return m_primary_target_name;
    }

    /*!
     * @brief Set primary_target_name
     * @param[in] primary_target_name the primary_target_name
     * */
    void set_primary_target_name(const OptionalField<std::string>& primary_target_name) {
        m_primary_target_name = primary_target_name;
    }

    /*!
     * @brief Get primary target port
     * @return primary target port
     * */
    const OptionalField<std::uint32_t>& get_primary_target_port() const {
        return m_primary_target_port;
    }

    /*!
     * @brief Set primary target port
     * @param[in] primary_target_port the primary target port
     * */
    void set_primary_target_port(const OptionalField<std::uint32_t>& primary_target_port) {
        m_primary_target_port = primary_target_port;
    }

    /*!
     * @brief Get primary LUN
     * @return primary LUN
     * */
    const OptionalField<std::uint32_t>& get_primary_lun() const {
        return m_primary_lun;
    }

    /*!
     * @brief Set primary LUN
     * @param[in] primary_lun the primary LUN
     * */
    void set_primary_lun(const OptionalField<std::uint32_t>& primary_lun) {
        m_primary_lun = primary_lun;
    }

    /*!
     * @brief Get flag determining if primary target vlan is enabled
     * @return flag determining if primary target vlan is enabled
     * */
    bool get_primary_vlan_enable() const {
        return m_primary_vlan_enable;
    }

    /*!
     * @brief Set flag determining if primary target vlan is enabled
     * @param[in] primary_vlan_enable the flag determining if primary target vlan is enabled
     * */
    void set_primary_vlan_enable(const bool primary_vlan_enable) {
        m_primary_vlan_enable = primary_vlan_enable;
    }

    /*!
     * @brief Get primary target VLAN Id
     * @return primary target VLAN Id
     * */
    const OptionalField<std::uint32_t>& get_primary_vlan_id() const {
        return m_primary_vlan_id;
    }

    /*!
     * @brief Set primary target VLAN Id
     * @param[in] primary_vlan_id the primary target VLAN Id
     * */
    void set_primary_vlan_id(const OptionalField<std::uint32_t>& primary_vlan_id) {
        m_primary_vlan_id = primary_vlan_id;
    }

    /*!
     * @brief Get primary target DNS IP address
     * @return primary target DNS IP address
     * */
    const OptionalField<std::string>& get_primary_dns() const {
        return m_primary_dns;
    }

    /*!
     * @brief Set primary target DNS IP address
     * @param[in] primary_dns the primary target DNS IP address
     * */
    void set_primary_dns(const OptionalField<std::string>& primary_dns) {
        m_primary_dns = primary_dns;
    }

    /*!
     * @brief Get secondary target IP address
     * @return secondary target IP address
     * */
    const OptionalField<std::string>& get_secondary_target_address() const {
        return m_secondary_target_address;
    }

    /*!
     * @brief Set secondary target IP address
     * @param[in] secondary_target_address the secondary target IP address
     * */
    void set_secondary_target_address(const OptionalField<std::string>& secondary_target_address) {
        m_secondary_target_address = secondary_target_address;
    }

    /*!
     * @brief Get secondary target name
     * @return secondary target name
     * */
    const OptionalField<std::string>& get_secondary_target_name() const {
        return m_secondary_target_name;
    }

    /*!
     * @brief Set secondary target name
     * @param[in] secondary_target_name the secondary target name
     * */
    void set_secondary_target_name(const OptionalField<std::string>& secondary_target_name) {
        m_secondary_target_name = secondary_target_name;
    }

    /*!
     * @brief Get secondary target port
     * @return secondary target port
     * */
    const OptionalField<std::uint32_t>& get_secondary_target_port() const {
        return m_secondary_target_port;
    }

    /*!
     * @brief Set secondary target port
     * @param[in] secondary_target_port the secondary target port
     * */
    void set_secondary_target_port(const OptionalField<std::uint32_t>& secondary_target_port) {
        m_secondary_target_port = secondary_target_port;
    }

    /*!
     * @brief Get secondary target LUN
     * @return secondary target LUN
     * */
    const OptionalField<std::uint32_t>& get_secondary_lun() const {
        return m_secondary_lun;
    }

    /*!
     * @brief Set secondary target LUN
     * @param[in] secondary_lun the secondary target LUN
     * */
    void set_secondary_lun(const OptionalField<std::uint32_t>& secondary_lun) {
        m_secondary_lun = secondary_lun;
    }

    /*!
     * @brief Get flag determining if secondary target's VLAN is enabled
     * @return flag determining if secondary target's VLAN is enabled
     * */
    const OptionalField<bool>& get_secondary_vlan_enable() const {
        return m_secondary_vlan_enable;
    }

    /*!
     * @brief Set flag determining if secondary target's VLAN is enabled
     * @param[in] secondary_vlan_enable the flag determining if secondary target's VLAN is enabled
     * */
    void set_secondary_vlan_enable(const OptionalField<bool>& secondary_vlan_enable) {
        m_secondary_vlan_enable = secondary_vlan_enable;
    }

    /*!
     * @brief Get secondary target VLAN Id
     * @return secondary target VLAN Id
     * */
    const OptionalField<std::uint32_t>& get_secondary_vlan_id() const {
        return m_secondary_vlan_id;
    }

    /*!
     * @brief Set secondary target VLAN Id
     * @param[in] secondary_vlan_id the secondary target VLAN Id
     * */
    void set_secondary_vlan_id(const OptionalField<std::uint32_t>& secondary_vlan_id) {
        m_secondary_vlan_id = secondary_vlan_id;
    }

    /*!
     * @brief Get secondary target's DNS IP address
     * @return secondary target's DNS IP address
     * */
    const OptionalField<std::string>& get_secondary_dns() const {
        return m_secondary_dns;
    }

    /*!
     * @brief Set secondary target's DNS IP address
     * @param[in] secondary_dns the secondary target's DNS IP address
     * */
    void set_secondary_dns(const OptionalField<std::string>& secondary_dns) {
        m_secondary_dns = secondary_dns;
    }

    /*!
     * @brief Get authentication method
     * @return authentication method
     * */
    const enums::FunctionAuthenticationMethod get_authentication_method() const {
        return m_authentication_method;
    }

    /*!
     * @brief Set authentication method
     * @param[in] authentication_method the authentication method
     * */
    void set_authentication_method(const enums::FunctionAuthenticationMethod authentication_method) {
        m_authentication_method = authentication_method;
    }

    /*!
     * @brief Get CHAP username
     * @return CHAP username
     * */
    const OptionalField<std::string>& get_chap_username() const {
        return m_chap_username;
    }

    /*!
     * @brief Set CHAP username
     * @param[in] chap_username the CHAP username
     * */
    void set_chap_username(const OptionalField<std::string>& chap_username) {
        m_chap_username = chap_username;
    }

    /*!
     * @brief Get CHAP secret
     * @return CHAP secret
     * */
    const OptionalField<std::string>& get_chap_secret() const {
        return m_chap_secret;
    }

    /*!
     * @brief Set CHAP secret
     * @param[in] chap_secret the CHAP secret
     * */
    void set_chap_secret(const OptionalField<std::string>& chap_secret) {
        m_chap_secret = chap_secret;
    }

    /*!
     * @brief Get CHAP username for 2-way authentication
     * @return CHAP username for 2-way authentication
     * */
    const OptionalField<std::string>& get_mutual_chap_username() const {
        return m_mutual_chap_username;
    }

    /*!
     * @brief Set CHAP username for 2-way authentication
     * @param[in] mutual_chap_username the CHAP username for 2-way authentication
     * */
    void set_mutual_chap_username(const OptionalField<std::string>& mutual_chap_username) {
        m_mutual_chap_username = mutual_chap_username;
    }

    /*!
     * @brief Get CHAP secret for 2-way authentication
     * @return CHAP secret for 2-way authentication
     * */
    const OptionalField<std::string>& get_mutual_chap_secret() const {
        return m_mutual_chap_secret;
    }

    /*!
     * @brief Set CHAP secret for 2-way authentication
     * @param[in] mutual_chap_secret the CHAP secret for 2-way authentication
     * */
    void set_mutual_chap_secret(const OptionalField<std::string>& mutual_chap_secret) {
        m_mutual_chap_secret = mutual_chap_secret;
    }

private:
    enums::IPAddressType m_ip_address_type{enums::IPAddressType::IPv4};
    std::string m_initiator_address{};
    std::string m_initiator_name{};
    OptionalField<std::string> m_initiator_default_gateway{};
    std::string m_initiator_netmask{};
    bool m_target_info_via_dhcp{};
    bool m_ip_mask_dns_via_dhcp{};
    bool m_router_advertisement_enabled{};

    OptionalField<std::string> m_primary_target_address{};
    OptionalField<std::string> m_primary_target_name{};
    OptionalField<std::uint32_t> m_primary_target_port{};
    OptionalField<std::uint32_t> m_primary_lun{};
    bool m_primary_vlan_enable{};
    OptionalField<std::uint32_t> m_primary_vlan_id{};
    OptionalField<std::string> m_primary_dns{};

    OptionalField<std::string> m_secondary_target_address{};
    OptionalField<std::string> m_secondary_target_name{};
    OptionalField<std::uint32_t> m_secondary_target_port{};
    OptionalField<std::uint32_t> m_secondary_lun{};
    OptionalField<bool> m_secondary_vlan_enable{};
    OptionalField<std::uint32_t> m_secondary_vlan_id{};
    OptionalField<std::string> m_secondary_dns{};

    enums::FunctionAuthenticationMethod m_authentication_method{enums::FunctionAuthenticationMethod::None};
    OptionalField<std::string> m_chap_username{};
    OptionalField<std::string> m_chap_secret{};
    OptionalField<std::string> m_mutual_chap_username{};
    OptionalField<std::string> m_mutual_chap_secret{};
};

}
}
}
