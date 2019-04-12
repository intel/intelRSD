/*!
 * @brief Declaration of Network QoS Bandwidth for Priority Group Allocation class.
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
 * @file qos_bandwidth_allocation.hpp
 */

#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! QoS DCBX Bandwidth Allocation */
class QosBandwidthAllocation {
public:
    explicit QosBandwidthAllocation();

    QosBandwidthAllocation(const std::uint32_t priority_group, const std::uint32_t bandwidth_percent):
        m_priority_group(priority_group),
        m_bandwidth_percent(bandwidth_percent)
    {}

    ~QosBandwidthAllocation();

    /*! Enable copy */
    QosBandwidthAllocation(const QosBandwidthAllocation&) = default;
    QosBandwidthAllocation& operator=(const QosBandwidthAllocation&) = default;
    QosBandwidthAllocation(QosBandwidthAllocation&&) = default;
    QosBandwidthAllocation& operator=(QosBandwidthAllocation&&) = default;


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
     * @brief Sets Bandwidth percent
     * @param[in] bandwidth_percent Bandwidth percent
     */
    void set_bandwidth_percent(const OptionalField<std::uint32_t>& bandwidth_percent) {
        m_bandwidth_percent = bandwidth_percent;
    }


    /*!
     * @brief Gets Bandwidth percent
     * @return Bandwidth percent
     */
    const OptionalField<std::uint32_t>& get_bandwidth_percent() const {
        return m_bandwidth_percent;
    }


    /*!
     * @brief Converts QosBandwidthAllocation to JSON object
     * @return JSON representation of QosBandwidthAllocation object
     */
    json::Json to_json() const;


    /*!
     * @brief Constructs an object of class QosBandwidthAllocation from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed QosBandwidthAllocation object
     */
    static QosBandwidthAllocation from_json(const json::Json& json);

private:
    OptionalField<std::uint32_t> m_priority_group{};
    OptionalField<std::uint32_t> m_bandwidth_percent{};
};

}
}
}