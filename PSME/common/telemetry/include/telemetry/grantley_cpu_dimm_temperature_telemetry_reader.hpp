/*!
 * @brief Class to handle Node Manager sensors for Grantley
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
 * @file grantley_cpu_dimm_temperature_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/cpu_dimm_temperature_telemetry_reader.hpp"

namespace telemetry {

/*!
 * @brief Base class for cpu dimm temperature telemetry readers for Grantley platform
 */
class GrantleyCpuDimmTemperatureTelemetryReader : public CpuDimmTemperatureTelemetryReader {
public:
    /*!
     * @brief Get reader id for the Grantley temperature readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();


    /*!
     * @brief Create temperature reader for Grantley platform
     * @param _resource_key resource "type" to be handled
     * @param _metric_definition metric definition
     */
    GrantleyCpuDimmTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition)
        : CpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition, assigned_reader_id()) { }


    std::string get_info() const override { return "Grantley"; }

protected:
    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    bool is_valid(Context::Ptr context) const override;
};


/*!
 * @brief Cpu temperature telemetry reader for Grantley platform
 */
class GrantleyCpuTemperatureTelemetryReader : public GrantleyCpuDimmTemperatureTelemetryReader {
public:

    /*!
     * @brief Create temperature reader for Grantley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    GrantleyCpuTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        GrantleyCpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController&) override;
};


/*!
 * @brief Margin to throttle telemetry reader for Grantley platform
 */
class GrantleyCpuMarginToThrottleTelemetryReader : public GrantleyCpuDimmTemperatureTelemetryReader {
public:

    /*!
     * @brief Create margin to throttle telemetry reader for Grantley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    GrantleyCpuMarginToThrottleTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        GrantleyCpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController&) override;
};


/*!
 * @brief Dimm temperature telemetry reader for Grantley platform
 */
class GrantleyDimmTemperatureTelemetryReader : public GrantleyCpuDimmTemperatureTelemetryReader {
public:

    /*!
     * @brief Create temperature reader for Grantley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    GrantleyDimmTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        GrantleyCpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController&) override;
};


}
