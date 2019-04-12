/*!
 * @brief TelemetryService declaration.
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
 * @file compute/include/telemetry/telemetry_service.hpp
 */

#pragma once

#include "telemetry/metric_processor.hpp"
#include "telemetry/resource_key.hpp"

#include "agent-framework/module/model/metric.hpp"
#include "agent-framework/module/model/metric_definition.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"

#include <memory>
#include <vector>

namespace telemetry {

/*!
 * @brief Entry point for telemetry functionality.
 */
class TelemetryService {
public:
    using Ptr = std::shared_ptr<TelemetryService>;

    TelemetryService(ipmi::IpmiController& _ctrl, TelemetryReader::PtrVector&& _reader_ptrs);

    /*!
     * @brief Destructor
     */
    virtual ~TelemetryService();

    /*!
     * @brief Builds metrics for given resource.
     *
     * First it checks which metrics are available for given resource,
     * then creates/updates metrics in model.
     *
     * @param uuid_generator Metric uuid generator
     * @param resource Resource for which metrics are build.
     */
    template<typename U, typename R>
    void build_metrics_for_resource(const U& uuid_generator, const R& resource) {
        const auto resource_key = get_resource_key(resource);
        for (auto& reader : m_metrics_processor.get_reader_ptrs()) {
            if (reader->get_resource_key() == resource_key) {
                if (agent_framework::model::enums::State::Absent != resource.get_status().get_state()) {
                    reader->fill_component(resource);
                    if (reader->fills_metric()) {
                        const auto& metric_definition = reader->get_metric_definition(uuid_generator);
                        const auto& metric = reader->get_metric(uuid_generator, resource);
                        add_or_update(metric_definition);
                        add_or_update(metric);
                    }
                    if (reader->fills_health()) {
                        update_resource_health(*reader);
                    }
                }
                else {
                    remove_metric(*reader);
                }
            }
        }
    }

    TelemetryReader::PtrVector process_all_metrics();

    const TelemetryReader::PtrVector& get_reader_ptrs() const;

    std::chrono::time_point<std::chrono::steady_clock> get_earliest_update_time() const;

private:
    /*!
     * @brief Apply configuration to the readers
     */
    static TelemetryReader::PtrVector configure(TelemetryReader::PtrVector&& readers);

    /*!
     * @brief Adds or updates metric in model
     * @param metric Metric to be added/updated.
     */
    void add_or_update(const agent_framework::model::Metric& metric);

    /*!
     * @brief Adds/Updates MetricDefinition in model
     * @param metric_definition MetricDefinition to be added/updated.
     */
    void add_or_update(const agent_framework::model::MetricDefinition& metric_definition);



    /*!
     * @brief Removes metric from model for given reader
     * @param reader TelemetryReader for which metrics are removed from model.
     */
    void remove_metric(const TelemetryReader& reader);

    /*!
     * @brief Update metric in the resource
     * @param reader source of value to be set
     */
    void update_metrics_model(const TelemetryReader& reader);

    /*!
     * @brief Update health in the resource
     * @param reader source of value to be set
     */
    void update_resource_health(const TelemetryReader& reader);

    /*!
     * @brief Processor to handle readers
     */
    MetricsProcessor m_metrics_processor;

    /*!
     * @brief Notifications to be sent after the run
     */
    agent_framework::model::attribute::EventData::Vector m_notifications{};
};

}
