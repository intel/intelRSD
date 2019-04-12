/*!
 * @brief Declaration of TelemetryHub.
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
 * @file ipmi/telemetry/telemetry_hub.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/sdv/nm/reading.hpp"

#include <unordered_map>

namespace ipmi {
namespace sdv {

using ipmi::command::sdv::MetricIdVec;
using ipmi::command::sdv::Readings;

class TelemetryHub final {
public:

    /*! Map of package guid and readings in package */
    using PackagesWithReadings = std::unordered_map<std::uint32_t, ipmi::command::sdv::Readings>;

    /*!
     * @brief Constructor
     * @param ipmi_controller IpmiController
     * @param bridge BridgeInfo to NodeManager
     */
    explicit TelemetryHub(IpmiController& ipmi_controller, const ipmi::BridgeInfo& bridge)
        : m_ipmi(ipmi_controller), m_bridge{bridge} {}

    /*!
     * @brief Retrieves metrics in package
     * @param package_guid Reading package identifier
     * @return Metrics in package.
     */
    MetricIdVec get_package_metrics(std::uint32_t package_guid) const;

    /*!
     * @brief Retrieves package readings
     * @param package_guid Reading package identifier
     * @return Readings collection.
     */
    Readings get_package_readings(std::uint32_t package_guid) const;

    /*!
     * @brief Creates new reading package
     * @param metric_ids Metrics to be contained by new package.
     * @return Created Reading Package GUID.
     */
    std::uint32_t create_package(const MetricIdVec& metric_ids) const;

    /*!
     * @brief Add readings to package
     * @param package_guid Reading package identifier
     * @param metric_ids Metrics to be appended to the package.
     * @return Reading Package GUID.
     */
    std::uint32_t add_metrics_to_package(std::uint32_t package_guid, const MetricIdVec& metric_ids) const;

    /*!
     * @brief Removes reading package
     * @param package_guid Reading package identifier
     */
    void remove_package(std::uint32_t package_guid) const;

    /*!
     * @brief Loads all packages
     * @return All found packages with metrics
     */
    PackagesWithReadings load_packages() const;

    /*!
     * @brief Loads all packages with readings
     * @return All found packages with readings
     */
    PackagesWithReadings load_packages_with_readings() const;

private:
    IpmiController& m_ipmi;
    const ipmi::BridgeInfo& m_bridge;
};

}
}
