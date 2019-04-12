/*!
 * @brief Telemetry reader for memory throttling
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
 * @file memory_throttling_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"


namespace telemetry {


class MemoryThrottlingTelemetryReader : public TelemetryReader {
public:
    /*!
     * @brief Get reader id for the TelemetryHub readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();

    MemoryThrottlingTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition);

    std::string get_info() const override { return "MemoryThrottling"; }

protected:

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;
};


}
