/*!
 * @brief Metric definition builder
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
 * @file metric_definition_builder.hpp
 */

#pragma once

#include "agent-framework/module/model/metric_definition.hpp"
#include "logger/logger_factory.hpp"

namespace telemetry {

class MetricDefinitionBuilder {
    using MetricDefinition = agent_framework::model::MetricDefinition;
public:
    MetricDefinitionBuilder(const std::string& metric_jsonptr);

    MetricDefinition build() const;

    MetricDefinitionBuilder& set_name(const std::string& value);
    MetricDefinitionBuilder& set_description(const std::string& value);
    MetricDefinitionBuilder& set_jsonptr(const std::string& value);
    MetricDefinitionBuilder& set_implementation(agent_framework::model::enums::MetricImplementation value);
    MetricDefinitionBuilder& set_metric_calculable(agent_framework::model::enums::MetricCalculable value);
    MetricDefinitionBuilder& set_units(const std::string& value);
    MetricDefinitionBuilder& set_data_type(agent_framework::model::enums::MetricDataType value);
    MetricDefinitionBuilder& set_is_linear(bool value);
    MetricDefinitionBuilder& set_metric_type(agent_framework::model::enums::MetricType value);
    MetricDefinitionBuilder& set_discrete_metric_type(agent_framework::model::enums::DiscreteMetricType value);
    MetricDefinitionBuilder& add_discrete_value(const std::string& value);
    MetricDefinitionBuilder& set_physical_context(agent_framework::model::enums::PhysicalContext value);
    MetricDefinitionBuilder& set_sensor_type(agent_framework::model::enums::SensorType value);
    MetricDefinitionBuilder& set_sensing_interval(const std::string& value);
    MetricDefinitionBuilder& set_precision(int64_t value);
    MetricDefinitionBuilder& set_calculation_precision(double value);
    MetricDefinitionBuilder& set_accuracy(double value);
    MetricDefinitionBuilder& set_calibration(double value);
    MetricDefinitionBuilder& set_time_stamp_accuracy(const std::string& value);
    MetricDefinitionBuilder& set_min_reading_range(double value);
    MetricDefinitionBuilder& set_max_reading_range(double value);
    MetricDefinitionBuilder& set_calculation_algorithm(agent_framework::model::enums::MetricAlgorithm value);
    MetricDefinitionBuilder& set_calculation_time_interval(const std::string& value);

    MetricDefinitionBuilder& set_shoreup_period(const std::string& value);
    MetricDefinitionBuilder& set_shoreup_period(double value);

private:
    MetricDefinition build_default(const std::string& metric_jsonptr);
    MetricDefinition m_metric_definition;
};

}
