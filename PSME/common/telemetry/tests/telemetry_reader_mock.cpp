/*!
 * @brief Telemetry reader mock implementation
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
 * @file telemetry_reader_mock.cpp
 */

#include "telemetry_reader_mock.hpp"

namespace telemetry {

TelemetryReaderValuesFromArray::TelemetryReaderValuesFromArray(ResourceInstance _resource_key,
                                                               MetricDefinition& _metric_definition,
                                                               const std::vector<json::Json>& _values)
    : TelemetryReader(_resource_key, _metric_definition, TelemetryReaderValuesFromArray::assigned_reader_id()),
      m_values(_values) {}


TelemetryReaderValuesFromArray::~TelemetryReaderValuesFromArray() {}


TelemetryReader::ReaderId TelemetryReaderValuesFromArray::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}


bool TelemetryReaderValuesFromArray::read(Context::Ptr, ipmi::IpmiController&) {
    if (m_last_idx > m_values.size()) {
        return update_value(nullptr);
    }
    return update_value(m_values[m_last_idx++]);
}


} // telemetry
