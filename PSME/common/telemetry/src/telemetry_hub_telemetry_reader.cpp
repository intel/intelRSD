/*!
 * @brief Implementation of Telemetry Hub reader
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
 * @file telemetry_hub_telemetry_reader.cpp
 */

#include "telemetry/telemetry_hub_telemetry_reader.hpp"
#include "ipmi/telemetry/telemetry_hub.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "logger/logger_factory.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <vector>
#include <unordered_map>

using namespace ipmi::command::generic;
using namespace telemetry;
using ipmi::command::sdv::NO_READING_AVAILABLE;

namespace {

using ipmi::command::sdv::MetricId;
using ipmi::command::sdv::Reading;
using ipmi::command::sdv::MeasureType;

template<typename E>
constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

struct HubMetricDefinition final {
    using ConversionFn = json::Json(*)(const std::uint32_t& raw);
    ResourceInstance resource;
    MetricId metric_id;
    const char* name;
    const char* unit;
    ConversionFn conversion_fn;
};

json::Json no_conversion(const std::uint32_t& raw) { return raw; }
json::Json to_watts(const std::uint32_t& raw) { return raw / 4096; }

using HubMetricDefs = std::array<HubMetricDefinition, 8>;

constexpr HubMetricDefs hub_metric_definitions = {{
    {ResourceInstance{ResourceInstance::Component::Processor, 0},
                 MetricId{to_integral(MetricUid::CPU0_AVG_FREQ), MeasureType::LATEST}, "CPU0_AVG_FREQ", "MHz", no_conversion},
    {ResourceInstance{ResourceInstance::Component::Processor, 1},
                 MetricId{to_integral(MetricUid::CPU1_AVG_FREQ), MeasureType::LATEST}, "CPU1_AVG_FREQ", "MHz", no_conversion},
    {ResourceInstance{ResourceInstance::Component::Processor, 0},
                 MetricId{to_integral(MetricUid::CPU0_CONSUMED_POWER), MeasureType::LATEST}, "CPU0_CONSUMED_POWER", "Watts", to_watts},
    {ResourceInstance{ResourceInstance::Component::Processor, 1},
                 MetricId{to_integral(MetricUid::CPU1_CONSUMED_POWER), MeasureType::LATEST}, "CPU1_CONSUMED_POWER", "Watts", to_watts},
    {ResourceInstance{ResourceInstance::Component::Chassis},
                 MetricId{to_integral(MetricUid::AC_POWER), MeasureType::LATEST}, "AC_POWER", "Watts", to_watts},
    {ResourceInstance{ResourceInstance::Component::Chassis},
                 MetricId{to_integral(MetricUid::DC_POWER), MeasureType::LATEST}, "DC_POWER", "Watts", to_watts},
    {ResourceInstance{ResourceInstance::Component::Processor, 0},
                 MetricId{to_integral(MetricUid::CPU0_DRAM_POWER), MeasureType::LATEST}, "CPU0_DRAM_POWER", "Watts", to_watts},
    {ResourceInstance{ResourceInstance::Component::Processor, 1},
                 MetricId{to_integral(MetricUid::CPU1_DRAM_POWER), MeasureType::LATEST}, "CPU1_DRAM_POWER", "Watts", to_watts}
}};

constexpr HubMetricDefinition EMPTY_DEFINITION = {ResourceInstance{}, MetricId{0}, "EMPTY", "", no_conversion};
constexpr std::uint32_t INVALID_INDEX = 0xFFFFFFFF;

const HubMetricDefinition& get_hub_metric_definition(MetricUid uid) {
    for (const auto& metric_def : hub_metric_definitions) {
        if (metric_def.metric_id.m_id == to_integral(uid)) {
            return metric_def;
        }
    }
    return EMPTY_DEFINITION;
}

constexpr std::uint32_t get_reading_index(MetricUid uid, const std::uint32_t idx) {
    return idx < hub_metric_definitions.size()
           ? (hub_metric_definitions[idx].metric_id.m_id == to_integral(uid)) ? idx : get_reading_index(uid, idx + 1)
           : INVALID_INDEX;
}

constexpr std::uint32_t get_reading_index(MetricUid uid) {
    return get_reading_index(uid, 0);
}


class TelemetryHubContext : public TelemetryReader::Context {
public:

    TelemetryHubContext(ipmi::IpmiController& ctrl)
        : m_ipmi{ctrl}, m_telemetry_hub{m_ipmi, m_bridge_to_nm},
          m_package_readings(init_readings()) {
        init_telemetry_hub();
    }

    json::Json get_reading_value(std::uint32_t index) const {
        const auto& reading = m_package_readings.at(index).m_raw_value;
        return reading != NO_READING_AVAILABLE
               ? hub_metric_definitions.at(index).conversion_fn(reading)
               : json::Json();
    }

