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
 * @file metric_definition.hpp
 */

#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

class MetricDefinition : public Resource {
public:
    using Wildcards = attribute::Array<attribute::Wildcard>;
    using CalculationParametersArray = attribute::Array<attribute::CalculationParameters>;
    using DiscreteValues = attribute::Array<std::string>;



    explicit MetricDefinition(const std::string& parent_uuid = {},
                              enums::Component parent_type = enums::Component::None);


    ~MetricDefinition();


    /* Enable copy and move semantics */
    MetricDefinition(const MetricDefinition&) = default;


    MetricDefinition(MetricDefinition&&) = default;


    MetricDefinition& operator=(const MetricDefinition&) = default;


    MetricDefinition& operator=(MetricDefinition&&) = default;


    /*!
     * @brief Construct a MetricDefinition object from json::Json
     * @param[in] json Json object representing a metric
     * @return MetricDefinition object
     */
    static MetricDefinition from_json(const json::Json& json);


    /*!
     * @brief Convert a MetricDefinition object to json::Json
     * @return json::Json object representing a metric
     */
    json::Json to_json() const;

    /*!
     * @brief Set properties from given json
     * @param json JSON object to fill metric definition
     */
    void fill_from_json(const json::Json& json);

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return MetricDefinition::collection_name;
    }


    /*!
     * @brief Get metric component name enum
     * @return MetricDefinition component name enum
     */
    static enums::Component get_component() {
        return MetricDefinition::component;
    }


    /*!
     * @brief Get metric implementation
     * @return metric implementation
     * */
    const OptionalField<enums::MetricImplementation>& get_implementation() const {
        return m_implementation;
    }

    /*!
     * @brief Set metric implementation
     * @param[in] implementation the metric implementation
     * */
    void set_implementation(const OptionalField<enums::MetricImplementation>& implementation) {
        m_implementation = implementation;
    }


    /*!
     * @brief Get calculable
     * @return calculable
     * */
    const OptionalField<enums::MetricCalculable>& get_calculable() const {
        return m_calculable;
    }


    /*!
     * @brief Set calculable
     * @param[in] calculable the calculable
     * */
    void set_calculable(const OptionalField<enums::MetricCalculable>& calculable) {
        m_calculable = calculable;
    }


    /*!
     * @brief Get metric units
     * @return metric units
     * */
    const OptionalField<std::string>& get_units() const {
        return m_units;
    }


    /*!
     * @brief Set metric units
     * @param[in] units the metric units
     * */
    void set_units(const OptionalField<std::string>& units) {
        m_units = units;
    }


    /*!
     * @brief Get metric data type
     * @return metric data type
     * */
    const OptionalField<enums::MetricDataType>& get_data_type() const {
        return m_data_type;
    }


    /*!
     * @brief Set metric data type
     * @param[in] data_type the metric data type
     * */
    void set_data_type(const OptionalField<enums::MetricDataType>& data_type) {
        m_data_type = data_type;
    }


    /*!
     * @brief Get flag determining whether metric is linear
     * @return flag determining whether metric is linear
     * */
    const OptionalField<bool>& get_is_linear() const {
        return m_is_linear;
    }


    /*!
     * @brief Set flag determining whether metric is linear
     * @param[in] is_linear the flag determining whether metric is linear
     * */
    void set_is_linear(const OptionalField<bool>& is_linear) {
        m_is_linear = is_linear;
    }


    /*!
     * @brief Get metric type
     * @return metric type
     * */
    const OptionalField<enums::MetricType>& get_metric_type() const {
        return m_type;
    }


    /*!
     * @brief Set metric type
     * @param[in] type the metric type
     * */
    void set_metric_type(const OptionalField<enums::MetricType>& type) {
        m_type = type;
    }


    /*!
     * @brief Get metric wildcards array
     * @return metric wildcards array
     * */
    const Wildcards& get_wildcards() const {
        return m_wildcards;
    }


    /*!
     * @brief Set metric wildcards array
     * @param[in] wildcards the metric wildcards array
     * */
    void set_wildcards(const Wildcards& wildcards) {
        m_wildcards = wildcards;
    }


    /*!
     * @brief Add member to metric wildcards array
     * @param[in] wildcard the member to add to metric wildcards array
     * */
    void add_wildcard(const attribute::Wildcard& wildcard) {
        m_wildcards.add_entry(wildcard);
    }


    /*!
     * @brief Get metric calculation parameters array
     * @return metric calculation parameters array
     * */
    const CalculationParametersArray& get_calculation_parameters() const {
        return m_calculation_parameters;
    }


    /*!
     * @brief Set metric calculation parameters array
     * @param[in] calculation_parameters the metric calculation parameters array
     * */
    void set_calculation_parameters(const CalculationParametersArray& calculation_parameters) {
        m_calculation_parameters = calculation_parameters;
    }


    /*!
     * @brief Add member to metric calculation parameters array
     * @param[in] calculation_parameter the member to add to metric calculation parameters array
     * */
    void add_calculation_parameter(const attribute::CalculationParameters& calculation_parameter) {
        m_calculation_parameters.add_entry(calculation_parameter);
    }


    /*!
     * @brief Get metric physical context
     * @return metric physical context
     * */
    const OptionalField<enums::PhysicalContext>& get_physical_context() const {
        return m_physical_context;
    }


    /*!
     * @brief Set metric physical context
     * @param[in] physical_context the metric physical context
     * */
    void set_physical_context(const OptionalField<enums::PhysicalContext>& physical_context) {
        m_physical_context = physical_context;
    }


    /*!
     * @brief Get metric sensor type
     * @return metric sensor type
     * */
    const OptionalField<enums::SensorType>& get_sensor_type() const {
        return m_sensor_type;
    }


    /*!
     * @brief Set metric sensor type
     * @param[in] sensor_type the metric sensor type
     * */
    void set_sensor_type(const OptionalField<enums::SensorType>& sensor_type) {
        m_sensor_type = sensor_type;
    }


    /*!
     * @brief Get metric sensing interval
     * @return metric sensing interval
     * */
    const OptionalField<std::string>& get_sensing_interval() const {
        return m_sensing_interval;
    }


    /*!
     * @brief Set metric sensing interval
     * @param[in] sensing_interval the metric sensing interval
     * */
    void set_sensing_interval(const OptionalField<std::string>& sensing_interval);

    /*!
     * @brief Get metric sensing period
     * @return metric sensing interval as seconds
     * @warning it is set properly only if sensing_interval is set.
     */
    const agent_framework::utils::Iso8601TimeInterval& get_sensing_period() const {
        return m_sensing_period;
    }


    /*!
     * @brief Get array of possible values of discrete metric
     * @return array of possible values of discrete metric
     * */
    const DiscreteValues& get_discrete_values() const {
        return m_discrete_values;
    }


    /*!
     * @brief Set array of possible values of discrete metric
     * @param[in] discrete_values the array of possible values of discrete metric
     * */
    void set_discrete_values(const DiscreteValues& discrete_values) {
        m_discrete_values = discrete_values;
    }


    /*!
     * @brief Add member to array of possible values of discrete metric
     * @param[in] discrete_value the member to add to array of possible values of discrete metric
     * */
    void add_discrete_value(const std::string& discrete_value) {
        m_discrete_values.add_entry(discrete_value);
    }


    /*!
     * @brief Get metric precision
     * @return metric precision
     * */
    const OptionalField<std::int64_t>& get_precision() const {
        return m_precision;
    }


    /*!
     * @brief Set metric precision
     * @param[in] precision the metric precision
     * */
    void set_precision(const OptionalField<std::int64_t>& precision) {
        m_precision = precision;
    }


    /*!
     * @brief Get metric accuracy
     * @return metric accuracy
     * */
    const OptionalField<double>& get_accuracy() const {
        return m_accuracy;
    }


    /*!
     * @brief Set metric accuracy
     * @param[in] accuracy the metric accuracy
     * */
    void set_accuracy(const OptionalField<double>& accuracy) {
        m_accuracy = accuracy;
    }


    /*!
     * @brief Get metric calibration
     * @return metric calibration
     * */
    const OptionalField<double>& get_calibration() const {
        return m_calibration;
    }


    /*!
     * @brief Set metric calibration
     * @param[in] calibration the metric calibration
     * */
    void set_calibration(const OptionalField<double>& calibration) {
        m_calibration = calibration;
    }


    /*!
     * @brief Get metric timestamp accuracy
     * @return metric timestamp accuracy
     * */
    const OptionalField<std::string>& get_time_stamp_accuracy() const {
        return m_time_stamp_accuracy;
    }


    /*!
     * @brief Set metric timestamp accuracy
     * @param[in] time_stamp_accuracy the metric timestamp accuracy
     * */
    void set_time_stamp_accuracy(const OptionalField<std::string>& time_stamp_accuracy) {
        m_time_stamp_accuracy = time_stamp_accuracy;
    }


    /*!
     * @brief Get metric minimum reading range
     * @return metric minimum reading range
     * */
    const OptionalField<double>& get_min_reading_range() const {
        return m_min_reading_range;
    }


    /*!
     * @brief Set metric minimum reading range
     * @param[in] min_reading_range the metric minimum reading range
     * */
    void set_min_reading_range(const OptionalField<double>& min_reading_range) {
        m_min_reading_range = min_reading_range;
    }


    /*!
     * @brief Get metric maximum reading range
     * @return metric maximum reading range
     * */
    const OptionalField<double>& get_max_reading_range() const {
        return m_max_reading_range;
    }


    /*!
     * @brief Set metric maximum reading range
     * @param[in] max_reading_range the metric maximum reading range
     * */
    void set_max_reading_range(const OptionalField<double>& max_reading_range) {
        m_max_reading_range = max_reading_range;
    }


    /*!
     * @brief Get metric calculation algorithm
     * @return metric calculation algorithm
     * */
    const OptionalField<enums::MetricAlgorithm>& get_calculation_algorithm() const {
        return m_calculation_algorithm;
    }


    /*!
     * @brief Set metric calculation algorithm
     * @param[in] calculation_algorithm the metric calculation algorithm
     * */
    void set_calculation_algorithm(const OptionalField<enums::MetricAlgorithm>& calculation_algorithm) {
        m_calculation_algorithm = calculation_algorithm;
    }


    /*!
     * @brief Get metric calculation time interval
     * @return metric calculation time interval
     * */
    const OptionalField<std::string>& get_calculation_time_interval() const {
        return m_calculation_time_interval;
    }

    /*!
     * @brief Set metric calculation time interval
     * @param[in] calculation_time_interval the metric calculation time interval
     * */
    void set_calculation_time_interval(const OptionalField<std::string>& calculation_time_interval);

    /*!
     * @brief Get metric calculation period
     * @return metric calculation time as seconds
     * @warning it is set properly only if calculation_time_interval is set.
     */
    const agent_framework::utils::Iso8601TimeInterval& get_calculation_period() const {
        return m_calculation_period;
    }


    /*!
     * @brief Get event shore up period
     * @return shore up period
     */
    double get_shoreup_period() const {
        return m_shoreup_period;
    }

    /*!
     * @brief Set event shore up period
     * @param period shore up period
     */
    void set_shoreup_period(double period);

    /*!
     * @brief Set event shore up period
     * @param period shore up period
     */
    void set_shoreup_period(const std::string& period);


    /*!
     * @brief Get metric definition description
     * @return metric definition description
     * */
    const OptionalField<std::string>& get_description() const {
        return m_description;
    }


    /*!
     * @brief Set metric definition description
     * @param[in] description the metric definition description
     * */
    void set_description(const OptionalField<std::string>& description) {
        m_description = description;
    }


    /*!
     * @brief Get metric definition discrete metric type
     * @return metric definition discrete metric type
     */
    const OptionalField<enums::DiscreteMetricType>& get_discrete_metric_type() const {
        return m_discrete_metric_type;
    }

    /*!
     * @brief Set metric definition discrete metric type
     * @param discrete_metric_type metric definition discrete metric type
     */
    void set_discrete_metric_type(const OptionalField<enums::DiscreteMetricType>& discrete_metric_type) {
        m_discrete_metric_type = discrete_metric_type;
    }


    /*!
     * @brief Get calculation precision
     * @return calculation precision
     */
    const OptionalField<double>& get_calculation_precision() const {
        return m_calculation_precision;
    }

    /*!
     * @brief Set calculation precision
     * @param calculation_precision calculation precision
     */
    void set_calculation_precision(const OptionalField<double>& calculation_precision) {
        m_calculation_precision = calculation_precision;
    }


    /*!
     * @brief Set metric JSON Pointer
     * @param[in] metric_jsonptr Metric JSON Pointer
     * */
    void set_metric_jsonptr(const std::string& metric_jsonptr) {
        m_metric_jsonptr = metric_jsonptr;
    }


    /*!
     * @brief Get metric JSON Pointer
     * @return metric JSON Pointer
     * */
    const std::string& get_metric_jsonptr() const {
        return m_metric_jsonptr;
    }


