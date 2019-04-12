/*!
 * @brief Memory Set attribute for Memory Domain model
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
 * @file memory_set.hpp
 */
#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "array.hpp"


namespace agent_framework {
namespace model {
namespace attribute {

/*! MemorySet */
class MemorySet {
public:

    explicit MemorySet();

    ~MemorySet();

    /*! Enable copy */
    MemorySet(const MemorySet&) = default;
    MemorySet& operator=(const MemorySet&) = default;
    MemorySet(MemorySet&&) = default;
    MemorySet& operator=(MemorySet&&) = default;

    /*!
     * @brief Add Memory UUID for interleave set to an array
     * @param[in] memory_uuid memory UUID
     */
    void add_memory_to_set(const Uuid& memory_uuid) {
        m_memory_set.add_entry(memory_uuid);
    }

    /*!
     * @brief Set Memory UUIDs array for interleave set
     * @param[in] memory_set memory UUIDs array
     */
    void set_memory_set(const attribute::Array<Uuid>& memory_set) {
        m_memory_set = memory_set;
    }

    /*!
     * @brief Get Memory UUIDs array for interleave set
     * @return memory UUIDs collection
     * */
    const attribute::Array<Uuid>& get_memory_set() const {
        return m_memory_set;
    }

    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class MemorySet from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed MemorySet object
     */
    static MemorySet from_json(const json::Json& json);

private:
    Array<Uuid> m_memory_set{};
};

}
}
}
