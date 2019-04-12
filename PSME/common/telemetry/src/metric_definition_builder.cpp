/*!
 * @brief Metric definition builder implementation
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
 * @file metric_definition_builder.cpp
 */

#include "telemetry/metric_definition_builder.hpp"

using namespace agent_framework::model;

namespace telemetry {


MetricDefinitionBuilder::MetricDefinitionBuilder(const std::string& metric_jsonptr)
    : m_metric_definition{build_default(metric_jsonptr)} {}


MetricDefinitionBuilder::MetricDefinition MetricDefinitionBuilder::build_default(const std::string& metric_jsonptr) {
    MetricDefinition definition{};
    definition.set_metric_jsonptr(metric_jsonptr);
    return definition;
}


MetricDefinitionBuilder::MetricDefinition MetricDefinitionBuilder::build() const {
    return m_metric_definition;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_name(const std::string& value) {
    m_metric_definition.set_name(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_description(const std::string& value) {
    m_metric_definition.set_description(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_jsonptr(const std::string& value) {
    m_metric_definition.set_metric_jsonptr(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_accuracy(double value) {
    m_metric_definition.set_accuracy(value);
    return *this;
}


MetricDefinitionBuilder&
MetricDefinitionBuilder::set_calculation_algorithm(agent_framework::model::enums::MetricAlgorithm value) {
    m_metric_definition.set_calculation_algorithm(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_calculation_time_interval(const std::string& value) {
    m_metric_definition.set_calculation_time_interval(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_is_linear(bool value) {
    m_metric_definition.set_is_linear(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_calibration(double value) {
    m_metric_definition.set_calibration(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_data_type(agent_framework::model::enums::MetricDataType value) {
    m_metric_definition.set_data_type(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_min_reading_range(double value) {
    m_metric_definition.set_min_reading_range(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_max_reading_range(double value) {
    m_metric_definition.set_max_reading_range(value);
    return *this;
}


MetricDefinitionBuilder&
MetricDefinitionBuilder::set_implementation(agent_framework::model::enums::MetricImplementation value) {
    m_metric_definition.set_implementation(value);
    return *this;
}


MetricDefinitionBuilder&
MetricDefinitionBuilder::set_metric_calculable(agent_framework::model::enums::MetricCalculable value) {
    m_metric_definition.set_calculable(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_metric_type(agent_framework::model::enums::MetricType value) {
    m_metric_definition.set_metric_type(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_discrete_metric_type(agent_framework::model::enums::DiscreteMetricType value) {
    m_metric_definition.set_discrete_metric_type(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::add_discrete_value(const std::string& value) {
    m_metric_definition.add_discrete_value(value);
    return *this;
}


MetricDefinitionBuilder&
MetricDefinitionBuilder::set_physical_context(agent_framework::model::enums::PhysicalContext value) {
    m_metric_definition.set_physical_context(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_sensor_type(agent_framework::model::enums::SensorType value) {
    m_metric_definition.set_sensor_type(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_sensing_interval(const std::string& value) {
    m_metric_definition.set_sensing_interval(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_shoreup_period(const std::string& value) {
    m_metric_definition.set_shoreup_period(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_shoreup_period(double value) {
    m_metric_definition.set_shoreup_period(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_units(const std::string& value) {
    m_metric_definition.set_units(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_precision(int64_t value) {
    m_metric_definition.set_precision(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_calculation_precision(double value) {
    m_metric_definition.set_calculation_precision(value);
    return *this;
}


MetricDefinitionBuilder& MetricDefinitionBuilder::set_time_stamp_accuracy(const std::string& value) {
    m_metric_definition.set_time_stamp_accuracy(value);
    return *this;
}


}
