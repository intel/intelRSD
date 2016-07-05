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
 *
 * @file region.hpp
 * @brief Region attribute
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_REGION_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_REGION_HPP

#include "agent-framework/module-ref/utils/utils.hpp"
#include "agent-framework/module-ref/enum/compute.hpp"

#include <string>
#include <json/json.h>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Region */
class Region {
public:

    explicit Region();

    Region(const std::string& region_id,
            const enums::MemoryClass& memory_type,
            std::uint32_t offset_mb,
            std::uint32_t size_mb):
            m_region_id{region_id},
            m_memory_type{memory_type},
            m_offset_mb{offset_mb},
            m_size_mb{size_mb}
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
     * @brief Sets offset MB
     * @param[in] offset_mb Offset MB
     * */
    void set_offset_mb(const OptionalField<std::uint32_t>& offset_mb) {
        m_offset_mb = offset_mb;
    }

    /*!
     * @brief Gets offset MB
     * @return offset MB
     * */
    const OptionalField<std::uint32_t>& get_offset_mb() const {
        return m_offset_mb;
    }

    /*!
     * @brief Sets size MB
     * @param[in] size_mb Size MB
     * */
    void set_size_mb(const OptionalField<std::uint32_t>& size_mb) {
        m_size_mb = size_mb;
    }

    /*!
     * @brief Gets Size MB
     * @return size MB
     * */
    OptionalField<std::uint32_t> get_size_mb() const {
        return m_size_mb;
    }

    /*!
     * @brief Converts this to json representation.
     * @return json representation of this class.
     * */
    Json::Value to_json() const;

    /*!
     * @brief construct an object of class Region from JSON
     * @param json the Json::Value deserialized to object
     * @return the newly constructed Region object
     */
    static Region from_json(const Json::Value& json);

private:
    OptionalField<std::string> m_region_id{};
    OptionalField<enums::MemoryClass> m_memory_type{};
    OptionalField<std::uint32_t> m_offset_mb{};
    OptionalField<std::uint32_t> m_size_mb{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_REGION_HPP */
