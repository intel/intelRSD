/*!
 * @brief Nvme agent's TelemetryService declaration.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file nvme/include/telemetry/nvme_constants.cpp
 */


#include "telemetry/nvme_constants.hpp"



namespace agent {
namespace nvme {
namespace telemetry {

constexpr const std::chrono::seconds Constants::TELEMETRY_DELAY;
constexpr const char Constants::TEMPERATURE_KELVIN_JSON_PTR[];
constexpr const char Constants::UNIT_SIZE_BYTES_JSON_PTR[];
constexpr const char Constants::UNITS_READ_JSON_PTR[];
constexpr const char Constants::UNITS_WRITTEN_JSON_PTR[];
constexpr const char Constants::HOST_READ_COMMANDS_JSON_PTR[];
constexpr const char Constants::HOST_WRITE_COMMANDS_JSON_PTR[];
constexpr const char Constants::POWER_CYCLES_JSON_PTR[];
constexpr const char Constants::POWER_ON_HOURS_JSON_PTR[];
constexpr const char Constants::CONTROLLER_BUSY_TIME_MINUTES_JSON_PTR[];
constexpr const char Constants::AVAILABLE_SPARE_PERCENTAGE_JSON_PTR[];
constexpr const char Constants::PREDICTED_MEDIA_LIFE_USED_PERCENT_JSON_PTR[];
constexpr const char Constants::UNSAFE_SHUTDOWNS_JSON_PTR[];
constexpr const char Constants::MEDIA_ERRORS_JSON_PTR[];
constexpr const char Constants::LATENCY_BUCKETS_JSON_PTR[];

constexpr const char Constants::FROM[];
constexpr const char Constants::TO[];
constexpr const char Constants::MICRO_SECONDS[];
constexpr const char Constants::MILLI_SECONDS[];

constexpr const char Constants::READS_HISTOGRAM[];
constexpr const char Constants::WRITES_HISTOGRAM[];

}
}
}

