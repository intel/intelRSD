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
 * @file id_memoizer.cpp
 */

#include "psme/rest/model/handlers/id_memoizer.hpp"

#include <logger/logger.hpp>
#include <logger/logger_factory.hpp>

#include <assert.h>
#include <set>

namespace psme {
namespace rest {
namespace model {
namespace handler {




const IdMemoizer::UuidType IdMemoizer::EMPTY{};


IdMemoizer::SPtr IdMemoizer::create(const std::string& name) {

    Allocators::const_iterator it = allocators.find(name);
    if (it != allocators.end()) {
        return it->second;
    }

    /* create new one */
    SPtr added{new IdMemoizer()};
    allocators[name] = added;
    return added;
}

void IdMemoizer::remove(void) {
    /*
     * in the map there is a name.. but allocator has no such property
     * Only 'this' of the allocator is unique..
     */
    Allocators::iterator it;
    for (it = allocators.begin(); it != allocators.end(); it++) {
        if (it->second.get() == this) {
            allocators.erase(it);
            return;
        }
    }
}

IdMemoizer::Allocators IdMemoizer::allocators{};

IdMemoizer::IdMemoizer() { }

bool IdMemoizer::allocate(
        const IdMemoizer::UuidType& parent,
        const IdMemoizer::UuidType& uuid,
        IdMemoizer::IdType id) {

    if (!is_allocated_for(parent, id).empty()) {
        return false;
    }
    allocated_ids[parent][id] = uuid;
    return true;
}

const IdMemoizer::UuidType& IdMemoizer::is_allocated_for(
        const IdMemoizer::UuidType& parent,
        IdMemoizer::IdType id) const {

    Allocated::const_iterator parent_it;
    parent_it = allocated_ids.find(parent);
    if (parent_it != allocated_ids.end()) {
        IdMap::const_iterator entity_it = parent_it->second.find(id);
        if (entity_it != parent_it->second.end()) {
            log_debug("db", "ID " << id << " allocated for entity " << entity_it->second);
            return entity_it->second;
        }
    }
    return EMPTY;
}

bool IdMemoizer::remove_allocated(
        const IdMemoizer::UuidType& parent,
        IdMemoizer::IdType id) {

    Allocated::iterator it;
    it = allocated_ids.find(parent);
    if (it != allocated_ids.end()) {
        if ((it->second).erase(id) != 0) {
            if (it->second.empty()) {
                allocated_ids.erase(it);
            }
            return true;
        }
    }
    return false;
}

void IdMemoizer::clear(void) {
    allocated_ids.clear();
}

} /*! @i{handler} */
} /*! @i{model} */
} /*! @i{rest} */
} /*! @i{psme} */
