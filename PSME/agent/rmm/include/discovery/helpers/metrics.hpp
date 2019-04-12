/*!
 * @brief Definitions of template operations on metrics for RMM resources
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file metrics.hpp
 */

#pragma once



#include "discovery/helpers/common.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "telemetry/resource_key.hpp"
#include "tree_stability/rmm_key_generator.hpp"
#include "logger/logger_factory.hpp"



namespace telemetry {
template<>
ResourceInstance get_resource_key<agent_framework::model::Fan>(const agent_framework::model::Fan& fan);

}


namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

namespace telemetry {
/*!
 * @brief Adds metric in model, sends notification
 * @param metric Metric to be added.
 */
void add(const agent_framework::model::Metric& metric, event_collector::EventCollectorInterfacePtr ec);

/*!
 * @brief Updates metric in model, sends notification
 * @param metric Metric to be updated.
 */
void update(const agent_framework::model::Metric& metric, event_collector::EventCollectorInterfacePtr ec);

/*!
 * @brief removes metric from model, sends notification
 * @param metric Metric to be removed.
 */
void remove(const agent_framework::model::Metric& metric, event_collector::EventCollectorInterfacePtr ec);

/*!
 * @brief Adds MetricDefinition in model
 * @param metric_definition MetricDefinition to be added.
 */
void add(const agent_framework::model::MetricDefinition& metric_definition,
                   event_collector::EventCollectorInterfacePtr ec);

/*!
 * Stabilize a Metric or MetricDefinition object.
 *
 * @return Resource persistent UUID.
 */
template<class T, class ... Args>
void stabilize(T& resource, const Args& ... args) {
    resource.set_unique_key(agent::rmm::RmmKeyGenerator::generate_key(resource, args...));
    resource.make_persistent_uuid();
    log_debug("telemetry", "Persistent UUID " << resource.get_uuid() << " generated for "
                                              << T::get_component().to_string() << " " << resource.get_temporary_uuid());
}

/*!
  * @brief Gets the metric matching a resource and a Sensor
  * @param resource A model object
  * @param resource_sensor
  */
template<RmmType TYPE>
agent_framework::model::Metric get_metric(const ModelType<TYPE>& resource, const ResourceSensorPtr resource_sensor) {
    const auto metrics = agent_framework::module::get_manager<agent_framework::model::Metric>().get_entries(
        [&resource, &resource_sensor](const agent_framework::model::Metric& metric) -> bool {
            return metric.get_component_type() == resource.get_component()
                   && metric.get_component_uuid() == resource.get_uuid()
                   && metric.get_metric_definition_uuid() == resource_sensor->get_definition().get_uuid();
        }
    );
    if (metrics.size() != 1) {
        throw std::runtime_error("Invalid number of Metrics (" + std::to_string(metrics.size()) + ") for "
                                 + resource_sensor->get_definition().get_metric_jsonptr() + " reading for "
                                 + resource.get_component().to_string() + " with uuid " + resource.get_uuid() );
    }
    return metrics.front();
}
}

/*!
 * @brief Sensor - matches a resource with all MetricDefinitions of its metrics,
 * contains two methods:
 * update - updates a metric based on current DiscoveryContext and DiscoveryParams
 * post_reading_update - executes any additional resource update related to metric reading. Returns true if
 * the resource changed
 */
template<RmmType TYPE>
class Sensor final : public ResourceSensor  {
    using MetricUpdateFun = std::function<bool(agent_framework::model::Metric&, const DiscoveryContext&,
                                               const DiscoveryParams<TYPE>&)>;
    using ResourceUpdateFun = std::function<bool(ModelType<TYPE>&, const DiscoveryContext&,
                                                 const DiscoveryParams<TYPE>&)>;

public:
    Sensor(::telemetry::ResourceInstance res, const agent_framework::model::MetricDefinition& def,
           MetricUpdateFun _update, ResourceUpdateFun _post_update)
        : ResourceSensor(res, def), update(_update), post_reading_update(_post_update) { }

