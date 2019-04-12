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
 * @file connected_entity.hpp
 * @brief ConnectedEntity
 */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace agent_framework {
namespace model {
namespace attribute {

/*! Connected Entity class */
class ConnectedEntity {
public:
    using Identifiers = attribute::Array<Identifier>;


    /*! Default constructor */
    explicit ConnectedEntity() {}


    ConnectedEntity(ConnectedEntity&&) = default;


    ConnectedEntity(const ConnectedEntity&) = default;


    ConnectedEntity& operator=(ConnectedEntity&&) = default;


    ConnectedEntity& operator=(const ConnectedEntity&) = default;


    ~ConnectedEntity();


    /*!
     * @brief Gets entity role
     * @return Entity role
     */
    const OptionalField<enums::EntityRole>& get_entity_role() const {
        return m_entity_role;
    }


    /*!
     * @brief Sets entity role
     * @param[in] entity_role Entity role
     */
    void set_entity_role(const OptionalField<enums::EntityRole>& entity_role) {
        m_entity_role = entity_role;
    }


    /*!
     * @brief Gets entity
     * @return Entity
     */
    const OptionalField<std::string>& get_entity() const {
        return m_entity;
    }


    /*!
     * @brief Sets entity
     * @param[in] entity Entity
     */
    void set_entity(const OptionalField<std::string>& entity) {
        m_entity = entity;
    }


    /*!
     * @brief Get connected entity identifiers
     * @return array with connected entity identifiers
     */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief Set connected entity identifiers
     * @param[in] identifiers array with connected entity identifiers
     */
    void set_identifiers(const Identifiers& identifiers) {
        m_identifiers = identifiers;
    }


    /*!
     * @brief Add connected entity identifier
     * @param[in] identifier supported type of identifier
     */
    void add_identifier(const attribute::Identifier& identifier) {
        m_identifiers.add_entry(identifier);
    }


    /*!
     * @brief Gets Lun
     * @return Lun
     */
    const OptionalField<std::int64_t>& get_lun() const {
        return m_lun;
    }


    /*!
     * @brief Sets Lun
     * @param[in] lun Lun
     */
    void set_lun(const OptionalField<std::int64_t>& lun) {
        m_lun = lun;
    }


    /*!
     * @brief Converts this object to json representation
     * @return Json representation of this object
     */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class ConnectedEntity from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed ConnectedEntity object
     */
    static ConnectedEntity from_json(const json::Json& json);


private:
    OptionalField<enums::EntityRole> m_entity_role{};
    OptionalField<std::string> m_entity{};
    Identifiers m_identifiers{};
    OptionalField<std::int64_t> m_lun{};
};

}
}
}
