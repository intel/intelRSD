/*!
 * @brief Implementation of Memory Throttling Telemetry reader
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
 * @file memory_throttling_telemetry_reader.cpp
 */

#include "telemetry/memory_throttling_telemetry_reader.hpp"
#include "ipmi/command/generic/get_sensor_reading.hpp"
#include "logger/logger_factory.hpp"


using namespace ipmi::command::generic;
using namespace telemetry;

namespace {

const ipmi::BridgeInfo BRIDGE_TO_NM{0x2c, 0x06};

enum MemoryThrottlingSensorNumber : std::uint8_t {
    CPU0_MEMORY_THROTTLING = 52,
    CPU1_MEMORY_THROTTLING = 53
};

using MemoryThrottlingSensorNumbers = std::array<std::uint8_t, 2>;

constexpr MemoryThrottlingSensorNumbers memory_throttling_sensor_numbers = {{
    CPU0_MEMORY_THROTTLING,
    CPU1_MEMORY_THROTTLING
}};

} // namespace

namespace telemetry {

TelemetryReader::ReaderId MemoryThrottlingTelemetryReader::assigned_reader_id() {
    static ReaderId reader_id = TelemetryReader::assign_reader_id();
    return reader_id;
}


MemoryThrottlingTelemetryReader::MemoryThrottlingTelemetryReader(ResourceInstance _resource_key,
                                                                 MetricDefinition& _metric_definition)
    : TelemetryReader(_resource_key, _metric_definition, MemoryThrottlingTelemetryReader::assigned_reader_id()) {}


bool MemoryThrottlingTelemetryReader::read(TelemetryReader::Context::Ptr, ipmi::IpmiController& ctrl) {
    request::GetSensorReading request{};
    response::GetSensorReading response{};

    double accumulated{};

    for (const auto& sensor_number: memory_throttling_sensor_numbers) {
        request.set_sensor_number(sensor_number);
        ctrl.send(request, BRIDGE_TO_NM, response);
        if (response.is_valid_reading()) {
            accumulated += response.get_sensor_reading();
        }
        else {
            return update_value(nullptr);
        }
    }

    // each reading is in range <0;200>, value 1 means 0.5%
    accumulated /= (memory_throttling_sensor_numbers.size() * 2.0);

    return update_value(accumulated);
}

} // telemetry
