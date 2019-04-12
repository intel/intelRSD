/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file acl_rule.hpp
 * @brief Acl Rule model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! AclRule */
class AclRule : public Resource {
public:

    using MirroredPorts = agent_framework::model::attribute::Array<std::string>;
    using VlanId = agent_framework::model::attribute::AclVlanId;
    using Ip = agent_framework::model::attribute::AclAddress;
    using Mac = agent_framework::model::attribute::AclAddress;
    using Port = agent_framework::model::attribute::AclPort;
    using Oem = agent_framework::model::attribute::Oem;

    explicit AclRule(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Acl);
    ~AclRule();

    AclRule(const AclRule&) = default;
    AclRule& operator=(const AclRule&) = default;
    AclRule(AclRule&&) = default;
    AclRule& operator=(AclRule&&) = default;

    /*!
     * @brief construct an object of class AclRule from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed AclRule object
     * */
    static AclRule from_json(const json::Json& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     * */
    json::Json to_json() const;

    /*!
     * @brief Get collection name
     *
     * @return collection name
     * */
    static enums::CollectionName get_collection_name() {
        return AclRule::collection_name;
    }

    /*!
     * @brief Get component name
     *
     * @return component name
     * */
    static enums::Component get_component() {
        return AclRule::component;
    }

    /*!
     * @brief Set rule identifier
     *
     * @param[in] rule_id rule identifier
     * */
    void set_rule_id(const OptionalField<uint32_t>& rule_id) {
        m_rule_id = rule_id;
    }

    /*!
     * @brief Get rule id
     *
     * @return rule id
     * */
    const OptionalField<uint32_t>& get_rule_id() const {
        return m_rule_id;
    }

    /*!
     * @brief Set action
     *
     * @param[in] action ACL action
     * */
    void set_action(const OptionalField<enums::AclAction>& action ) {
        m_action = action;
    }

    /*!
     * @brief Get action.
     *
     * @return action
     * */
    const OptionalField<enums::AclAction>& get_action() const {
        return m_action;
    }

    /*!
     * @brief Set forward mirror port.
     *
     * @param[in] forward_mirror_port Forward/mirror port
     * */
    void set_forward_mirror_port(const OptionalField<std::string>& forward_mirror_port ) {
        m_forward_mirror_port = forward_mirror_port;
    }

    /*!
     * @brief Get forward mirror port.
     *
     * @return forward mirror port.
     * */
    const OptionalField<std::string>& get_forward_mirror_port() const {
        return m_forward_mirror_port;
    }

    /*!
     * @brief Add a mirrored port.
     *
     * @param[in] mirrored_port Mirrored port
     * */
    void add_mirrored_port(const std::string& mirrored_port) {
        m_mirrored_ports.add_entry(mirrored_port);
    }

    /*!
     * @brief Set mirrored ports.
     *
     * @param[in] mirrored_ports Mirrored ports
     * */
    void set_mirrored_ports(const MirroredPorts& mirrored_ports) {
        m_mirrored_ports = mirrored_ports;
    }

    /*!
     * @brief Get mirrored ports.
     *
     * @return mirrored ports
     * */
    const MirroredPorts& get_mirrored_ports() const {
        return m_mirrored_ports;
    }


    /*!
     * @brief Get mirror type.
     *
     * @return mirror type
     * */
    const OptionalField<enums::AclMirrorType>& get_mirror_type() const {
        return m_mirror_type;
    }

    /*!
     * @brief Set mirror type.
     *
     * @param[in] mirror_type Mirror type
     * */
    void set_mirror_type(const OptionalField<enums::AclMirrorType>&
        mirror_type) {
        m_mirror_type = mirror_type;
    }

    /*!
     * @brief Get VLAN id
     *
     * @return VLAN ID
     * */
    const VlanId& get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Set VLAN id
     *
     * @param[in] vlan_id VLAN Id
     * */
    void set_vlan_id(const VlanId& vlan_id) {
        m_vlan_id = vlan_id;
    }

    /*!
     * @brief Get source IP
     *
     * @return source IP
     * */
    const Ip& get_source_ip() const {
        return m_source_ip;
    }

    /*!
     * @brief Set source IP
     *
     * @param[in] source_ip Source IP
     * */
    void set_source_ip(const Ip& source_ip ) {
        m_source_ip = source_ip;
    }

    /*!
     * @brief Get destination IP
     *
     * @return destination IP
     * */
    const Ip& get_destination_ip() const {
        return m_destination_ip;
    }

    /*!
     * @brief Set destination IP
     *
     * @param[in] destination_ip Destination IP
     * */
    void set_destination_ip(const Ip& destination_ip ) {
        m_destination_ip = destination_ip;
    }

    /*!
     * @brief Get source MAC
     *
     * @return source MAC
     * */
    const Ip& get_source_mac() const {
        return m_source_mac;
    }

    /*!
     * @brief Set source MAC
     *
     * @param[in] source_mac Source MAC
     * */
    void set_source_mac(const Mac& source_mac ) {
        m_source_mac = source_mac;
    }

    /*!
     * @brief Get destination MAC
     *
     * @return destination MAC
     * */
    const Mac& get_destination_mac() const {
        return m_destination_mac;
    }

    /*!
     * @brief Set destination MAC
     *
     * @param[in] destination_mac Destination MAC
     * */
    void set_destination_mac(const Mac& destination_mac ) {
        m_destination_mac = destination_mac;
    }

    /*!
     * @brief Get source port
     *
     * @return source port
     * */
    const Port& get_source_port() const {
        return m_source_port;
    }

    /*!
     * @brief Set source port
     *
     * @param[in] source_port Source port
     * */
    void set_source_port(const Port& source_port ) {
        m_source_port = source_port;
    }

    /*!
     * @brief Get destination port
     * @return destination port
     * */
    const Port& get_destination_port() const {
        return m_destination_port;
    }

    /*!
     * @brief Set destination port
     *
     * @param[in] destination_port Destination port
     * */
    void set_destination_port(const Port& destination_port ) {
        m_destination_port = destination_port;
    }

    /*!
     * @brief Get protocol
     *
     * @return protocol
     * */
    const OptionalField<uint32_t>& get_protocol() const {
        return m_protocol;
    }

    /*!
     * @brief Set protocol
     *
     * @param[in] protocol Protocol
     * */
    void set_protocol(const OptionalField<uint32_t>& protocol ) {
        m_protocol = protocol;
    }

private:
    OptionalField<std::uint32_t> m_rule_id{};
    OptionalField<enums::AclAction> m_action{};
    OptionalField<std::string> m_forward_mirror_port{};
    MirroredPorts m_mirrored_ports{};
    OptionalField<enums::AclMirrorType> m_mirror_type{};
    VlanId m_vlan_id{};
    Ip m_source_ip{};
    Ip m_destination_ip{};
    Mac m_source_mac{};
    Mac m_destination_mac{};
    Port m_source_port{};
    Port m_destination_port{};
    OptionalField<uint32_t> m_protocol{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

} // namespace model
} // namespace agent_framework
