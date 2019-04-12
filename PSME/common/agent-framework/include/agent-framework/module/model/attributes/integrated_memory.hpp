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
 *
 * @file integrated_memory.hpp
 * @brief IntegratedMemory attribute
 * */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

class IntegratedMemory {
public:
    /*!
     * @brief Get processor memory type
     *
     * @return Processor memory type
     */
    const OptionalField<enums::IntegratedMemoryType>& get_type() const {
        return m_type;
    }


    /*!
     * @brief Set processor memory type
     *
     * @param type Memory type
     */
    void set_type(const OptionalField<enums::IntegratedMemoryType>& type) {
        m_type = type;
    }


    /*!
     * @brief Get capacity in megabytes
     *
     * @return Processor capacity in megabytes
     */
    const OptionalField<std::uint32_t>& get_capacity_mb() const {
        return m_capacity_mb;
    }


    /*!
     * Set memory capacity in megabytes
     *
     * @param capacity_mb Processor memory capacity in megabytes
     */
    void set_capacity_mb(const OptionalField<std::uint32_t>& capacity_mb) {
        m_capacity_mb = capacity_mb;
    }


    /*!
     * @brief Get processor memory speed in MHz
     *
     * @return Processor memory speed in MHz
     */
    const OptionalField<std::uint32_t>& get_speed_mhz() const {
        return m_speed_mhz;
    }


    /*!
     * @brief Set processor memory speed in MHz
     *
     * @param speed_mhz Processor memory speed
     */
    void set_speed_mhz(const OptionalField<std::uint32_t>& speed_mhz) {
        m_speed_mhz = speed_mhz;
    }


    /*!
     * @brief construct an object of class IntegratedMemory from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed IntegratedMemory object
     */
    static IntegratedMemory from_json(const json::Json& json);


    /*!
     * @brief Transform the object to JSon
     *
     * @return A IntegratedMemory object serialized to json::Json
     */
    json::Json to_json() const;


private:
    OptionalField<enums::IntegratedMemoryType> m_type{};
    OptionalField<std::uint32_t> m_capacity_mb{};
    OptionalField<std::uint32_t> m_speed_mhz{};
};

}
}
}
