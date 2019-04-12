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
 * @file id_memoizer.hpp
 */

#pragma once

#include <memory>
#include <map>

namespace psme {
namespace rest {
namespace model {
namespace handler {

/*!
 * @brief Allocated IDs
 *
 * Internal class for IdPolicy. Object contains all assigned (to objects) IDs.
 * Only "live" objects are included here.
 */
class IdMemoizer {
public:
    using UuidType = std::string;
    using IdType =  std::uint64_t;


    /*! @brief Shared pointer */
    using SPtr = std::shared_ptr<IdMemoizer>;

    /*!
     * @brief Allocator factory
     *
     * Creates allocator. All created allocators are kept statically by name
     * (entity type).
     *
     * @param name allocator name to be returned
     * @return allocator for requested name (entity type)
     */
    static SPtr create(const std::string& name);

    /*!
     * @brief Remove reference to current allocator
     */
    void remove();

    /*!
     * @brief Allocated identifier in the parent
     * @param parent UUID of the parent
     * @param uuid UUID of the resource
     * @param id Identifier to be allocated
     * @return true if identifier was not allocated
     */
    bool allocate(const UuidType& parent, const UuidType& uuid, IdType id);

    /*! Check ID if allocated. Checking is done for all entries in the parent */
    const UuidType& is_allocated_for(const UuidType& parent, IdType id) const;

    /*! @brief Remove ID allocation */
    bool remove_allocated(const UuidType& parent, IdType id);

    /*! @brief Remove all kept identifiers */
    void clear(void);

private:
    /*! @brief Default constructor used by the factory */
    IdMemoizer();

    /*! @brief Not available "constructors" */
    IdMemoizer& operator=(const IdMemoizer&) = delete;
    IdMemoizer(const IdMemoizer&) = delete;

    using Allocators = std::map<std::string, SPtr>;

    /*! @brief Allocators factory cache */
    static Allocators allocators;


    /*! @brief Allocated IDs */
    using IdMap = std::map<IdType, UuidType>;

    /*! @brief All allocated IDs for parents */
    using Allocated = std::map<UuidType, IdMap>;

    /*!
     * @brief Allocated IDs.
     *
     * Global map of all allocated IDs per parent. Parent is identified by entity
     * type and UUID.
     */
    Allocated allocated_ids{};

    /*! @brief Empty string */
    static const std::string EMPTY;
};

} /*! @i{handler} */
} /*! @i{model} */
} /*! @i{rest} */
} /*! @i{psme} */
