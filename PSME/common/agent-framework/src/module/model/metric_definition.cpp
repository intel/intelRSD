/*!
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
 * @file metric_definition.cpp
 */


#include "agent-framework/module/model/metric_definition.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/utils/iso8601_time_interval.hpp"

using namespace agent_framework::model;
using agent_framework::utils::Iso8601TimeInterval;

const enums::Component MetricDefinition::component = enums::Component::MetricDefinition;
const enums::CollectionName MetricDefinition::collection_name = enums::CollectionName::MetricDefinitions;

MetricDefinition::MetricDefinition(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} { }


MetricDefinition::~MetricDefinition() { }


MetricDefinition MetricDefinition::from_json(const json::Json& json) {
    MetricDefinition metric{};

    /* properties not allowed to be set */
    metric.set_name(json[literals::MetricDefinition::NAME]);
    metric.set_data_type(json[literals::MetricDefinition::DATA_TYPE]);
    metric.set_metric_type(json[literals::MetricDefinition::METRIC_TYPE]);
    metric.set_discrete_metric_type(json[literals::MetricDefinition::DISCRETE_METRIC_TYPE]);
    metric.set_discrete_values(DiscreteValues::from_json(json[literals::MetricDefinition::DISCRETE_VALUES]));

    metric.fill_from_json(json);
    return metric;
}

void MetricDefinition::fill_from_json(const json::Json& json) {
    if (json.count(literals::MetricDefinition::DESCRIPTION) != 0) {
        set_description(json[literals::MetricDefinition::DESCRIPTION]);
    }
    if (json.count(literals::MetricDefinition::IMPLEMENTATION) != 0) {
        set_implementation(json[literals::MetricDefinition::IMPLEMENTATION]);
    }
    if (json.count(literals::MetricDefinition::CALCULABLE) != 0) {
        set_calculable(json[literals::MetricDefinition::CALCULABLE]);
    }
    if (json.count(literals::MetricDefinition::UNITS) != 0) {
        set_units(json[literals::MetricDefinition::UNITS]);
    }
    if (json.count(literals::MetricDefinition::IS_LINEAR) != 0) {
        set_is_linear(json[literals::MetricDefinition::IS_LINEAR]);
    }
    if (json.count(literals::MetricDefinition::WILDCARDS) != 0) {
        set_wildcards(Wildcards::from_json(json[literals::MetricDefinition::WILDCARDS]));
    }
    if (json.count(literals::MetricDefinition::CALCULATION_PARAMETERS) != 0) {
        set_calculation_parameters(CalculationParametersArray::from_json(
            json[literals::MetricDefinition::CALCULATION_PARAMETERS]));
    }
    if (json.count(literals::MetricDefinition::PHYSICAL_CONTEXT) != 0) {
        set_physical_context(json[literals::MetricDefinition::PHYSICAL_CONTEXT]);
    }
    if (json.count(literals::MetricDefinition::SENSOR_TYPE) != 0) {
        set_sensor_type(json[literals::MetricDefinition::SENSOR_TYPE]);
    }
    if (json.count(literals::MetricDefinition::SENSING_INTERVAL) != 0) {
        set_sensing_interval(json[literals::MetricDefinition::SENSING_INTERVAL]);
    }
    if (json.count(literals::MetricDefinition::PRECISION) != 0) {
        set_precision(json[literals::MetricDefinition::PRECISION]);
    }
    if (json.count(literals::MetricDefinition::ACCURACY) != 0) {
        set_accuracy(json[literals::MetricDefinition::ACCURACY]);
    }
    if (json.count(literals::MetricDefinition::CALIBRATION) != 0) {
        set_calibration(json[literals::MetricDefinition::CALIBRATION]);
    }
    if (json.count(literals::MetricDefinition::TIME_STAMP_ACCURACY) != 0) {
        set_time_stamp_accuracy(json[literals::MetricDefinition::TIME_STAMP_ACCURACY]);
    }
    if (json.count(literals::MetricDefinition::MIN_READING_RANGE) != 0) {
        set_min_reading_range(json[literals::MetricDefinition::MIN_READING_RANGE]);
    }
    if (json.count(literals::MetricDefinition::MAX_READING_RANGE) != 0) {
        set_max_reading_range(json[literals::MetricDefinition::MAX_READING_RANGE]);
    }
    if (json.count(literals::MetricDefinition::CALCULATION_ALGORITHM) != 0) {
        set_calculation_algorithm(json[literals::MetricDefinition::CALCULATION_ALGORITHM]);
    }
    if (json.count(literals::MetricDefinition::CALCULATION_TIME_INTERVAL) != 0) {
        set_calculation_time_interval(json[literals::MetricDefinition::CALCULATION_TIME_INTERVAL]);
    }
    if (json.count(literals::MetricDefinition::CALCULATION_PRECISION) != 0) {
        set_calculation_precision(json[literals::MetricDefinition::CALCULATION_PRECISION]);
    }

    /* special property: not in JSON, but handled via config file */
    if (json.count(literals::MetricDefinition::SHOREUP_PERIOD) != 0) {
        if (json[literals::MetricDefinition::SHOREUP_PERIOD].is_string()) {
            set_shoreup_period(json[literals::MetricDefinition::SHOREUP_PERIOD].get<std::string>());
        }
        else if (json[literals::MetricDefinition::SHOREUP_PERIOD].is_number()) {
            set_shoreup_period(json[literals::MetricDefinition::SHOREUP_PERIOD].get<double>());
        }
        else {
            log_error(LOGUSR, "Shore up interval must be either ISO8601 string or seconds as a number");
        }
    }
}


