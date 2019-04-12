/*!
 * @brief Implementation of TelemetryRunner
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file telemetry_runner.cpp
 */

#include "telemetry/telemetry_runner.hpp"
#include "agent-framework/module/common_components.hpp"
#include "status/bmc.hpp"

using namespace agent_framework::module;

namespace {

using telemetry::MetricsProcessor;
using telemetry::TelemetryReader;
using agent_framework::model::Metric;

void print_reader(std::ostream& ss, const TelemetryReader& reader, unsigned line) {
    ss << "[" << line << "] metric=" << reader.get_resource_key()
                                     << reader.get_metric_definition().get_metric_jsonptr();
    if (!get_manager<Metric>().entry_exists(reader.get_metric_uuid())) {
        ss << "[not in db]";
    }
    ss << " resource=" << reader.get_resource_uuid()
       << " def=" << reader.get_metric_definition().get_name() << ":" << reader.get_metric_definition().get_uuid()
       << " = " << reader.get_value();
}

}


std::chrono::steady_clock::time_point::duration telemetry::TelemetryRunner::run(
    agent::compute::Bmc& bmc, std::chrono::steady_clock::time_point::duration interval) {

    auto telemetry = bmc.telemetry();
    if (bmc.get_state() == agent::compute::Bmc::State::ONLINE && telemetry) {
        const auto start = TelemetryReader::TimePoint::clock::now();

        ++m_runs_counter;
        TelemetryReader::PtrVector changed = telemetry->process_all_metrics();
        if (!changed.empty()) {
            m_changes_counter++;
            log_debug("telemetry", bmc.get_id() << " run #" << m_runs_counter << "/" << m_changes_counter
                                                           << ":: changed " << changed.size() << " metrics");
            unsigned line = 0;
            for (const auto& reader : telemetry->get_reader_ptrs()) {
                bool found = false;
                for (const auto& checked : changed) {
                    if (reader.get() == checked.get()) {
                        found = true;
                        break;
                    }
                }
                std::ostringstream ss;
                ss << bmc.get_id() << " ";
                print_reader(ss, *reader, line++);
                if (found) {
                    ss << "*";
                }
                log_debug("telemetry", ss.str());
            }
        }
        auto time = telemetry->get_earliest_update_time();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time - start);
        log_debug("telemetry", "Next reading shall be done in " << millis.count() << "ms");

        return std::chrono::duration_cast<std::chrono::steady_clock::time_point::duration>(time - start);
    }
    else {
        return interval;
    }
}
