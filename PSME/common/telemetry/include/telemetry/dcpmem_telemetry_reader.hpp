/*!
 * @brief Intel Optane DC Persistent Memory telemetry reader
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
 * @file dcpmem_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"
#include "pmem/structs/dcpmem_structs.hpp"
#include "pmem/memory_topology.hpp"

namespace telemetry {

class DcpmemTelemetryReader final : public TelemetryReader {
public:
    /*!
     * @brief Get reader id for the DCPMEM telemetry reader (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();

    DcpmemTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition) :
                          TelemetryReader(_resource_key, _metric_definition, assigned_reader_id()) {}

    std::string get_info() const override { return "DcpmemTelemetry"; };

protected:
    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    /*! Check reader validity */
    bool is_valid(Context::Ptr context) const override;

    /*! Read all metrics */
    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;

private:
    /*! Translate SMART_INFO structure parameters into Metrics in json format */
    json::Json process_telemetry_reading(const MetricDefinition& metric,
                                         const pmem::structs::SMART_INFO& smart_info,
                                         const pmem::structs::MEMORY_INFO& memory_info);

    /*! Block size in bytes */
    std::uint8_t m_block_size = pmem::MemoryTopology::get_dcpmem_block_size();
};

// List of supported Memory Metrics - naming shall be coherent with MetricDefinition objects defined for ComputeTelemetryService
ENUM(MemoryMetricName, uint32_t, memoryMetricUnsupported,
     memoryLastShutdownSuccess, memoryPredictedMediaLifeLeftPercent, memoryAlarmTripsTemperature, memoryControllerTemperature,
     memoryUptimeSeconds, memoryUnsafeShutdownCount, memoryPowerCycles, memoryPowerOnTimeSeconds,
     memoryCurrentPeriodBlocksRead, memoryCurrentPeriodBlocksWritten,
     memoryCurrentPeriodHostReadRequests, memoryCurrentPeriodHostWriteRequests);
}