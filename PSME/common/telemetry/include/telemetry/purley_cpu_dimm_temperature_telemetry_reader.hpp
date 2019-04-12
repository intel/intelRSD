/*!
 * @brief Class to handle Node Manager sensors for Purley
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
 * @file purley_cpu_dimm_temperature_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/cpu_dimm_temperature_telemetry_reader.hpp"

namespace telemetry {

/*!
 * @brief Base class for cpu dimm temperature telemetry readers for Purley platform
 */
class PurleyCpuDimmTemperatureTelemetryReader : public CpuDimmTemperatureTelemetryReader {
public:
    /*!
     * @brief Get reader id for the Purley temperature readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();


    /*!
     * @brief Create temperature reader for Purley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    PurleyCpuDimmTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        CpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition, assigned_reader_id()) { }

    std::string get_info() const override { return "Purley"; }

protected:
    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    bool is_valid(Context::Ptr context) const override;
};


/*!
 * @brief Cpu temperature telemetry reader for Purley platform
 */
class PurleyCpuTemperatureTelemetryReader : public PurleyCpuDimmTemperatureTelemetryReader {
public:

    /*!
     * @brief Create cpu temperature reader for Purley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    PurleyCpuTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        PurleyCpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController&) override;
};


/*!
 * @brief Margin to throttle telemetry reader for Purley platform
 */
class PurleyCpuMarginToThrottleTelemetryReader : public PurleyCpuDimmTemperatureTelemetryReader {
public:

    /*!
     * @brief Create margin to throttle reader for Purley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    PurleyCpuMarginToThrottleTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        PurleyCpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController&) override;
};


/*!
 * @brief Dimm temperature telemetry reader for Purley platform
 */
class PurleyDimmTemperatureTelemetryReader : public PurleyCpuDimmTemperatureTelemetryReader {
public:

    /*!
     * @brief Create dimm temperature reader for Purley platform.
     *
     * @param _resource_key is used to determine the property (from the command) to be taken
     * @param _metric_definition metric definition instance to be used
     */
    PurleyDimmTemperatureTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
        PurleyCpuDimmTemperatureTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController&) override;
};

}
