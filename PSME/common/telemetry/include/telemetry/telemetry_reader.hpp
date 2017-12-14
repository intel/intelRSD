/*!
 * @brief Base class for all telemetry readers
 *
 * Each object derived from this class represents metric (value).
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file telemetry_reader.hpp
 */

#pragma once

#include "telemetry/resource_key.hpp"
#include "agent-framework/module/model/metric_definition.hpp"
#include "agent-framework/module/model/metric.hpp"
#include "ipmi/ipmi_controller.hpp"

#include <vector>
#include <memory>
#include <chrono>
#include <list>

namespace telemetry {

/* forward declaration for friend declaration */
class MetricsProcessor;

class TelemetryReader {

    friend class MetricsProcessor;

public:
    using MetricDefinition = agent_framework::model::MetricDefinition;
    using Metric = agent_framework::model::Metric;

    using SampleTime = std::chrono::time_point<std::chrono::steady_clock>;

    class Context {
    public:
        using Ptr = std::shared_ptr<Context>;

        virtual ~Context();

        /*!
         * @brief Update context data
         * @return false if nothing has changed
         */
        virtual bool update() = 0;
    };

    using Ptr = std::shared_ptr<TelemetryReader>;
    using PtrVector = std::vector<Ptr>;
    using ReaderId = std::uint16_t;

    enum class State {
        NOT_VALID,   //!< sensor not initialized or has wrong configuration
        VALUE_NOT_PRESENT, //!< value was not read (after initialization/data not available)
        VALUE_READ         //!< value was read, should be reported
    };


    /*!
     * @brief Reader "name", to be used in logging, etc.
     * @return reader name
     */
    virtual std::string get_info() const;


    /*!
     * @brief Get reader type
     * @return reader type
     * @warning Readers are grouped by the type, each reader should have uniq type!
     */
    ReaderId get_reader_id() const { return reader_id; }


    /*!
     * @brief Get resource key to identify the resource
     * @return related resource key
     */
    const ResourceInstance& get_resource_key() const { return resource_key; }

    /*!
     * @brief Get metric definition
     * @return metric definition object reference.
     */
    const MetricDefinition& get_metric_definition() const {
        return metric_definition;
    }

    template<typename R>
    void fill_component(const R& resource) {
        metric.set_component_uuid(resource.get_uuid());
        metric.set_component_type(resource.get_component());
    }

    /*!
     * @brief Get metric definition of this reader
     * @param uuid_generator UUID generator
     * @return MetricDefinition object reference
     */
    template<typename U>
    const MetricDefinition& get_metric_definition(const U& uuid_generator) {
        if (!metric_definition.has_persistent_uuid()) {
            uuid_generator.stabilize(metric_definition);
        }
        return metric_definition;
    }


    /*!
     * @brief Get metric represented by this reader
     * @param uuid_generator UUID generator
     * @param resource Metric's resource
     * @return Metric object reference
     */
    template<typename U, typename R>
    const Metric& get_metric(const U& uuid_generator, const R& resource) {
        static_assert(std::is_base_of<agent_framework::model::Resource, R>::value, "Invalid resource type");
        get_metric_definition(uuid_generator);
        metric.set_metric_definition_uuid(metric_definition.get_uuid());
        metric.set_name(metric_definition.get_metric_jsonptr());
        uuid_generator.stabilize(metric, static_cast<const agent_framework::model::Resource&>(resource));
        return metric;
    }


    /*!
     * @brief Get metric UUID
     * @return metric UUID
     */
    const std::string& get_metric_uuid() const {
        return metric.get_uuid();
    }


    /*!
     * @brief Get metric's component UUID
     * @return metric's component UUID
     */
    const std::string& get_resource_uuid() const {
        return metric.get_component_uuid();
    }


    /*!
     * @brief Get reader state
     * @return reader state
     */
    State get_reader_state() const { return reader_state; }


    const json::Json& get_value() const {
        return metric.get_value();
    }

