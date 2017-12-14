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
 * @file connected_entity.hpp
 * @brief ConnectedEntity
 * */

#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {


/*! Connected Entity class */
class ConnectedEntity {
public:

    /*! Default constructor */
    explicit ConnectedEntity(){}
    ConnectedEntity(ConnectedEntity&&) = default;
    ConnectedEntity(const ConnectedEntity&) = default;
    ConnectedEntity& operator=(ConnectedEntity&&) = default;
    ConnectedEntity& operator=(const ConnectedEntity&) = default;

    ~ConnectedEntity();

    /*!
     * @brief Gets entity type
     * @return Entity type
     * */
    const OptionalField<enums::EntityType>& get_entity_type() const {
        return m_entity_type;
    }

    /*!
     * @brief Sets entity type
     * @param[in] entity_type Entity Type
     * */
    void set_entity_type(const OptionalField<enums::EntityType>& entity_type) {
        m_entity_type = entity_type;
    }

    /*!
     * @brief Gets entity role
     * @return Entity role
     * */
    const OptionalField<enums::EntityRole>& get_entity_role() const {
        return m_entity_role;
    }

    /*!
     * @brief Sets entity role
     * @param[in] entity_role Entity role
     * */
    void set_entity_role(const OptionalField<enums::EntityRole>& entity_role) {
        m_entity_role = entity_role;
    }

    /*!
     * @brief Gets entity
     * @return Entity
     * */
    const OptionalField<std::string>& get_entity() const {
        return m_entity;
    }

    /*!
     * @brief Sets entity
     * @param[in] entity Entity
     * */
    void set_entity(const OptionalField<std::string>& entity) {
        m_entity = entity;
    }

    /*
     * Converts this object to json representation
     * @return Json representation of this object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class ConnectedEntity from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed ConnectedEntity object
     */
    static ConnectedEntity from_json(const json::Json& json);

private:
    OptionalField<enums::EntityType> m_entity_type{};
    OptionalField<enums::EntityRole> m_entity_role{};
    OptionalField<std::string> m_entity{};
};

}
}
}
