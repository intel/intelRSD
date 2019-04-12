/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file storage_pool.hpp
 * @brief Declaration of Storage Pool class
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

class StoragePool : public Resource {
public:
    using CapacitySources = attribute::Array<attribute::CapacitySource>;
    using Identifiers = attribute::Array<attribute::Identifier>;


    explicit StoragePool(const std::string& parent_uuid = {},
                         enums::Component parent_type = enums::Component::None);


    virtual ~StoragePool();


    StoragePool(const StoragePool&) = default;


    StoragePool& operator=(const StoragePool&) = default;


    StoragePool(StoragePool&&) = default;


    StoragePool& operator=(StoragePool&&) = default;


    /*!
     * @brief Construct an object of class StoragePool from JSON
     * @param json the json::Json deserialized to object
     * @return The newly constructed StoragePool object
     * */
    static StoragePool from_json(const json::Json& json);


    /*!
     * @brief Transform the object to JSON
     * @return The object serialized to json::Json
     * */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return Collection name
     */
    static enums::CollectionName get_collection_name() {
        return StoragePool::collection_name;
    }


    /*!
     * @brief Get component name
     * @return Component name
     */
    static enums::Component get_component() {
        return StoragePool::component;
    }


    /*!
     * @brief Get capacity
     * @return capacity
     */
    const attribute::Capacity& get_capacity() const {
        return m_capacity;
    }


    /*!
     * @brief Set capacity
     * @param[in] capacity capacity
     */
    void set_capacity(const attribute::Capacity& capacity) {
        m_capacity = capacity;
    }


    /*!
     * @brief Get Capacity Sources
     * @return capacity sources
     */
    const CapacitySources& get_capacity_sources() const {
        return m_capacity_sources;
    }


    /*!
     * @brief Set capacity sources
     * @param[in] capacity_sources Capacity Sources
     */
    void set_capacity_sources(const CapacitySources& capacity_sources) {
        m_capacity_sources = capacity_sources;
    }


    /*!
     * @brief Add connected entity to the Connected entities array
     * @param[in] capacity_source Connected entity
     */
    void add_capacity_source(const attribute::CapacitySource& capacity_source) {
        m_capacity_sources.add_entry(capacity_source);
    }


    /*!
    * @brief Get pool's identifiers
    * @return Array with pool's identifiers
    */
    const Identifiers& get_identifiers() const {
        return m_identifiers;
    }


    /*!
     * @brief Set pool's identifiers
     * @param[in] identifiers array with pools identifiers
     */
    void set_identifiers(const Identifiers& identifiers) {
        m_identifiers = identifiers;
    }


    /*!
     * @brief Add pool identifier
     * @param[in] identifier pools identifier to add
     */
    void add_identifier(const attribute::Identifier& identifier) {
        m_identifiers.add_entry(identifier);
    }


    /*!
     * @brief Gets block size bytes
     * @return block_size bytes
     * */
    const OptionalField<std::uint64_t>& get_block_size_bytes() const {
        return m_block_size_bytes;
    }


    /*!
     * @brief Sets block size bytes
     * @param[in] block_size_bytes of type uint64_t
     */
    void set_block_size_bytes(const OptionalField<std::uint64_t>& block_size_bytes) {
        m_block_size_bytes = block_size_bytes;
    }

    /*!
     * @brief Checks if storage pool has available capacity to allocate new bytes.
     * @param storage_pool Storage pool object
     * @param requested_capacity_bytes Capacity bytes to check
     * @return Returns true if requested capacity could be allocated, false otherwise.
     * */
    static bool can_allocate_bytes(const StoragePool& pool, std::int64_t requested_capacity_bytes);


private:
    attribute::Capacity m_capacity{};
    CapacitySources m_capacity_sources{};
    OptionalField<std::uint64_t> m_block_size_bytes{};
    Identifiers m_identifiers{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