    /*! If reader fills metric data */
    bool fills_metric() const {
        return (filled_properties & FILL_METRIC) == FILL_METRIC;
    }

    /*! If reader fills health of the resource */
    bool fills_health() const {
        return (filled_properties & FILL_HEALTH) == FILL_HEALTH;
    }

    /*!
     * @brief Get health of the resource
     * @return health to be set in the resource
     */
    agent_framework::model::enums::Health::base_enum get_health() const {
        return m_health;
    }

    /*!
     * @brief Get time when reader data should be calculated
     * @return next update time point
     */
    SampleTime get_time_to_update() const {
        return next_time_to_update;
    }

    /*!
     * @brief Set sensing interval if not defined in metric definition
     * @param interval to be set
     */
    void set_sensing_interval(const std::string& interval);

    /*!
     * @brief Set event shore up period in metric definition
     * @param period to be set [s]
     */
    void set_shoreup_period(double period);

    /*!
     * @brief Set event shore up period in metric definition
     * @param period to be set, should follow ISO8601
     */
    void set_shoreup_period(const std::string& period);

    /*!
     * @brief Set metric definition properties
     * @param properties to be set
     */
    void set_metric_definition_properties(const json::Json& properties);

    /*!
     * @brief Check if there was context update for the reader
     * @return true if reader is to be read when update_time is passed
     */
    bool is_to_be_read() const {
        return to_be_read;
    }

    /*!
     * @brief Check if reader is being processed
     * @return true if is context-valid and has next computation time set.
     */
    bool is_being_processed() const;

protected:
    /*!
     * @brief Prepare configuration for the reader.
     *
     * It is called only once, context is common for all readers of same type.
     *
     * @param ctrl IPMI controller
     * @param readers list of all readers defined for the platform. It is necessary for
     *                "synthetized sensors" (which takes data from other metrics)
     * @return shared pointer to just created configuration
     */
    virtual Context::Ptr create_context(ipmi::IpmiController& ctrl, PtrVector& readers);


    /*!
     * @brief Check reader is valid according given context and internal reader rules.
     * @param context "configuration" of all readers
     * @return true if reader instance is valid and ready to use.
     */
    virtual bool is_valid(Context::Ptr context) const;


    /*!
     * @brief Get reading value base on context and IPMI communication
     * @param context common context for all readers of the class, to get appropriate values
     * @param ctrl IPMI controller to handle commands
     * @return true if reading has changed
     */
    virtual bool read(Context::Ptr context, ipmi::IpmiController& ctrl) = 0;


protected:
    /*!
     * @brief Assign reader id for new reader type (class)
     * @return unique (per class) reader id
     */
    static ReaderId assign_reader_id();

    /*!
     * @brief Flags to be passed as "target" for the reader
     * @{
     */
    static constexpr unsigned FILL_METRIC = 0x01;
    static constexpr unsigned FILL_HEALTH = 0x02;
    /*! @} */

    /*!
     * @brief Create temperature reader for resource.
     *
     * @param _resource_key is used to determine the resource to set the read value
     * @param _metric_definition metric definition instance to be used
     * @param _reader_id type (identifier) of the reader
     */
    TelemetryReader(ResourceInstance _resource_key, MetricDefinition& _metric_definition,
                    ReaderId _reader_id, unsigned _filled_properties = FILL_METRIC) :
        resource_key(_resource_key), metric_definition(_metric_definition), reader_id(_reader_id),
        filled_properties(_filled_properties) {}

    virtual ~TelemetryReader() {}

    TelemetryReader(const TelemetryReader&) = default;

    /*!
     * @brief Set read value
     * @param raw_value locally kept value (to be compared with read one)
     * @param read_value just read value to be set
     * @param no_reading_value "special" value, which indicates no value is available
     * @param convert function to convert the value
     * @return true if previous value was changed
     */
    template<typename T>
    bool set_value(T& raw_value, T read_value, T no_reading_value,
                   std::function<json::Json(T)> convert = [](T val) -> json::Json { return val; }) {

        if (read_value != no_reading_value) {
            return set_value(raw_value, read_value, convert);
        }
        else {
            return clear_value();
        }
    }

