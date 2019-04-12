/*!
 * @brief Base class for all telemetry readers
 *
 * Each object derived from this class represents metric (value).
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
 * @file telemetry_reader.hpp
 */

#pragma once

#include "resource_key.hpp"
#include "value_rounder.hpp"
#include "samples_processor.hpp"
#include "agent-framework/module/model/metric_definition.hpp"
#include "agent-framework/module/model/metric.hpp"
#include "ipmi/ipmi_controller.hpp"

#include <vector>
#include <memory>
#include <chrono>

namespace telemetry {

/* forward declaration for friend declaration */
class MetricsProcessor;

class TelemetryReader {

    friend class MetricsProcessor;

public:
    using MetricDefinition = agent_framework::model::MetricDefinition;
    using Metric = agent_framework::model::Metric;
    using TimePoint = std::chrono::steady_clock::time_point;

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
    ReaderId get_reader_id() const { return m_reader_id; }


    /*!
     * @brief Get resource key to identify the resource
     * @return related resource key
     */
    const ResourceInstance& get_resource_key() const { return m_resource_key; }

    /*!
     * @brief Get metric definition
     * @return metric definition object reference.
     */
    const MetricDefinition& get_metric_definition() const {
        return m_metric_definition;
    }

    template<typename R>
    void fill_component(const R& resource) {
        m_metric.set_component_uuid(resource.get_uuid());
        m_metric.set_component_type(resource.get_component());
    }

    /*!
     * @brief Get metric definition of this reader
     * @param uuid_generator UUID generator
     * @return MetricDefinition object reference
     */
    template<typename U>
    const MetricDefinition& get_metric_definition(const U& uuid_generator) {
        if (!m_metric_definition.has_persistent_uuid()) {
            uuid_generator.stabilize(m_metric_definition);
        }
        return m_metric_definition;
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
        m_metric.set_metric_definition_uuid(m_metric_definition.get_uuid());
        m_metric.set_name(m_metric_definition.get_metric_jsonptr());
        uuid_generator.stabilize(m_metric, static_cast<const agent_framework::model::Resource&>(resource));
        return m_metric;
    }


    /*!
     * @brief Get metric UUID
     * @return metric UUID
     */
    const std::string& get_metric_uuid() const {
        return m_metric.get_uuid();
    }


    /*!
     * @brief Get metric's component UUID
     * @return metric's component UUID
     */
    const std::string& get_resource_uuid() const {
        return m_metric.get_component_uuid();
    }

    const json::Json& get_value() const {
        return m_metric.get_value();
    }

    /*! If reader fills metric data */
    bool fills_metric() const {
        return (m_filled_properties & FILL_METRIC) == FILL_METRIC;
    }

    /*! If reader fills health of the resource */
    bool fills_health() const {
        return (m_filled_properties & FILL_HEALTH) == FILL_HEALTH;
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
    const OptionalField<TimePoint>& get_time_to_update() const {
        return m_next_time_to_update;
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
        return m_to_be_read;
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
        m_resource_key(_resource_key), m_metric_definition(_metric_definition), m_reader_id(_reader_id),
        m_filled_properties(_filled_properties) {
        // value rounder
        if (m_metric_definition.get_calculation_precision().has_value()
            && m_metric_definition.get_calculation_precision().value() > 0) {
            m_value_rounder.reset(new ValueRounder<>(m_metric_definition.get_calculation_precision().value()));
        }
        // samples processor
        if (m_metric_definition.get_calculation_algorithm().has_value()
            && m_metric_definition.get_calculation_time_interval().has_value()) {
            auto calculation_interval = m_metric_definition.get_calculation_period().as<typename SamplesProcessor<>::TimePoint::duration>();
            auto sensing_interval = m_metric_definition.get_sensing_period().as<typename SamplesProcessor<>::TimePoint::duration>();
            auto algorithm = select_algorithm<typename SamplesProcessor<>::Samples>(*m_metric_definition.get_calculation_algorithm());
            m_samples_processor.reset(new SamplesProcessor<>(calculation_interval, sensing_interval, algorithm));
        }
    }

    virtual ~TelemetryReader() {}


    /*!
     * @brief Update value
     * @param value new value to be set
     * @return true if previous value was changed
     */
    bool update_value(json::Json value) {
        if (m_samples_processor) {
            value = m_samples_processor->add_and_process_samples(value);
        }
        if (m_value_rounder) {
            value = (*m_value_rounder)(value);
        }
        if (get_value() != value) {
            set_value(value);
            return true;
        }
        return false;
    }

    /*!
     * @brief Set unconditional reader checked condition
     * @param should_be_read if reader should process context in next run
     */
    void set_to_be_read(bool should_be_read) {
        m_to_be_read = should_be_read;
    }


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
        m_metric.set_value(value);
    }

    /*!
     * @brief Metric definition to be modified by the reader
     */
    MetricDefinition& m_metric_definition;

private:
    TelemetryReader() = delete;
    TelemetryReader(const TelemetryReader&) = delete;
    TelemetryReader(const TelemetryReader&&) = delete;
    TelemetryReader& operator=(const TelemetryReader&) = delete;
    TelemetryReader& operator=(const TelemetryReader&&) = delete;

    const ResourceInstance m_resource_key;
    const ReaderId m_reader_id;
    unsigned m_filled_properties;
    agent_framework::model::enums::Health::base_enum m_health{};

    /*!
     * @brief Reader update time
     *
     * The time, when value should be updated. If not initialized
     * (first) reading is done immediatelly and appropriate time is set.
     */
    OptionalField<TimePoint> m_next_time_to_update{};

    /*!
     * @brief Flag to request processing the reading
     *
     * If context is updated, all readers should process the
     * context to get new value and/or signal value was changed.
     * If flag is not set, reader is "ignored", no calculation is
     * done.
     */
    bool m_to_be_read{true};

    /*!
     * @brief Computation algorithms
     *
     * If metric is computable, then all values are gathered in the time-stamped list.
     * All necessary computations are done on these.
     * @{
     */
    std::unique_ptr<SamplesProcessor<>> m_samples_processor{};
    std::unique_ptr<ValueRounder<>> m_value_rounder{};
    /* @} */

    Metric m_metric{};
};

}