private:
    OptionalField<enums::MetricImplementation> m_implementation{};
    OptionalField<enums::MetricCalculable> m_calculable{};
    OptionalField<std::string> m_units{};
    OptionalField<enums::MetricDataType> m_data_type{};
    OptionalField<bool> m_is_linear{};
    OptionalField<enums::MetricType> m_type{};
    Wildcards m_wildcards{};
    CalculationParametersArray m_calculation_parameters{};
    OptionalField<enums::PhysicalContext> m_physical_context{};
    OptionalField<enums::SensorType> m_sensor_type{};
    OptionalField<std::string> m_sensing_interval{};
    DiscreteValues m_discrete_values{};
    OptionalField<std::int64_t> m_precision{};
    OptionalField<double> m_accuracy{};
    OptionalField<double> m_calibration{};
    OptionalField<std::string> m_time_stamp_accuracy{};
    OptionalField<double> m_min_reading_range{};
    OptionalField<double> m_max_reading_range{};
    OptionalField<enums::MetricAlgorithm> m_calculation_algorithm{};
    OptionalField<std::string> m_calculation_time_interval{};
    OptionalField<std::string> m_description{};
    OptionalField<enums::DiscreteMetricType> m_discrete_metric_type{};
    OptionalField<double> m_calculation_precision{};

    /* Not serializable */
    std::string m_metric_jsonptr{};
    agent_framework::utils::Iso8601TimeInterval m_calculation_period{};
    agent_framework::utils::Iso8601TimeInterval m_sensing_period{std::chrono::seconds(10)};
    double m_shoreup_period{10.0};


    static const enums::Component component;
    static const enums::CollectionName collection_name;
};

}
}
