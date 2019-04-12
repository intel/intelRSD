/*!
 * @brief Memory Chunks model interface
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
 * @file memory_chunks.hpp
 */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! MemoryChunks */
class MemoryChunks : public Resource {
public:
     explicit MemoryChunks(const Uuid& parent_uuid = {}, enums::Component parent_type = enums::Component::MemoryDomain);
    ~MemoryChunks();

    MemoryChunks(const MemoryChunks&) = default;
    MemoryChunks& operator=(const MemoryChunks&) = default;
    MemoryChunks(MemoryChunks&&) = default;
    MemoryChunks& operator=(MemoryChunks&&) = default;

    /*!
     * @brief Construct an object of class MemoryChunks from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed MemoryChunks object
     * */
    static MemoryChunks from_json(const json::Json& json);

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
        return MemoryChunks::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return MemoryChunks::component;
    }

    /*!
     * @brief Set memory chunk size
     * @param[in] memory_chunk_size_mib memory chunk size (in MiB)
     * */
    void set_memory_chunk_size_mib(const OptionalField<std::uint32_t>& memory_chunk_size_mib) {
        m_memory_chunk_size_mib = memory_chunk_size_mib;
    }

    /*!
     * @brief Get memory chunk size
     * @return memory chunk size (in MiB)
     * */
    const OptionalField<std::uint32_t>& get_memory_chunk_size_mib() const {
        return m_memory_chunk_size_mib;
    }

    /*!
     * @brief Set address range type
     * @param[in] address_range_type address range type
     * */
    void set_address_range_type(const OptionalField<enums::AddressRangeType>& address_range_type) {
        m_address_range_type = address_range_type;
    }

    /*!
     * @brief Get address_range_type
     * @return address range type
     * */
    const OptionalField<enums::AddressRangeType>& get_address_range_type() const {
        return m_address_range_type;
    }

    /*!
     * @brief Set memory mirroring flag
     * @param[in] is_mirror_enabled mirror enabled flag
     * */
    void set_mirror_enabled(const OptionalField<bool>& is_mirror_enabled) {
        m_is_mirror_enabled = is_mirror_enabled;
    }

    /*!
     * @brief Check if memory mirroring is enabled for this MemoryChunk
     * @return True if is enabled, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& is_mirror_enabled() const {
        return m_is_mirror_enabled;
    }

    /*!
     * @brief Set memory sparing status
     * @param[in] is_spare spare enabled status
     * */
    void set_spare(const OptionalField<bool>& is_spare) {
        m_is_spare = is_spare;
    }

    /*!
     * @brief Check if memory sparing is enabled for this MemoryChunk
     * @return True if is enabled, false if not, empty OptionalField if not set
     * */
    const OptionalField<bool>& is_spare() const {
        return m_is_spare;
    }

    /*!
     * @brief Add interleave set for a memory chunk
     * @param[in] interleave_set interleave set
     * */
    void add_interleave_set(const attribute::InterleaveSet& interleave_set) {
        m_interleave_sets.add_entry(interleave_set);
    }

    /*!
     * @brief Set interleave sets
     * @param[in] interleave_sets interleave sets
     * */
    void set_interleave_sets(const attribute::Array<attribute::InterleaveSet>& interleave_sets) {
        m_interleave_sets = interleave_sets;
    }

    /*!
     * @brief Get interleave sets
     * @return interleave sets
     * */
    const attribute::Array<attribute::InterleaveSet>& get_interleave_sets() const {
        return m_interleave_sets;
    }

private:
    OptionalField<std::uint32_t> m_memory_chunk_size_mib{};
    OptionalField<enums::AddressRangeType> m_address_range_type{};
    OptionalField<bool> m_is_mirror_enabled{};
    OptionalField<bool> m_is_spare{};
    attribute::Array<attribute::InterleaveSet> m_interleave_sets{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}