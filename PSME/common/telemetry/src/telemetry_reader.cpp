/*!
 * @brief Implementation of base class of all telemetry readers
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
    return std::to_string(static_cast<unsigned>(m_reader_id));
}

bool TelemetryReader::is_being_processed() const {
    return m_next_time_to_update.has_value();
}

void TelemetryReader::set_sensing_interval(const std::string& interval) {
    if (!m_metric_definition.get_sensing_interval().has_value()) {
        m_metric_definition.set_sensing_interval(interval);
    }
}

void TelemetryReader::set_shoreup_period(double period) {
    m_metric_definition.set_shoreup_period(period);
}

void TelemetryReader::set_shoreup_period(const std::string& period) {
    m_metric_definition.set_shoreup_period(period);
}

/* Prepare configuration for the reader. It is called only once (on first occurence) */
TelemetryReader::Context::Ptr TelemetryReader::create_context(ipmi::IpmiController&, TelemetryReader::PtrVector&) {
    return nullptr;
}


bool TelemetryReader::is_valid(TelemetryReader::Context::Ptr) const {
    return true;
}

void TelemetryReader::set_metric_definition_properties(const json::Json& properties) {
    m_metric_definition.fill_from_json(properties);
}


}
