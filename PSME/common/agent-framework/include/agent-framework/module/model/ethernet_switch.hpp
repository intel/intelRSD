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
 * @file ethernet_switch.hpp
 * @brief EthernetSwitch model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! EthernetSwitch */
class EthernetSwitch : public Resource {
public:
    explicit EthernetSwitch(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Manager);
    ~EthernetSwitch();

    EthernetSwitch(const EthernetSwitch&) = default;
    EthernetSwitch& operator=(const EthernetSwitch&) = default;
    EthernetSwitch(EthernetSwitch&&) = default;
    EthernetSwitch& operator=(EthernetSwitch&&) = default;


    /*!
     * @brief construct an object of class EthernetSwitch from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed EthernetSwitch object
     */
    static EthernetSwitch from_json(const json::Json& json);


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
    static const enums::CollectionName& get_collection_name() {
        return EthernetSwitch::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static const enums::Component& get_component() {
        return EthernetSwitch::component;
    }


    /*!
     * @brief Set switch identifier
     * @param[in] switch_identifier switch identifier string
     * */
    void set_switch_identifier(const OptionalField<std::string>& switch_identifier) {
        m_switch_identifier = switch_identifier;
    }


    /*!
     * @brief Get switch identifier
     * @return switch identifier
     * */
    const OptionalField<std::string>& get_switch_identifier() const {
        return m_switch_identifier;
    }


    /*!
     * @brief Set mac address
     * @param[in] mac_address switch mac address
     * */
    void set_mac_address(const OptionalField<std::string>& mac_address) {
        m_mac_address = mac_address;
    }


    /*!
     * @brief Get mac address
     * @return mac address string
     * */
    const OptionalField<std::string>& get_mac_address() const {
        return m_mac_address;
    }


    /*!
     * @brief setter for technology attribute
     *
     * @param technology of type enums::SwitchTechnology
     */
    void set_technology(const OptionalField<enums::SwitchTechnology>& technology) {
        m_technology = technology;
    }


    /*!
     * @brief Get switch technology
     * @return switch technology
     * */
    const OptionalField<enums::SwitchTechnology>& get_technology() const {
        return m_technology;
    }


    /*!
     * @brief setter for fru_info attribute
     *
     * @param fru_info of type attribute::FruInfo
     */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }


