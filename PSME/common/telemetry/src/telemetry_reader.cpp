/*!
 * @brief Implementation of base class of all telemetry readers
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file telemetry_reader.cpp
 */

#include "telemetry/telemetry_reader.hpp"
#include "agent-framework/module/enum/common.hpp"

#include <atomic>
#include <map>
#include <set>
#include <iostream>

namespace telemetry {


TelemetryReader::Context::~Context() {}


TelemetryReader::ReaderId TelemetryReader::assign_reader_id() {
    static std::atomic<ReaderId> reader_id{0};
    return reader_id++;
}


std::string TelemetryReader::get_info() const {
    return std::to_string(static_cast<unsigned>(reader_id));
}

bool TelemetryReader::is_being_processed() const {
    if (!time_to_update_set) {
        return false;
    }
    return get_reader_state() != State::NOT_VALID;
}

void TelemetryReader::set_sensing_interval(const std::string& interval) {
    if (!metric_definition.get_sensing_interval().has_value()) {
        metric_definition.set_sensing_interval(interval);
    }
}

void TelemetryReader::set_shoreup_period(double period) {
    metric_definition.set_shoreup_period(period);
}

void TelemetryReader::set_shoreup_period(const std::string& period) {
    metric_definition.set_shoreup_period(period);
}

/* Prepare configuration for the reader. It is called only once (on first occurence) */
TelemetryReader::Context::Ptr TelemetryReader::create_context(ipmi::IpmiController&, TelemetryReader::PtrVector&) {
    return nullptr;
}


bool TelemetryReader::is_valid(TelemetryReader::Context::Ptr) const {
    return true;
}

bool TelemetryReader::is_computable() const {
    if (!metric_definition.get_calculation_algorithm().has_value()) {
        return false;
    }
    if (!metric_definition.get_calculation_time_interval().has_value()) {
        return false;
    }
    return true;
}


json::Json TelemetryReader::process_values_with_historical(const json::Json& value) {
    if (!value.is_number()) {
        return value;
    }

    using Seconds = std::chrono::duration<double, std::ratio<1>>;

    /* add current sample */
    SampleTime time = std::chrono::steady_clock::now();
    samples.push_back({value, time});

    /* remove "old" samples */
    Seconds from_definition = metric_definition.get_calculation_period();
    while ((!samples.empty()) && (samples.front().second + from_definition < time)) {
        samples.pop_front();
    }

    /* computation cannot be done if there is too few data! */
    if (samples.size() < 2) {
        return value;
    }

    switch (*metric_definition.get_calculation_algorithm()) {
        case agent_framework::model::enums::MetricAlgorithm::AverageOverInterval: {
            /* compute new value from all entries */
            std::chrono::time_point<std::chrono::steady_clock> first_time;
            std::chrono::time_point<std::chrono::steady_clock> previous_time;
            double previous_value = 0.0;
            double area = 0.0;
            bool first = true;
            for (const Sample& sample : samples) {
                if (first) {
                    first_time = sample.second;
                    first = false;
                }
                else {
                    area += ((previous_value + sample.first) / 2.0)
                            * std::chrono::duration_cast<Seconds>(sample.second - previous_time).count();
                }
                previous_value = sample.first;
                previous_time = sample.second;
            }
            return area / std::chrono::duration_cast<Seconds>(previous_time - first_time).count();
        }
        case agent_framework::model::enums::MetricAlgorithm::MaximumDuringInterval: {
            double ret = samples.front().first;
            for (const Sample& sample : samples) {
                if (ret < sample.first) {
                    ret = sample.first;
                }
            }
            return ret;
        }
        case agent_framework::model::enums::MetricAlgorithm::MinimumDuringInterval: {
            double ret = samples.front().first;
            for (const Sample& sample : samples) {
                if (ret > sample.first) {
                    ret = sample.first;
                }
            }
            return ret;
        }
        default:
            throw std::runtime_error("Not handled metric algorithm");
    }
}

bool TelemetryReader::set_rounded(const json::Json& value) {
    /* common error if any conversion returns null */
    if (value.is_null()) {
        log_error("telemetry", metric_definition.get_metric_jsonptr() << ": converted value is null");
    }

    /* only numeric values are checked, calculation precission must be greater than 0 */
    if ((!value.is_number()) || (!metric_definition.get_calculation_precision().has_value())
        || (metric_definition.get_calculation_precision() <= 0.0)) {

        if (get_value() != value) {
            set_value(value);
            return true;
        }
        else {
            return false;
        }
    }

    double calculation_precision = metric_definition.get_calculation_precision().value();
    double rounded = round(double(value) / calculation_precision) * calculation_precision;
    if ((!get_value().is_number()) || (rounded != get_value())) {
        set_value(rounded);
        return true;
    }
    else {
        return false;
    }
}



void TelemetryReader::set_metric_definition_properties(const json::Json& properties) {
    metric_definition.fill_from_json(properties);
}


}
