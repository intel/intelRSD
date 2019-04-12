/*!
 * @brief Class to handle Node Manager sensors for bandwidth
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
 * @file bandwidth_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"

namespace telemetry {

/*!
 * @brief Base class for bandwidth readers
 */
class BandwidthTelemetryReader : public TelemetryReader {
public:
    /*!
     * @brief Get reader id for the bandwidth readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();

    /*!
     * @brief Create bandwidth reader for resource.
     * @param _resource_key Identifies resource for which bandwidth is measured
     * @param _metric_definition metric definition instance to be used
     */
    BandwidthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition)
        : TelemetryReader(_resource_key, _metric_definition, assigned_reader_id()) {}

    std::string get_info() const override { return "Bandwidth"; }

protected:
    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;
};


/*!
 * @brief Cpu bandwidth reader
 */
class CpuBandwidthTelemetryReader : public BandwidthTelemetryReader {
public:

    /*!
     * @brief Create cpu bandwidth reader for resource.
     * @param _resource_key Identifies resource for which bandwidth is measured
     * @param _metric_definition metric definition instance to be used
     */
    CpuBandwidthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition)
        : BandwidthTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;
};


/*!
 * @brief Memory bandwidth reader
 */
class MemoryBandwidthTelemetryReader : public BandwidthTelemetryReader {
public:

    /*!
     * @brief Create memory bandwidth reader for resource.
     * @param _resource_key Identifies resource for which bandwidth is measured
     * @param _metric_definition metric definition instance to be used
     */
    MemoryBandwidthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition)
        : BandwidthTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;
};


/*!
 * @brief Io bandwidth reader
 */
class IoBandwidthTelemetryReader : public BandwidthTelemetryReader {
public:

    /*!
     * @brief Create io bandwidth reader for resource.
     * @param _resource_key Identifies resource for which bandwidth is measured
     * @param _metric_definition metric definition instance to be used
     */
    IoBandwidthTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition)
        : BandwidthTelemetryReader(_resource_key, _metric_definition) {}

protected:

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;
};

}
