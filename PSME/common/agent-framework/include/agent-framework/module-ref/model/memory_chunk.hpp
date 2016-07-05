/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file memory_chunk.hpp
 *
 * @brief Memory Chunk model class
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MODEL_MEMORY_CHUNK_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_MEMORY_CHUNK_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"

namespace agent_framework {
namespace model {

/*! MemoryChunk */
class MemoryChunk : public Resource {
public:
    using InterleaveSets = attribute::Array<attribute::InterleaveSet>;

    explicit MemoryChunk(const std::string& parent_uuid = {});
    ~MemoryChunk();

    MemoryChunk(const MemoryChunk&) = default;
    MemoryChunk& operator=(const MemoryChunk&) = default;
    MemoryChunk(MemoryChunk&&) = default;
    MemoryChunk& operator=(MemoryChunk&&) = default;

    /*!
     * @brief construct an object of class MemoryChunk from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed MemoryChunk object
     */
    static MemoryChunk from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /**
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return MemoryChunk::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     * */
    static enums::Component get_component() {
        return MemoryChunk::component;
    }

    /*!
     * @brief Get Chunk Name
     * @return Chunk Name
     * */
    const OptionalField<std::string>& get_chunk_name() const {
        return m_chunk_name;
    }

    /*!
     * @brief Set chunk name
     * @param[in] chunk_name Chunk name
     * */
    void set_chunk_name(const OptionalField<std::string>& chunk_name) {
        m_chunk_name = chunk_name;
    }

    /*!
     * @brief Get Chunk Id
     * @return Chunk Id
     * */
    const OptionalField<std::uint32_t>& get_chunk_id() const {
        return m_chunk_id;
    }

    /*!
     * @brief Set chunk Id
     * @param[in] chunk_id Chunk ID
     * */
    void set_chunk_id(const OptionalField<std::uint32_t>& chunk_id) {
        m_chunk_id = chunk_id;
    }

    /*!
     * @brief Get Chunk Size MB
     * @return Chunk Size MB
     * */
    OptionalField<double> get_chunk_size_mb() const {
        return m_chunk_size_mb;
    }

    /*!
     * @brief Sets chunk size MB
     * @param[in] chunk_size_mb Chunk Size MB
     * */
    void set_chunk_size_mb(const OptionalField<double>& chunk_size_mb) {
        m_chunk_size_mb = chunk_size_mb;
    }

    /*!
     * @brief gets Chunk Type
     * @return Type
     * */
    const OptionalField<enums::MemoryChunkType> get_type() const {
        return m_type;
    }

    /*!
     * @brief Sets type
     * @param[in] type Type
     * */
    void set_type(const OptionalField<enums::MemoryChunkType>& type) {
        m_type = type;
    }

    /*!
     * @brief Gets mirrored
     * @return Mirrored
     * */
    const OptionalField<bool>& get_mirrored() const {
        return m_mirrored;
    }

    /*!
     * @brief Sets mirrored
     * @param[in] mirrored Mirrored
     * */
    void set_mirrored(const OptionalField<bool>& mirrored) {
        m_mirrored = mirrored;
    }

    /*!
     * @brief Gets spare
     * @return Spare
     * */
    const OptionalField<bool>& get_spare() const {
        return m_spare;
    }

    /*!
     * @brief Sets spare
     * @param[in] spare Spare
     * */
    void set_spare(const OptionalField<bool>& spare) {
        m_spare = spare;
    }

    /*!
     * @brief Gets interleave sets
     * @return Interleave sets
     * */
    const InterleaveSets& get_interleave_sets() const {
        return m_interleave_sets;
    }

    /*!
     * @brief Sets interleave sets
     * @param[in] interleave_sets Interleave sets
     * */
    void set_interleave_sets(const InterleaveSets& interleave_sets) {
        m_interleave_sets = interleave_sets;
    }

    /*!
     * @brief Adds interleave set
     * @param[in] interleave_set Interleave set
     * */
    void add_interleave_set(const attribute::InterleaveSet& interleave_set) {
        m_interleave_sets.add_entry(interleave_set);
    }

private:
    OptionalField<std::string> m_chunk_name{};
    OptionalField<std::uint32_t> m_chunk_id{};
    OptionalField<double> m_chunk_size_mb{};
    OptionalField<enums::MemoryChunkType> m_type{};
    OptionalField<bool> m_mirrored{};
    OptionalField<bool> m_spare{};
    InterleaveSets m_interleave_sets{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}

#endif /* _AGENT_FRAMEWORK_MODULE_MODEL_MEMORY_CHUNK_HPP */