    /*!
     * @brief Get switch FRU info
     * @return FRU info
     * */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }


    /*!
     * @brief setter for location attribute
     *
     * @param location of type std::uint32_t
     */
    void set_location(const OptionalField<std::uint32_t>& location) {
        m_location = location;
    }


    /*!
     * @brief return location of type std::uint32_t
     *
     * @return the location value
     */
    const OptionalField<std::uint32_t>& get_location() const {
        return m_location;
    }


    /*!
     * @brief setter for chassis attribute
     *
     * @param chassis of type std::string
     */
    void set_chassis(const OptionalField<std::string>& chassis) {
        m_chassis = chassis;
    }


    /*!
     * @brief return an object chassis of class std::string
     *
     * @return the chassis value
     */
    const OptionalField<std::string>& get_chassis() const {
        return m_chassis;
    }


    /*!
     * @brief Setter for firmware name
     *
     * @param firmware_name firmware name to set or NIL to remove
     */
    void set_firmware_name(const OptionalField<std::string>& firmware_name) {
        m_firmware_name = firmware_name;
    }


    /*!
     * @brief Getter for firmware name
     *
     * @return firmware name as a string or NIL if not set
     */
    const OptionalField<std::string>& get_firmware_name() const {
        return m_firmware_name;
    }


    /*!
     * @brief Setter for firmware version
     *
     * @param firmware_version firmware version to set or NIL to remove
     */
    void set_firmware_version(const OptionalField<std::string>& firmware_version) {
        m_firmware_version = firmware_version;
    }


    /*!
     * @brief Getter for firmware version
     *
     * @return firmware version as a string or NIL if not set
     */
    const OptionalField<std::string>& get_firmware_version() const {
        return m_firmware_version;
    }


    /*!
     * @brief setter for switch role in the network
     *
     * @param role to be set
     */
    void set_role(const OptionalField<enums::SwitchRole>& role) {
        m_role = role;
    }


    /*!
     * @brief Get switch role
     * @return switch role
     * */
    const OptionalField<enums::SwitchRole>& get_role() const {
        return m_role;
    }


    /*!
     * @brief Setter for manufacturing date
     *
     * @param manufacturing_date manufacturing date to set or NIL to remove
     */
    void set_manufacturing_date(const OptionalField<std::string>& manufacturing_date) {
        m_manufacturing_date = manufacturing_date;
    }


    /*!
     * @brief Getter for manufacturing date
     *
     * @return manufacturing date as a string or NIL if not set
     */
    const OptionalField<std::string>& get_manufacturing_date() const {
        return m_manufacturing_date;
    }


    /*!
     * @brief Setter for max ACL number
     *
     * @param[in] max_acl_number max ACL number to set or NIL to remove
     */
    void set_max_acl_number(const OptionalField<std::uint32_t>
        max_acl_number) {
        m_max_acl_number = max_acl_number;
    }


    /*!
     * @brief Getter for max ACL number
     *
     * @return max ACL number or NIL if not set
     */
    const OptionalField<std::uint32_t> get_max_acl_number() const {
        return m_max_acl_number;
    }


    /*!
     * @brief Setter for LLDP enabled (Link Layer Discovery Protocol)
     *
     * @param[in] lldp_enabled LLDP enable or disable
     */
    void set_lldp_enabled(const OptionalField<bool> lldp_enabled) {
        m_lldp_enabled = lldp_enabled;
    }


    /*!
     * @brief Getter for LLDP enabled (Link Layer Discovery Protocol)
     *
     * @return LLDP enabled or disabled
     */
    const OptionalField<bool> get_lldp_enabled() const {
        return m_lldp_enabled;
    }


    /*!
     * @brief Setter for PFC enabled (Priority-Based Flow Control)
     *
     * @param[in] pfc_enabled PFC enable or disable
     */
    void set_pfc_enabled(const OptionalField<bool> pfc_enabled) {
        m_pfc_enabled = pfc_enabled;
    }


    /*!
     * @brief Getter for PFC enabled (Priority-Based Flow Control)
     *
     * @return PFC enabled or disabled
     */
    const OptionalField<bool> get_pfc_enabled() const {
        return m_pfc_enabled;
    }


    /*!
     * @brief Setter for ETS enabled (Enhanced Transmission Selection)
     *
     * @param[in] ets_enabled ETS enable or disable
     */
    void set_ets_enabled(const OptionalField<bool> ets_enabled) {
        m_ets_enabled = ets_enabled;
    }


    /*!
     * @brief Getter for ETS enabled (Enhanced Transmission Selection)
     *
     * @return ETS enabled or disabled
     */
    const OptionalField<bool> get_ets_enabled() const {
        return m_ets_enabled;
    }


    /*!
     * @brief Setter for DCBX enabled (Data Center Bridging Capability Exchange)
     *
     * @param[in] dcbx_enabled DCBX enable or disable
     */
    void set_dcbx_enabled(const OptionalField<bool> dcbx_enabled) {
        m_dcbx_enabled = dcbx_enabled;
    }


    /*!
     * @brief Getter for DCBX enabled (Data Center Bridging Capability Exchange)
     *
     * @return DCBX enabled or disabled
     */
    const OptionalField<bool> get_dcbx_enabled() const {
        return m_dcbx_enabled;
    }


    /*!
     * @brief Setter for QoS application protocol
     *
     * @param[in] application_protocol QoS application protocol
     */
    void set_qos_application_protocol(const attribute::Array<attribute::QosApplicationProtocol>& application_protocol) {
        m_qos_application_protocol = application_protocol;
    }


    /*!
     * @brief Getter for QoS application protocol
     *
     * @return QoS application protocol
     */
    const attribute::Array<attribute::QosApplicationProtocol>& get_qos_application_protocol() const {
        return m_qos_application_protocol;
    }


    /*!
     * @brief Setter for QoS priority to priority group mapping for ETS
     *
     * @param[in] priority_mapping QoS priority to priority group mapping
     */
    void set_qos_priority_group_mapping(const attribute::Array<attribute::QosPriorityGroupMapping>& priority_mapping) {
        m_qos_priority_group_mapping = priority_mapping;
    }


    /*!
     * @brief Getter for QoS priority to priority group mapping for ETS
     *
     * @return QoS priority to priority group mapping
     */
    const attribute::Array<attribute::QosPriorityGroupMapping>& get_qos_priority_group_mapping() const {
        return m_qos_priority_group_mapping;
    }


    /*!
     * @brief Setter for QoS bandwidth allocation for ETS
     *
     * @param[in] bandwidth_allocation QoS bandwidth allocation object
     */
    void set_qos_bandwidth_allocation(const attribute::Array<attribute::QosBandwidthAllocation>& bandwidth_allocation) {
        m_qos_bandwidth_allocation = bandwidth_allocation;
    }


    /*!
     * @brief Getter for QoS bandwidth allocation for ETS
     *
     * @return QoS bandwidth allocation object
     */
    const attribute::Array<attribute::QosBandwidthAllocation>& get_qos_bandwidth_allocation() const {
        return m_qos_bandwidth_allocation;
    }

private:
    OptionalField<std::string> m_switch_identifier{};
    OptionalField<enums::SwitchTechnology> m_technology{};
    OptionalField<std::string> m_mac_address{};
    OptionalField<std::string> m_firmware_name{}; // new in gami 0.82
    OptionalField<std::string> m_firmware_version{}; // new in gami 0.82
    OptionalField<enums::SwitchRole> m_role{}; // new in gami 0.82
    attribute::FruInfo m_fru_info{};
    OptionalField<std::string> m_manufacturing_date{}; // new in gami 0.82
    OptionalField<std::uint32_t> m_location{};
    OptionalField<std::string> m_chassis{};
    OptionalField<std::uint32_t> m_max_acl_number{};
    OptionalField<bool> m_lldp_enabled{};
    OptionalField<bool> m_pfc_enabled{};
    OptionalField<bool> m_ets_enabled{};
    OptionalField<bool> m_dcbx_enabled{};
    attribute::Array<attribute::QosApplicationProtocol> m_qos_application_protocol{};
    attribute::Array<attribute::QosPriorityGroupMapping> m_qos_priority_group_mapping{};
    attribute::Array<attribute::QosBandwidthAllocation> m_qos_bandwidth_allocation{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

} // namespace model
} // namespace agent_framework

