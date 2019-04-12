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
 * @file endpoint.hpp
 * @brief Fabric endpoint
 * */
#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! Endpoint */
class Endpoint : public Resource {
public:
    using ConnectedEntities = attribute::Array<attribute::ConnectedEntity>;
    using Identifiers = attribute::Array<attribute::Identifier>;
    using IpTransportDetails = attribute::Array<attribute::IpTransportDetail>;


    explicit Endpoint(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Fabric);


    ~Endpoint();


    Endpoint(const Endpoint&) = default;


    Endpoint& operator=(const Endpoint&) = default;


    Endpoint(Endpoint&&) = default;


    Endpoint& operator=(Endpoint&&) = default;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Endpoint::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Endpoint::component;
    }


    /*!
     * @brief construct an object of class Endpoint from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed Endpoint object
     */
    static Endpoint from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Gets protocol
     * @return protocol
     */
    const OptionalField<enums::TransportProtocol>& get_protocol() const {
        return m_protocol;
    }


    /*!
     * @brief Sets protocol
     * @param[in] protocol Protocol
     */
    void set_protocol(const OptionalField<enums::TransportProtocol>& protocol) {
        m_protocol = protocol;
    }


    /*!
     * @brief Gets OEM protocol
     * @return OEM protocol type
     */
    const OptionalField<enums::OemProtocol>& get_oem_protocol() const {
        return m_oem_protocol;
    }


    /*!
     * @brief Sets OEM protocol
     * @param[in] protocol OEM protocol
     */
    void set_oem_protocol(const OptionalField<enums::OemProtocol>& protocol) {
        m_oem_protocol = protocol;
    }


    /*!
     * @brief Get Connected Entities
     * @return connected entities
     */
    const ConnectedEntities& get_connected_entities() const {
        return m_connected_entities;
    }


    /*!
     * @brief Set connected entities
     * @param[in] connected_entities Connected Entities
     */
    void set_connected_entities(const ConnectedEntities& connected_entities) {
        m_connected_entities = connected_entities;
    }


    /*!
     * @brief Add connected entity to the Connected entities array
     * @param[in] connected_entity Connected entity
     */
    void add_connected_entity(const attribute::ConnectedEntity& connected_entity) {
        m_connected_entities.add_entry(connected_entity);
    }


    /*!
     * @brief Get storage controller's identifiers
     * @return array with storage controller's identifiers
     */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief Set storage controller's identifiers
     * @param[in] identifiers array with storage controller's identifiers
     */
    void set_identifiers(const Identifiers& identifiers) {
        m_identifiers = identifiers;
    }


    /*!
     * @brief Add device identifier
     * @param[in] identifier supported type of identifier
     */
    void add_identifier(const attribute::Identifier& identifier) {
        m_identifiers.add_entry(identifier);
    }


    /*!
     * @brief Get IP transports details
     * @return array with IP transport details
     */
    const IpTransportDetails& get_ip_transport_details() const {
        return m_transports;
    }


    /*!
     * @brief Set IP transports details
     * @param[in] transports array with IP transport details
     */
    void set_ip_transport_details(const IpTransportDetails& transports) {
        m_transports = transports;
    }


    /*!
     * @brief Add IP transport details
     * @param[in] transport details object
     */
    void add_ip_transport_detail(const attribute::IpTransportDetail& transport) {
        m_transports.add_entry(transport);
    }


    /*!
     * @brief Gets username
     * @return username which is used for endpoint
     * */
    const OptionalField<std::string>& get_username() const {
        return m_username;
    }


    /*!
     * @brief Sets username
     * @param[in] username which is used for endpoint
     */
    void set_username(const OptionalField<std::string>& username) {
        m_username = username;
    }


    /*!
     * @brief Cleans username
     * */
    void clean_username() {
        m_username.reset();
    }


    /*!
     * @brief Cleans password
     * */
    void clean_password() {
        m_password.reset();
    }


    /*!
     * @brief Gets password
     * @return password which is used for endpoint
     * */
    const OptionalField<std::string>& get_password() const {
        return m_password;
    }


    /*!
     * @brief Sets password
     * @param[in] password which is used for endpoint
     */
    void set_password(const OptionalField<std::string>& password) {
        m_password = password;
    }


    /*!
     * @brief Is the provided UUID an entity of this endpoint
     * @param[in] endpoint_uuid Endpoint's UUID
     * @param[in] resource_uuid Queried resource's UUID
     * @return Is the resource an entity of this endpoint
     */
    static bool has_entity(const Uuid& endpoint_uuid, const Uuid& resource_uuid);


    /*!
     * @brief Is the provided resource UUID member of any endpoint
     * @param[in] resource_uuid uuid of the queried resource
     * @return Is the resource member of any endpoint
     */
    static bool is_resource_in_endpoint(const Uuid& resource_uuid);


    /*!
     * @brief Checks if given endpoint is in provided role.
     * @param[in] endpoint_uuid UUID of endpoint to check.
     * @param[in] role Requested entity role.
     * @return Returns true if endpoint is in provided role.
     */
    static bool is_in_role(const Uuid& endpoint_uuid, const enums::EntityRole& role);


    /*!
     * @brief Checks if given endpoint is in provided role.
     * @param[in] endpoint Reference to endpoint to check.
     * @param[in] role Requested entity role.
     * @return Returns true if endpoint is in provided role.
     */
    static bool is_in_role(const Endpoint& endpoint, const enums::EntityRole& role);


    /*!
     * @brief Checks if given endpoint is in target role.
     * @param endpoint_uuid UUID of endpoint to check.
     * @return Returns true if endpoint is in target role.
     */
    static bool is_target(const Uuid& endpoint_uuid);


    /*!
     * @brief Checks if given endpoint is in initiator role.
     * @param endpoint_uuid UUID of endpoint to check.
     * @return Returns true if endpoint is in initiator role.
     */
    static bool is_initiator(const Uuid& endpoint_uuid);


    /*!
     * @brief Checks if given endpoint is in target role.
     * @param endpoint Reference to endpoint to check.
     * @return Returns true if endpoint is in target role.
     */
    static bool is_target(const Endpoint& endpoint);


    /*!
     * @brief Checks if given endpoint is in initiator role.
     * @param endpoint Reference to endpoint to check.
     * @return Returns true if endpoint is in initiator role.
     */
    static bool is_initiator(const Endpoint& endpoint);


    /*!
     * @brief Checks if Endpoint is bound to Zone
     * @param endpoint_uuid Endpoint's UUID
     * @return is Endpoint bound to Zone
     */
    static bool is_bound_to_zone(const Uuid& endpoint_uuid);


private:

    OptionalField<enums::TransportProtocol> m_protocol{};
    OptionalField<enums::OemProtocol> m_oem_protocol{};
    ConnectedEntities m_connected_entities{};
    Identifiers m_identifiers{};
    IpTransportDetails m_transports{};
    OptionalField<std::string> m_username{};
    OptionalField<std::string> m_password{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
