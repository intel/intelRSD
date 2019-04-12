/*!
 * @brief Policies to assign IDs for entities
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file id_policy.hpp
 */

#pragma once

#include "psme/rest/model/handlers/id_policy_helpers.hpp"
#include "psme/rest/model/handlers/id_memoizer.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "generic/assertions.hpp"

#include "database/database.hpp"

#include <memory>

namespace psme {
namespace rest {
namespace model {
namespace handler {

/*!
 * @brief ID policy assigner
 *
 * An IdPolicy is used to assign REST IDs to model objects discovered
 * by agents. The following class is a virtual base class for all IdPolicies.
 * Its purpose is to ensure that fetching component multiple times
 * does not change rest id
 * */
template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
class IdPolicy {

    friend class HandlerTest;
    friend class FabricHandlersTest;

public:
    /*! @brief Typedef for UUID */
    using UuidType = std::string;

    /*! @brief Typedef for ID */
    using IdType =  std::uint64_t;

    /*!
     * @brief Constructor for id policy
     *
     */
    IdPolicy();

    /*! @brief virtual destructor */
    virtual ~IdPolicy();


    /*!
     * @brief Generate ID
     *
     * Generate ID, which is unique either globally among objects of
     * the same type or for objects with the same parent, and which is to be
     * assigned to newly discovered entity.
     *
     * @param uuid UUID of the entity
     * @param parent_uuid UUID of the parent
     * @return generated ID
     */
    virtual IdType get_id(const UuidType& uuid, const UuidType& parent_uuid);

    /*!
     * @brief Purge IdPolicy object
     *
     * Deallocate ID for requested entity
     *
     * @param uuid UUID to be purged
     * @param parent_uuid Resource parent UUID
     */
    virtual void purge(const UuidType& uuid, const UuidType& parent_uuid);

    /*!
     * @brief Mark all last assigned ID for parent as invalid
     *
     * If parent is removed, mark all LAST ID records of all sub-resources as invalid.
     * They will be wiped out on outdate.
     *
     * @param parent_uuid UUID of removed parent
     * @return number of invalidated LAST ID records
     */
    static unsigned invalidate_last(const UuidType& parent_uuid);

protected:

    /*! @brief Clear all internal data in the database and free all allocated IDs. */
    void reset();

private:
    /*! @brief Not available "constructors" */
    IdPolicy& operator=(const IdPolicy&) = delete;
    IdPolicy(const IdPolicy&) = delete;

    /*!
     * @brief static shared pointer that holds static database object
     *
     * Database static object will be constructed in IdPolicy constructor.
     */
    static database::Database::SPtr database;

    /*!
     * @brief static shared pointer that holds static IdMemoizer object
     *
     * Memoizer static object will be constructed in IdPolicy constructor.
     */
    static IdMemoizer::SPtr memoizer;
    static constexpr agent_framework::model::enums::Component component = CT;
};

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
database::Database::SPtr IdPolicy<CT, NZ>::database {};

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
IdMemoizer::SPtr IdPolicy<CT, NZ>::memoizer {};

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
constexpr agent_framework::model::enums::Component IdPolicy<CT, NZ>::component;

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
IdPolicy<CT, NZ>::IdPolicy() {
    if (!database) {
        database = database::Database::create(component.to_string(), true);
    }
    if (!memoizer) {
        memoizer = IdMemoizer::create(component.to_string());
    }
}
template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
IdPolicy<CT, NZ>::~IdPolicy() { }

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
database::IdValue::IdType IdPolicy<CT, NZ>::IdPolicy::get_id(const UuidType& uuid, const UuidType& parent_uuid) {
    const UuidType& parent = (NZ == NumberingZone::SHARED) ? "" : parent_uuid;

    /* parent might be empty, then 'last' name is assumed */
    database::UuidKey last_key{database::ResourceLastKey::LAST, parent};
    database::IdValue last_id{};
    /* if no last entry found.. 0 is assumed */
    database->get(last_key, last_id);

    database::UuidKey entity_key(uuid, parent);
    database::IdValue entity_id{};
    database::IdValue::IdType id;
    auto next_free = last_id.get() + 1;
    if (database->get(entity_key, entity_id)) {
        id = entity_id.get();
    } else {
        id = next_free;
    }

    /* check if ID has been already allocated and if so, check next id */
    auto _id = id;
    for (;;) {
        auto allocated = memoizer->is_allocated_for(parent, id);
        /* not allocated */
        if (allocated.empty()) {
            break;
        }
        /* already allocated for the entity */
        if (allocated == uuid) {
            break;
        }
        // conflict.. internal error
        auto next_candidate_id = ++next_free;
        if (id <= last_id.get()) {
            log_error("db",
                      "Internal Error : ID " << _id <<
                      " kept in database already allocated, replacing with next that is expected to be free: " << next_candidate_id);
        }
        else {
            log_error("db",
                      "Internal Error : ID " << _id <<
                      " already allocated, replacing with next that is expected to be free: " << next_candidate_id);
        }
        id = next_candidate_id;
    }

    /* what about wrapping? int64 is big enough for us? */
    assert(generic::FAIL("Entity ID wrapped", id == 0));

    /* allocate id for parent */
    memoizer->allocate(parent, uuid, id);

    /* store "new" last-assigned value to the database. If "last" is not valid,
     * make it valid. LastId must stay "forever"
     */
    if ((last_id.get() < id) || (database::Database::EntityValidity::VALID != database->get_validity(last_key))) {
        last_id.set(id);
        database->put(last_key, last_id);
    }

    /* and ID for uuid */
    entity_id.set(id);
    database->put(entity_key, entity_id);

    log_debug("db", "Assigned id=" << id << " for " << database->get_name() << "." << parent << "." <<   uuid);
    return id;
}

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
void IdPolicy<CT, NZ>::purge(const UuidType& uuid, const UuidType& parent_uuid) {
    const UuidType& parent = (NZ == NumberingZone::SHARED) ? "" : parent_uuid;

    database::UuidKey entity_key{uuid, parent};
    database::IdValue entity_id{};
    if (!database->get(entity_key, entity_id)) {
        assert(generic::FAIL("No entity stored in the database"));
        return;
    }

    /* invalidate ID for subresources.. if any */
    unsigned num = invalidate_last(uuid);
    if (0 != num) {
        log_info("db", "Invalidated " << num << " resources ID::LAST for " << uuid);
    }

    /* mark entity as not valid, it will be wiped at some time.. */
    database->invalidate(entity_key);

    /* remove from allocated_ids */
    if (!memoizer->remove_allocated(parent, entity_id.get())) {
        assert(generic::FAIL("Entity not allocated"));
    }
}

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
void IdPolicy<CT, NZ>::reset() {
    database::UuidKey entity_key{};
    database->drop(entity_key);
    database->remove();

    memoizer->clear();
    memoizer->remove();
}

template <agent_framework::model::enums::Component::Component_enum CT, NumberingZone NZ>
unsigned IdPolicy<CT, NZ>::invalidate_last(const UuidType& parent_uuid) {
    const UuidType& parent = (NZ == NumberingZone::SHARED) ? "" : parent_uuid;

    database::Database::SPtr db{database::Database::create("*parent", true)};
    database::ResourceLastKey key{parent};
    auto ret = db->cleanup(key);
    db->remove();
    return ret;
}

} /*! @i{handler} */
} /*! @i{model} */
} /*! @i{rest} */
} /*! @i{psme} */
