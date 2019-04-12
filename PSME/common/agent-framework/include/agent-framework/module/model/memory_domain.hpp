/*!
 * @brief Memory Domain model interface
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file memory_domain.hpp
 */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! MemoryDomain */
class MemoryDomain : public Resource {
public:
    explicit MemoryDomain(const Uuid& parent_uuid = {}, enums::Component parent_type = enums::Component::System);
    ~MemoryDomain();

    MemoryDomain(const MemoryDomain&) = default;
    MemoryDomain& operator=(const MemoryDomain&) = default;
    MemoryDomain(MemoryDomain&&) = default;
    MemoryDomain& operator=(MemoryDomain&&) = default;

    /*!
     * @brief Construct an object of class MemoryChunks from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed MemoryChunks object
     * */
    static MemoryDomain from_json(const json::Json& json);

    /*!
     * @brief Transform the object to Json
     * @return the object serialized to json::Json
     * */
    json::Json to_json() const;

    /*!
     * @brief Get collection name
     * @return collection name
     * */
    static enums::CollectionName get_collection_name() {
        return MemoryDomain::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return MemoryDomain::component;
    }

    /*!
     * @brief Set flag indicating Memory Domain supports the creation of Memory Chunks
     * @param[in] allows_memory_chunk_creation memory chunk creation allowed flag
     * */
    void set_allows_memory_chunk_creation(const OptionalField<bool>& allows_memory_chunk_creation) {
        m_allows_memory_chunk_creation = allows_memory_chunk_creation;
    }

    /*!
     * @brief Check if Memory Domain supports the creation of Memory Chunks
     * @return True if is enabled, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& get_allows_memory_chunk_creation() const {
        return m_allows_memory_chunk_creation;
    }

    /*!
     * @brief Set flag indicating Memory Domain supports the provisioning of blocks of memory
     * @param[in] allows_block_provisioning block provisioning allowed flag
     * */
    void set_allows_block_provisioning(const OptionalField<bool>& allows_block_provisioning) {
        m_allows_block_provisioning = allows_block_provisioning;
    }

    /*!
     * @brief Check if Memory Domain supports the provisioning of blocks of memory
     * @return True if is enabled, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& get_allows_block_provisioning() const {
        return m_allows_block_provisioning;
    }

    /*!
     * @brief Set memory mirroring allowed flag
     * @param[in] allows_mirroring mirror allowed flag
     * */
    void set_allows_mirroring(const OptionalField<bool>& allows_mirroring) {
        m_allows_mirroring = allows_mirroring;
    }

    /*!
     * @brief Check if MemoryDomain supports creation of MemoryChunks with mirroring enabled
     * @return True if is allowed, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& get_allows_mirroring() const {
        return m_allows_mirroring;
    }

    /*!
     * @brief Set memory sparing allowed flag
     * @param[in] allows_sparing sparing allowed flag
     * */
    void set_allows_sparing(const OptionalField<bool>& allows_sparing) {
        m_allows_sparing = allows_sparing;
    }

    /*!
     * @brief Check if MemoryDomain supports creation of MemoryChunks with sparing enabled
     * @return True if is allowed, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& get_allows_sparing() const {
        return m_allows_sparing;
    }

    /*!
     * @brief Add interleave set for a memory chunk
     * @param[in] interleavable_memory_sets interleave set
     * */
    void add_interleavable_memory_set(const attribute::MemorySet& interleavable_memory_set) {
        m_interleavable_memory_sets.add_entry(interleavable_memory_set);
    }

    /*!
     * @brief Set interleave sets for the memory chunks
     * @param[in] interleavable_memory_sets interleave sets
     * */
    void set_interleavable_memory_sets(const attribute::Array<attribute::MemorySet>& interleavable_memory_sets) {
        m_interleavable_memory_sets = interleavable_memory_sets;
    }

    /*!
     * @brief Get interleave sets for the memory chunks
     * @return interleave sets
     * */
    const attribute::Array<attribute::MemorySet>& get_interleavable_memory_sets() const {
        return m_interleavable_memory_sets;
    }

private:
    OptionalField<bool> m_allows_memory_chunk_creation{};
    OptionalField<bool> m_allows_block_provisioning{};
    OptionalField<bool> m_allows_mirroring{};
    OptionalField<bool> m_allows_sparing{};
    attribute::Array<attribute::MemorySet> m_interleavable_memory_sets{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}