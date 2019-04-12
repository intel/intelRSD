/*!
 * @brief Constants for telemetry endpoints
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
 * @file telemetry.hpp
 */

#pragma once

namespace psme {
namespace rest {
namespace constants {

namespace TelemetryService {
extern const char METRIC_DEFINITIONS[];
extern const char METRIC_REPORT_DEFINITIONS[];
extern const char TRIGGERS[];
}

namespace MetricDefinition {
extern const char IMPLEMENTATION[];
extern const char CALCULABLE[];
extern const char UNITS[];
extern const char DATA_TYPE[];
extern const char IS_LINEAR[];
extern const char METRIC_TYPE[];
extern const char WILDCARDS[];
extern const char VALUES[];
extern const char METRIC_PROPERTIES[];
extern const char CALCULATION_PARAMETERS[];
extern const char SOURCE_METRIC[];
extern const char RESULT_METRIC[];
extern const char PHYSICAL_CONTEXT[];
extern const char SENSOR_TYPE[];
extern const char SENSING_INTERVAL[];
extern const char DISCRETE_VALUES[];
extern const char PRECISION[];
extern const char ACCURACY[];
extern const char CALIBRATION[];
extern const char TIMESTAMP_ACCURACY[];
extern const char MIN_READING_RANGE[];
extern const char MAX_READING_RANGE[];
extern const char CALCULATION_ALGORITHM[];
extern const char CALCULATION_TIME_INTERVAL[];
extern const char CALCULATION_PRECISION[];
extern const char DISCRETE_METRIC_TYPE[];
}

namespace TelemetryCommon {
extern const char METRIC_REPORT_DESTINATION[];
extern const char POLLING_INTERVAL_MS[];
extern const char METRICS[];
extern const char LOG_TO[];
extern const char METRIC_VALUE[];
extern const char METRIC_DEFINITION[];
}

namespace MetricReportDefinition {
extern const char METRIC_REPORT_TYPE[];
extern const char METRIC_REPORT_ACTIONS[];
extern const char REPORT_INTERVAL_MS[];
extern const char TRANSMIT_FORMAT[];
}

namespace Trigger {
extern const char TRIGGER_TYPE[];
extern const char TRIGGER_ACTIONS[];
extern const char NUMERIC_TRIGGERS[];
extern const char DISCRETE_TRIGGERS[];
extern const char VALUE[];
extern const char DIRECTION_OF_CROSSING[];
extern const char DWELL_TIME_MS[];
extern const char VALUES[];
}

}
}
}
