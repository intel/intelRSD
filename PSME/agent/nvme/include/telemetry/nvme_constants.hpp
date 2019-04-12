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
 * @file nvme/include/telemetry/nvme_constants.hpp
 */

#pragma once



#include <chrono>



namespace agent {
namespace nvme {
namespace telemetry {

class Constants {

public:

    static constexpr const std::chrono::seconds TELEMETRY_DELAY{10};

    static constexpr const char TEMPERATURE_KELVIN_JSON_PTR[] = "/TemperatureKelvin";
    static constexpr const char UNIT_SIZE_BYTES_JSON_PTR[] = "/LifeTime/UnitSizeBytes";
    static constexpr const char UNITS_READ_JSON_PTR[] = "/LifeTime/UnitsRead";
    static constexpr const char UNITS_WRITTEN_JSON_PTR[] = "/LifeTime/UnitsWritten";
    static constexpr const char HOST_READ_COMMANDS_JSON_PTR[] = "/LifeTime/HostReadCommands";
    static constexpr const char HOST_WRITE_COMMANDS_JSON_PTR[] = "/LifeTime/HostWriteCommands";
    static constexpr const char POWER_CYCLES_JSON_PTR[] = "/LifeTime/PowerCycles";
    static constexpr const char POWER_ON_HOURS_JSON_PTR[] = "/LifeTime/PowerOnHours";
    static constexpr const char CONTROLLER_BUSY_TIME_MINUTES_JSON_PTR[] = "/LifeTime/ControllerBusyTimeMinutes";
    static constexpr const char AVAILABLE_SPARE_PERCENTAGE_JSON_PTR[] = "/HealthData/AvailableSparePercentage";
    static constexpr const char PREDICTED_MEDIA_LIFE_USED_PERCENT_JSON_PTR[] = "/HealthData/PredictedMediaLifeUsedPercent";
    static constexpr const char UNSAFE_SHUTDOWNS_JSON_PTR[] = "/HealthData/UnsafeShutdowns";
    static constexpr const char MEDIA_ERRORS_JSON_PTR[] = "/HealthData/MediaErrors";
    static constexpr const char LATENCY_BUCKETS_JSON_PTR[] = "/LatencyBuckets";

    static constexpr const std::uint32_t RANGE = 31;

    static constexpr const char FROM[] = "From";
    static constexpr const char TO[] = "To";
    static constexpr const char MICRO_SECONDS[] = "MicroSeconds";
    static constexpr const char MILLI_SECONDS[] = "MilliSeconds";

    static constexpr const char READS_HISTOGRAM[] = "/ReadsLatencyHistogram/";
    static constexpr const char WRITES_HISTOGRAM[] = "/WritesLatencyHistogram/";
};

}
}
}

