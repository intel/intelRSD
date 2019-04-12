/*!
 * @brief Definition of database entity class.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file database_entity.hpp
 */

#pragma once

#include "agent-framework/module/utils/uuid.hpp"

#include "database/aggregate.hpp"
#include "database/persistent_attributes.hpp"

namespace agent_framework {
namespace database {

template <const char DATABASE_NAME[]>
class DatabaseEntity {
public:

    DatabaseEntity(const Uuid& uuid) :
        m_db{::database::PersistentAttributes::get_instance(), uuid, DATABASE_NAME} { }

    /*!
     * @brief Destructor
     */
    virtual ~DatabaseEntity() {
        m_db.dump();
    }

    /*!
     * @brief Store an attribute in database
     *
     * @param property Attribute's key
     * @param value Attribute's value
     */
    virtual void put(const std::string& property, const std::string& value) {
        try {
            m_db.update(property, value);
        }
        catch (const std::exception&) {
            // There is no key in DB so put the value to DB
            m_db.put(property, value);
        }
    }

    /*!
     * @brief Get an attribute from database
     *
     * @param property Attribute's key
     * @return Attribute's value
     */
    virtual std::string get(const std::string& property) const {
        return m_db.get(property);
    }

    /*!
     * @brief Delete an attribute from database
     *
     * @param property Attribute's key
     */
    virtual void del(const std::string& property) {
        m_db.del(property);
    }

    /*!
     * @brief Append an entry to the list in given bucket
     *
     * @param bucket Bucket's name
     * @param value entry value
     */
    virtual void append(const std::string& bucket, const std::string& value) {
        m_db.append(bucket, value);
    }

    /*!
     * @brief Get all entries from given bucket
     *
     * @param bucket Bucket's name
     * @return vector of values from given bucket
     */
    virtual std::vector<std::string> get_multiple_values(const std::string& bucket) const {
        return m_db.get_multiple_values(bucket);
    }

    /*!
     * @brief Remove database
     *
     * @param entity Uuid of the entity
     *
     * @warning This method needs to be called after close all active connections to database (instances of this class)
     */
    static void remove(const Uuid& entity) {
        ::database::Aggregate<::database::PersistentAttributes> db{
            ::database::PersistentAttributes::get_instance(), entity, DATABASE_NAME
        };
        db.remove();
    }

    /*!
     * @brief Get aggregates stored in database.
     * @return Database aggregates of persistent attributes.
     */
    static ::database::Aggregate<::database::PersistentAttributes>::AggregateContainer get_aggregates() {
        return ::database::Aggregate<::database::PersistentAttributes>::get_aggregates(
            ::database::PersistentAttributes::get_instance(), DATABASE_NAME);
    }

private:
    ::database::Aggregate<::database::PersistentAttributes> m_db;
};


}
}