    /*!
     * @brief Set read value
     * @param raw_value locally kept value (to be compared with read one)
     * @param read_value just read value to be set
     * @param convert function to convert the value
     * @return true if previous value was changed
     */
    template<typename T>
    bool set_value(T& raw_value, T read_value,
                   std::function<json::Json(T)> convert = [](T val) -> json::Json { return val; }) {

        if (get_reader_state() == State::VALUE_NOT_PRESENT) {
            set_reader_state(State::VALUE_READ);
            raw_value = read_value;

            json::Json converted = convert(read_value);
            if (is_computable()) {
                /* converted value IS same as processed with history.. there is exactly one sample */
                process_values_with_historical(converted);
            }
            set_value(nullptr);
            return set_rounded(converted);
        }
        else if (is_computable()) {
            /* add read value to the values and compute average from all collected values */
            json::Json computed = process_values_with_historical(convert(read_value));
            raw_value = read_value;
            return set_rounded(computed);
        }
        else if (raw_value != read_value) {
            raw_value = read_value;
            return set_rounded(convert(read_value));
        }
        return false;
    }

    /*!
     * @brief Clear reader state
     *
     * Should be called when data cannot be read
     *
     * @return true if previosly value was read.
     */
    bool clear_value() {
        if (get_reader_state() == State::VALUE_READ) {
            if (is_computable()) {
                samples.clear();
            }
            set_reader_state(State::VALUE_NOT_PRESENT);
            return true;
        }
        return false;
    }

    /*!
     * @brief Set unconditional reader checked condition
     * @param should_be_read if reader should process context in next run
     */
    void set_to_be_read(bool should_be_read) {
        to_be_read = should_be_read;
    }

    /*!
     * @brief Alter reader state on the data
     * @param state to be set
     */
    void set_reader_state(State state) { reader_state = state; }


    /*!
     * @brief Set health of the resource.
     * @param health to be set
     * @warning Set only when reader fills health
     */
    void set_health(agent_framework::model::enums::Health::base_enum health) {
        m_health = health;
    }

    /*!
     * @brief Set value in the metric
     * @param value new value
     */
    void set_value(const json::Json& value) {
        metric.set_value(value);
    }

    /*!
     * @brief Metric definition to be modified by the reader
     */
    MetricDefinition metric_definition{};

private:
    TelemetryReader() = delete;

    TelemetryReader& operator=(const TelemetryReader&) = delete;

    const ResourceInstance resource_key;
    const ReaderId reader_id;

    State reader_state{State::NOT_VALID};

    unsigned filled_properties;
    agent_framework::model::enums::Health::base_enum m_health{};

    /*!
     * @brief Reader update time
     *
     * The time, when value should be updated. If not initialized
     * (first) reading is done immediatelly and appropriate time is set.
     */
    SampleTime next_time_to_update{};
    bool time_to_update_set{false};

    /*!
     * @brief Flag to request processing the reading
     *
     * If context is updated, all readers should process the
     * context to get new value and/or signal value was changed.
     * If flag is not set, reader is "ignored", no calculation is
     * done.
     */
    bool to_be_read{true};

    /*!
     * @brief Computation algorithms
     *
     * If metric is computable, then all values are gathered in the time-stamped list.
     * All necessary computations are done on these.
     * @{
     */
    using Sample = std::pair<double, SampleTime>;
    using Samples = std::list<Sample>;
    Samples samples{};

    json::Json process_values_with_historical(const json::Json& value);
    bool is_computable() const;
    /* @} */

    /*!
     * @brief Set value with respect to calculation precision
     *
     * Numeric values are rouded when calculation precision is set. Other
     * values are only compared to get if changed.
     *
     * @brief value value to be set
     * @return true if the value has changed
     */
    bool set_rounded(const json::Json& value);

    Metric metric{};
};

}