json::Json MetricDefinition::to_json() const {
    json::Json json = json::Json();
    json[literals::MetricDefinition::NAME] = get_name();
    json[literals::MetricDefinition::DESCRIPTION] = get_description();
    json[literals::MetricDefinition::IMPLEMENTATION] = get_implementation();
    json[literals::MetricDefinition::CALCULABLE] = get_calculable();
    json[literals::MetricDefinition::UNITS] = get_units();
    json[literals::MetricDefinition::DATA_TYPE] = get_data_type();
    json[literals::MetricDefinition::IS_LINEAR] = get_is_linear();
    json[literals::MetricDefinition::METRIC_TYPE] = get_metric_type();
    json[literals::MetricDefinition::WILDCARDS] = get_wildcards().to_json();
    json[literals::MetricDefinition::CALCULATION_PARAMETERS] = get_calculation_parameters().to_json();
    json[literals::MetricDefinition::PHYSICAL_CONTEXT] = get_physical_context();
    json[literals::MetricDefinition::SENSOR_TYPE] = get_sensor_type();
    json[literals::MetricDefinition::SENSING_INTERVAL] = get_sensing_interval();
    json[literals::MetricDefinition::DISCRETE_VALUES] = get_discrete_values().to_json();
    json[literals::MetricDefinition::PRECISION] = get_precision();
    json[literals::MetricDefinition::ACCURACY] = get_accuracy();
    json[literals::MetricDefinition::CALIBRATION] = get_calibration();
    json[literals::MetricDefinition::TIME_STAMP_ACCURACY] = get_time_stamp_accuracy();
    json[literals::MetricDefinition::MIN_READING_RANGE] = get_min_reading_range();
    json[literals::MetricDefinition::MAX_READING_RANGE] = get_max_reading_range();
    json[literals::MetricDefinition::CALCULATION_ALGORITHM] = get_calculation_algorithm();
    json[literals::MetricDefinition::CALCULATION_TIME_INTERVAL] = get_calculation_time_interval();
    json[literals::MetricDefinition::DISCRETE_METRIC_TYPE] = get_discrete_metric_type();
    json[literals::MetricDefinition::CALCULATION_PRECISION] = get_calculation_precision();
    return json;
}

void MetricDefinition::set_sensing_interval(const OptionalField<std::string>& sensing_interval) {
    m_sensing_interval = sensing_interval;
    if (sensing_interval.has_value()) {
        m_sensing_period = Iso8601TimeInterval::parse(*sensing_interval);
    }
}

void MetricDefinition::set_calculation_time_interval(const OptionalField<std::string>& calculation_time_interval) {
    m_calculation_time_interval = calculation_time_interval;
    if (calculation_time_interval.has_value()) {
        m_calculation_period = Iso8601TimeInterval::parse(*calculation_time_interval);
    }
}

void MetricDefinition::set_shoreup_period(const std::string& period) {
    m_shoreup_period = Iso8601TimeInterval::parse(period).as<std::chrono::duration<double>>().count();
}

void MetricDefinition::set_shoreup_period(double period) {
    m_shoreup_period = period;
}
