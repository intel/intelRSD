/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
    explicit Endpoint(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::Fabric);
    ~Endpoint();

    Endpoint(const Endpoint&) = default;
    Endpoint& operator=(const Endpoint&) = default;
    Endpoint(Endpoint&&) = default;
    Endpoint& operator=(Endpoint&&) = default;

    using ConnectedEntities = attribute::Array<attribute::ConnectedEntity>;
    using Identifiers = attribute::Array<attribute::Identifier>;

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
     * @param json the Json::Value deserialized to object
     * @return the newly constructed Endpoint object
     */
    static Endpoint from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /*!
     * @brief Gets protocol
     * @return protocol
     */
    const OptionalField<enums::StorageProtocol>& get_protocol() const {
        return m_protocol;
    }

    /*!
     * @brief Sets protocol
     * @param[in] protocol Protocol
     */
    void set_protocol(const OptionalField<enums::StorageProtocol>& protocol) {
        m_protocol = protocol;
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
     * @brief Is the drive an entity of this endpoint
     * @param[in] drive_uuid queried drive uuid
     * @return is the drive an entity of this endpoint
     */
    bool has_drive_entity(const std::string& drive_uuid) const;

private:

    OptionalField<enums::StorageProtocol> m_protocol{};
    ConnectedEntities m_connected_entities{};
    Identifiers m_identifiers{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
