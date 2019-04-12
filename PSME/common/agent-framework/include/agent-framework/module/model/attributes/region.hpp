/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * @file region.hpp
 * @brief Region attribute
 * */
#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Region */
class Region {
public:

    explicit Region();

    Region(const std::string& region_id,
            const enums::MemoryClass& memory_type,
            std::uint32_t offset_mib,
            std::uint32_t size_mib):
            m_region_id{region_id},
            m_memory_type{memory_type},
            m_offset_mib{offset_mib},
            m_size_mib{size_mib}
            {}

    ~Region();

    /*! Enable copy */
    Region(const Region&) = default;
    Region& operator=(const Region&) = default;
    Region(Region&&) = default;
    Region& operator=(Region&&) = default;

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
     * @brief Sets memory type
     * @param[in] memory_type Dimm function class
     * */
    void set_memory_type(const OptionalField<enums::MemoryClass>& memory_type) {
        m_memory_type = memory_type;
    }

    /*!
     * @brief Gets memory type
     * @return memory type
     * */
    const OptionalField<enums::MemoryClass>& get_memory_type() const {
        return m_memory_type;
    }

    /*!
     * @brief Sets offset MiB
     * @param[in] offset_mib Offset MiB
     * */
    void set_offset_mib(const OptionalField<std::uint32_t>& offset_mib) {
        m_offset_mib = offset_mib;
    }

    /*!
     * @brief Gets offset MiB
     * @return offset MiB
     * */
    const OptionalField<std::uint32_t>& get_offset_mib() const {
        return m_offset_mib;
    }

    /*!
     * @brief Sets size MiB
     * @param[in] size_mib Size MiB
     * */
    void set_size_mib(const OptionalField<std::uint32_t>& size_mib) {
        m_size_mib = size_mib;
    }

    /*!
     * @brief Gets Size MiB
     * @return size MiB
     * */
    OptionalField<std::uint32_t> get_size_mib() const {
        return m_size_mib;
    }

    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class Region from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed Region object
     */
    static Region from_json(const json::Json& json);

private:
    OptionalField<std::string> m_region_id{};
    OptionalField<enums::MemoryClass> m_memory_type{};
    OptionalField<std::uint32_t> m_offset_mib{};
    OptionalField<std::uint32_t> m_size_mib{};
};

}
}
}
