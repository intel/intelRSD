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
 * @file telemetry.cpp
 */

#include "psme/rest/constants/telemetry.hpp"


namespace psme {
namespace rest {
namespace constants {

namespace TelemetryService {
const char METRIC_DEFINITIONS[] = "MetricDefinitions";
const char METRIC_REPORT_DEFINITIONS[] = "MetricReportDefinitions";
const char TRIGGERS[] = "Triggers";
}

namespace MetricDefinition {
const char IMPLEMENTATION[] = "Implementation";
const char CALCULABLE[] = "Calculable";
const char UNITS[] = "Units";
const char DATA_TYPE[] = "DataType";
const char IS_LINEAR[] = "IsLinear";
const char METRIC_TYPE[] = "MetricType";
const char WILDCARDS[] = "Wildcards";
const char VALUES[] = "Values";
const char METRIC_PROPERTIES[] = "MetricProperties";
const char CALCULATION_PARAMETERS[] = "CalculationParameters";
const char SOURCE_METRIC[] = "SourceMetric";
const char RESULT_METRIC[] = "ResultMetric";
const char PHYSICAL_CONTEXT[] = "PhysicalContext";
const char SENSOR_TYPE[] = "SensorType";
const char SENSING_INTERVAL[] = "SensingInterval";
const char DISCRETE_VALUES[] = "DiscreteValues";
const char PRECISION[] = "Precision";
const char ACCURACY[] = "Accuracy";
const char CALIBRATION[] = "Calibration";
const char TIMESTAMP_ACCURACY[] = "TimeStampAccuracy";
const char MIN_READING_RANGE[] = "MinReadingRange";
const char MAX_READING_RANGE[] = "MaxReadingRange";
const char CALCULATION_ALGORITHM[] = "CalculationAlgorithm";
const char CALCULATION_TIME_INTERVAL[] = "CalculationTimeInterval";
const char CALCULATION_PRECISION[] = "CalculationPrecision";
const char DISCRETE_METRIC_TYPE[] = "DiscreteMetricType";
}

namespace TelemetryCommon {
const char METRIC_REPORT_DESTINATION[] = "MetricReportDestination";
const char POLLING_INTERVAL_MS[] = "PollingIntervalMilliseconds";
const char METRICS[] = "Metrics";
const char LOG_TO[] = "LogTo";
const char METRIC_VALUE[] = "MetricValue";
const char METRIC_DEFINITION[] = "MetricDefinition";
}


namespace MetricReportDefinition {
const char METRIC_REPORT_TYPE[] = "MetricReportType";
const char METRIC_REPORT_ACTIONS[] = "MetricReportActions";
const char REPORT_INTERVAL_MS[] = "ReportIntervalMilliseconds";
const char TRANSMIT_FORMAT[] = "TransmitFormat";
}

namespace Trigger {
const char TRIGGER_TYPE[] = "TriggerType";
const char TRIGGER_ACTIONS[] = "TriggerActions";
const char NUMERIC_TRIGGERS[] = "NumericTriggers";
const char DISCRETE_TRIGGERS[] = "DiscreteTriggers";
const char VALUE[] = "Value";
const char DIRECTION_OF_CROSSING[] = "DirectionOfCrossing";
const char DWELL_TIME_MS[] = "DwellTimeMilliseconds";
const char VALUES[] = "Values";
}

}
}
}
