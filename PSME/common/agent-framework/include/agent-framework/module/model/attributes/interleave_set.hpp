/*!
 * @brief Interleave Set attribute for Memory Chunks model
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
 * @file interleave_set.hpp
 */
#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! InterleaveSet */
class InterleaveSet {
public:

    explicit InterleaveSet();

    InterleaveSet(const std::string& region_id, std::uint32_t offset_mib, std::uint32_t size_mib, std::uint32_t memory_level = 1):
                  m_region_id{region_id},
                  m_offset_mib{offset_mib},
                  m_size_mib{size_mib},
                  m_memory_level{memory_level}
    {}

    ~InterleaveSet();

    /*! Enable copy */
    InterleaveSet(const InterleaveSet&) = default;
    InterleaveSet& operator=(const InterleaveSet&) = default;
    InterleaveSet(InterleaveSet&&) = default;
    InterleaveSet& operator=(InterleaveSet&&) = default;

    /*!
     * @brief Sets memory of the interleave set
     * @param[in] memory which is used for interleave set
     */
    void set_memory(const OptionalField<Uuid>& memory) {
        m_memory = memory;
    }

    /*!
     * @brief Gets memory of the interleave set
     * @return memory used for interleave set
     * */
    const OptionalField<Uuid>& get_memory() const {
        return m_memory;
    }

    /*!
     * @brief Sets region id
     * @param[in] region_id Region ID
     * */
    void set_region_id(const OptionalField<std::string>& region_id) {
        m_region_id = region_id;
    }

    /*!
     * @brief Gets region id
     * @return region id
     * */
    const OptionalField<std::string>& get_region_id() const {
        return m_region_id;
    }

    /*!
     * @brief Sets offset
     * @param[in] offset_mib offset in MiB
     * */
    void set_offset_mib(const OptionalField<std::uint32_t>& offset_mib) {
        m_offset_mib = offset_mib;
    }

    /*!
     * @brief Gets offset
     * @return offset in MiB
     * */
    const OptionalField<std::uint32_t>& get_offset_mib() const {
        return m_offset_mib;
    }

    /*!
     * @brief Sets size
     * @param[in] size_mib size in MiB
     * */
    void set_size_mib(const OptionalField<std::uint32_t>& size_mib) {
        m_size_mib = size_mib;
    }

    /*!
     * @brief Gets size
     * @return size in MiB
     * */
    OptionalField<std::uint32_t> get_size_mib() const {
        return m_size_mib;
    }

    /*!
     * @brief Sets memory level
     * @param[in] memory_level memory level
     * */
    void set_memory_level(const OptionalField<std::uint32_t>& memory_level) {
        m_memory_level = memory_level;
    }

    /*!
     * @brief Gets memory level
     * @return memory level
     * */
    OptionalField<std::uint32_t> get_memory_level() const {
        return m_memory_level;
    }

    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class InterleaveSet from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed InterleaveSet object
     */
    static InterleaveSet from_json(const json::Json& json);

private:
    OptionalField<Uuid> m_memory{};
    OptionalField<std::string> m_region_id{};
    OptionalField<std::uint32_t> m_offset_mib{};
    OptionalField<std::uint32_t> m_size_mib{};
    OptionalField<std::uint32_t> m_memory_level{};
};

}
}
}