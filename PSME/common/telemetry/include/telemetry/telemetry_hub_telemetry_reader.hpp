/*!
 * @brief Telemetry reader for metrics from telemetry hub
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
 * @file telemetry_hub_telemetry_reader.hpp
 */

#pragma once

#include "telemetry/telemetry_reader.hpp"

#include <vector>

namespace telemetry {


enum class MetricUid : std::uint32_t {
    CPU0_AVG_FREQ = 16793622,
    CPU1_AVG_FREQ = 16793878,
    CPU0_CONSUMED_POWER = 16793600,
    CPU1_CONSUMED_POWER = 16793856,
    AC_POWER = 16795648,
    DC_POWER = 16795649,
    CPU0_DRAM_POWER = 16793601,
    CPU1_DRAM_POWER = 16793857
};

using MetricUidVec = std::vector<MetricUid>;

struct MetricsToAggregate {
    enum class Operation {
        SUM
    };

    MetricsToAggregate(const MetricUidVec& metrics, Operation op = Operation::SUM)
        : m_metrics(metrics), m_op(op) {}

    MetricUidVec m_metrics{};
    Operation m_op{Operation::SUM};
};

class TelemetryHubAggregatedTelemetryReader;

class TelemetryHubTelemetryReader : public TelemetryReader {
    friend class TelemetryHubAggregatedTelemetryReader;
public:
    /*!
     * @brief Get reader id for the TelemetryHub readers (statically assigned)
     * @return reader id
     */
    static ReaderId assigned_reader_id();

    TelemetryHubTelemetryReader(MetricDefinition& _metric_definition, MetricUid metric_uid);

    std::string get_info() const override { return "TelemetryHub"; }

protected:

    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    bool is_valid(Context::Ptr context) const override;

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;

private:
    const std::uint32_t reading_index;
};


/*!
 * @brief Aggregates readings from several TelemetryHubTelemetryReaders
 */
class TelemetryHubAggregatedTelemetryReader: public TelemetryReader {
public:

    TelemetryHubAggregatedTelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                                          const MetricsToAggregate&);

protected:

    Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers) override;

    bool is_valid(Context::Ptr context) const override;

    bool read(Context::Ptr context, ipmi::IpmiController& ctrl) override;

private:
    std::vector<std::unique_ptr<TelemetryHubTelemetryReader>> m_readers{};
    MetricsToAggregate::Operation m_operation;
};


}
