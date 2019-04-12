/*!
 * @brief Reading definition for reading packages.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file ipmi/command/sdv/nm/reading.hpp
 */

#pragma once

#include <array>
#include <chrono>
#include <vector>

namespace ipmi {
namespace command {
namespace sdv {

/*! @brief INTEL IANA in LSB order */
constexpr const std::array<std::uint8_t, 3> INTEL_IANA_LSB_FIRST = {{0x57, 0x01, 0x0}};

/* ReadingPackage interval unit */
using deciseconds = std::chrono::duration<std::uint8_t, std::deci>;

/*! @brief Measure type */
enum class MeasureType: std::uint8_t {
    LATEST = 0,
    AVERAGE,
    MINIMUM,
    MAXIMUM
};

/*!
 * @brief Metric identifier with Measure Type
 */
struct MetricId {
    /*!
     * @brief Constructor
     * @param id Metric identifier
     * @param measure_type Metric measure type
     */
    constexpr MetricId(std::uint32_t id, MeasureType measure_type = MeasureType::LATEST)
        : m_id(id), m_measure_type(measure_type) {}

    std::uint32_t m_id{};
    MeasureType m_measure_type{MeasureType::LATEST};
};

/*! @brief MetricId collection */
using MetricIdVec = std::vector<MetricId>;

/*! @brief Constant indicating no reading available */
constexpr std::uint32_t NO_READING_AVAILABLE = 0xFFFFFFFF;

/*! @brief Metric Reading */
struct Reading {

    /*!
     * @brief Constructor
     * @param id MetricId
     * @param value Metric raw reading value
     */
    constexpr Reading(MetricId id, const std::uint32_t value = NO_READING_AVAILABLE)
        : m_metric_id(id), m_raw_value(value) {}

    MetricId m_metric_id;
    std::uint32_t m_raw_value{NO_READING_AVAILABLE};
};

/*! @brief Reading collection */
using Readings = std::vector<Reading>;

}
}
}