    bool update() override {
        try {
            const auto readings = m_telemetry_hub.get_package_readings(m_package_id);
            // sanity check
            if (readings.size() != m_package_readings.size()) {
                init_telemetry_hub();
                throw std::runtime_error("Readings size mismatch");
            }
            std::copy(readings.cbegin(), readings.cend(), m_package_readings.begin());
            return true;
        }
        catch(const ipmi::NodeBusyError& e) {
            log_warning("telemetry", e.what());
            return false;
        }
        catch (const ipmi::command::sdv::InvalidPackageGuid& e) {
            log_info("telemetry", e.what() << ": " << m_package_id);
            // after power off packages are cleared -> recreate them
            init_telemetry_hub();
            throw;
        }
    }

private:
    ipmi::IpmiController& m_ipmi;
    ipmi::sdv::TelemetryHub m_telemetry_hub;
    std::uint32_t m_package_id{};
    ipmi::command::sdv::Readings m_package_readings;
    const ipmi::BridgeInfo m_bridge_to_nm{0x2c, 0x06};

    ipmi::command::sdv::Readings init_readings() const {
        ipmi::command::sdv::Readings readings{};
        for (const auto& metric_def : hub_metric_definitions) {
            readings.emplace_back(metric_def.metric_id);
        }
        return readings;
    }

    bool package_exists() {
        m_package_id = 0u;
        for (auto& reading : m_package_readings) {
            reading.m_raw_value = NO_READING_AVAILABLE;
        }
        const auto& package_map = m_telemetry_hub.load_packages();
        for (const auto& package : package_map) {
            if (m_package_readings.size() == package.second.size()
                && std::equal(m_package_readings.cbegin(), m_package_readings.cend(), package.second.cbegin(),
                    [](const Reading& r1, const Reading& r2) {
                        return r1.m_metric_id.m_id == r2.m_metric_id.m_id
                               && r1.m_metric_id.m_measure_type == r2.m_metric_id.m_measure_type;
                    })) {
                m_package_id = package.first;
                log_info("telemetry", " Found existing package id: " << m_package_id);
                return true;
            }
        }
        return false;
    }

    void create_package() {
        ipmi::command::sdv::MetricIdVec metrics{};
        for (const auto& reading : m_package_readings) {
            metrics.emplace_back(reading.m_metric_id);
        }
        m_package_id = m_telemetry_hub.create_package(metrics);
        log_info("telemetry", " Created package id: " << m_package_id);
    }

    void init_telemetry_hub() {
        if (!package_exists()) {
            create_package();
        }
    }
};

} // namespace

namespace telemetry {

TelemetryReader::ReaderId TelemetryHubTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}


TelemetryHubTelemetryReader::TelemetryHubTelemetryReader(MetricDefinition& _metric_definition, MetricUid metric_uid)
    : TelemetryReader(get_hub_metric_definition(metric_uid).resource, _metric_definition, assigned_reader_id()),
      reading_index(get_reading_index(metric_uid)) {}


TelemetryReader::Context::Ptr TelemetryHubTelemetryReader::create_context(ipmi::IpmiController& ctrl,
                                                                    TelemetryReader::PtrVector&) {
    return std::make_shared<TelemetryHubContext>(ctrl);
}


bool TelemetryHubTelemetryReader::is_valid(Context::Ptr) const {
    return reading_index != INVALID_INDEX;
}


bool TelemetryHubTelemetryReader::read(TelemetryReader::Context::Ptr context, ipmi::IpmiController&) {
    TelemetryHubContext& ctx = static_cast<TelemetryHubContext&>(*context);
    return update_value(ctx.get_reading_value(reading_index));
}


TelemetryHubAggregatedTelemetryReader::TelemetryHubAggregatedTelemetryReader(ResourceInstance _resource_key,
                                                                             MetricDefinition& _metric_definition,
                                                                             const MetricsToAggregate& metrics)
    : TelemetryReader(_resource_key, _metric_definition, TelemetryHubTelemetryReader::assigned_reader_id()),
      m_operation(metrics.m_op) {
    for (const auto& metric_uid: metrics.m_metrics) {
        m_readers.push_back(std::unique_ptr<TelemetryHubTelemetryReader>(new TelemetryHubTelemetryReader(_metric_definition, metric_uid)));
    }
}


TelemetryReader::Context::Ptr TelemetryHubAggregatedTelemetryReader::create_context(ipmi::IpmiController& ctrl, PtrVector&) {
    return std::make_shared<TelemetryHubContext>(ctrl);
}


bool TelemetryHubAggregatedTelemetryReader::is_valid(Context::Ptr context) const {
    return  !m_readers.empty()
            && std::all_of(m_readers.cbegin(), m_readers.cend(),
                           [context](const std::unique_ptr<TelemetryHubTelemetryReader>& reader) {
                               return reader->is_valid(context);
                           });
}


bool TelemetryHubAggregatedTelemetryReader::read(Context::Ptr context, ipmi::IpmiController& ctrl) {
    double new_reading{0.0};
    for (auto& reader: m_readers) {
        reader->read(context, ctrl);
        const auto& value = reader->get_value();
        if (value.is_number()) {
            switch (m_operation) {
            case MetricsToAggregate::Operation::SUM:
                new_reading += value.get<double>();
                break;
            default:
                break;
            }
        }
        else {
            return update_value(nullptr);
        }
    }
    return update_value(new_reading);
}


} // telemetry