    virtual ~Sensor() { }

    MetricUpdateFun update;
    ResourceUpdateFun post_reading_update;
};

/*!
 * @brief Builds metrics for given resource.
 *
 * First it checks which metrics are available for given resource,
 * then creates/updates metrics in model.
 *
 * @tparam TYPE rmm resource helper enum type
 * @param resource_uuid uuid of the resource for which metrics are build.
 * @param dc Discovery Context used for discovery
 * @param dp additional parameters used to update the resource or its metrics
 */
template<RmmType TYPE>
void build_metrics(const std::string& resource_uuid, const DiscoveryContext& dc, const DiscoveryParams<TYPE>&) {
    const auto resource = agent_framework::module::get_manager<ModelType<TYPE>>()
        .get_entry(resource_uuid);
    const auto resource_key = ::telemetry::get_resource_key(resource);
    for (auto& resource_sensor : dc.telemetry_service->get_resource_sensors()) {
        if (resource_sensor->get_resource() == resource_key) {

            auto& definition = resource_sensor->get_definition();
            if (!definition.has_persistent_uuid()) {
                telemetry::stabilize(definition, definition.to_json().dump());
            }
            telemetry::add(definition, dc.event_collector);

            agent_framework::model::Metric metric;
            metric.set_component_uuid(resource.get_uuid());
            metric.set_component_type(resource.get_component());
            metric.set_metric_definition_uuid(definition.get_uuid());
            metric.set_name(definition.get_metric_jsonptr());
            telemetry::stabilize(metric, static_cast<const agent_framework::model::Resource&>(resource));
            telemetry::add(metric, dc.event_collector);
        }
    }
}


/*!
 * @brief Updates all metrics for a given resource.
 *
 * First it checks which metrics are available for given resource,
 * then creates/updates metrics in model.
 *
 * @tparam TYPE rmm resource helper enum type
 * @param resource_uuid uuid of the resource for which metrics are build.
 * @param dc Discovery Context used for discovery
 * @param dp additional parameters used to update the resource or its metrics
 * @returns true if the resource changed as a result of updating metric reading; false otherwise
 */
template<RmmType TYPE>
bool update_metrics(ModelType<TYPE>& resource, const DiscoveryContext& dc, const DiscoveryParams<TYPE>& dp) {
    bool changed = false;
    const auto resource_key = ::telemetry::get_resource_key(resource);
    for (const auto& resource_sensor : dc.telemetry_service->get_resource_sensors()) {
        if (resource_sensor->get_resource() == resource_key) {
            auto metric = telemetry::get_metric<TYPE>(resource, resource_sensor);

            auto sensor = std::static_pointer_cast<Sensor<TYPE>>(resource_sensor);
            bool metric_changed = sensor->update(metric, dc, dp);
            if (metric_changed) {
                telemetry::update(metric, dc.event_collector);
            }

            bool resource_changed = sensor->post_reading_update(resource, dc, dp);
            if (resource_changed) {
                changed = true;
            }
        }
    }
    return changed;
}


/*!
 * @brief Delete metrics for given resource.
 *
 * @tparam TYPE rmm resource helper enum type
 * @param dc Discovery Context used for discovery
 * @param resource_uuid uuid of the resource for which metrics are build.
 */
template<RmmType TYPE>
void remove_metrics(const DiscoveryContext& dc, const std::string& resource_uuid) {
    const auto resource = agent_framework::module::get_manager<ModelType<TYPE>>()
        .get_entry(resource_uuid);
    const auto metrics = agent_framework::module::get_manager<agent_framework::model::Metric>().get_entries(
        [&](const agent_framework::model::Metric& metric) -> bool {
            return metric.get_component_type() == resource.get_component()
                   && metric.get_component_uuid() == resource_uuid;
        }
    );
    for (const auto& metric : metrics) {
        telemetry::remove(metric, dc.event_collector);
    }
}

}
}
}
}
