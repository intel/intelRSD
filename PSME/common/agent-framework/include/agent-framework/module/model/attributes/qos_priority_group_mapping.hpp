/*!
 * @brief Declaration of Network QoS Priority to Priority Group Mapping.
 *        Used as DCBX Shared Configuration for ETS (Enhanced Transmission Selection).
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file qos_priority_group_mapping.hpp
 */

#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! QoS DCBX Priority to Priority Group Mapping */
class QosPriorityGroupMapping {
public:
    explicit QosPriorityGroupMapping();

    QosPriorityGroupMapping(const std::uint32_t priority_group, const std::uint32_t priority):
        m_priority_group(priority_group),
        m_priority(priority)
    {}

    ~QosPriorityGroupMapping();

    /*! Enable copy */
    QosPriorityGroupMapping(const QosPriorityGroupMapping&) = default;
    QosPriorityGroupMapping& operator=(const QosPriorityGroupMapping&) = default;
    QosPriorityGroupMapping(QosPriorityGroupMapping&&) = default;
    QosPriorityGroupMapping& operator=(QosPriorityGroupMapping&&) = default;


    /*!
     * @brief Sets Priority Group
     * @param[in] priority_group Priority Group
     */
    void set_priority_group(const OptionalField<std::uint32_t>& priority_group) {
        m_priority_group = priority_group;
    }


    /*!
     * @brief Gets Priority Group
     * @return Priority Group
     */
    const OptionalField<std::uint32_t>& get_priority_group() const {
        return m_priority_group;
    }


    /*!
    * @brief Sets Priority
    * @param[in] priority Priority
    */
    void set_priority(const OptionalField<std::uint32_t>& priority) {
        m_priority = priority;
    }


    /*!
     * @brief Gets Priority
     * @return Priority
     */
    const OptionalField<std::uint32_t>& get_priority() const {
        return m_priority;
    }


    /*!
     * @brief Converts QosPriorityGroupMapping to JSON object
     * @return JSON representation of QosPriorityGroupMapping object
     */
    json::Json to_json() const;


    /*!
     * @brief Constructs an object of class QosPriorityGroupMapping from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed QosPriorityGroupMapping object
     */
    static QosPriorityGroupMapping from_json(const json::Json& json);

private:
    OptionalField<std::uint32_t> m_priority_group{};
    OptionalField<std::uint32_t> m_priority{};
};

}
}
}