/*!
 * @brief Definitions of helper methods for metric operations
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
 * @file metrics.cpp
 */

#include "discovery/helpers/metrics.hpp"


namespace telemetry {
template<>
ResourceInstance get_resource_key<agent_framework::model::Fan>(const agent_framework::model::Fan& fan) {
    return ResourceInstance{ResourceInstance::Component::Fan, fan.get_slot_id()};
}
}

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {
namespace telemetry {

void add(const model::Metric& metric, event_collector::EventCollectorInterfacePtr ec) {
    using agent_framework::model::Metric;
    log_info("telemetry", "Added " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid()
                                   << " for " << metric.get_component_type() << " (" << metric.get_component_uuid() << ")");

    agent_framework::module::get_manager<Metric>().add_entry(metric);
    ec->poll_add_event(metric.get_component(), metric.get_uuid(), metric.get_parent_uuid());
}


void update(const model::Metric& metric, event_collector::EventCollectorInterfacePtr ec) {
    using agent_framework::model::Metric;
    log_info("telemetry", "Updated " << Metric::get_component().to_string() << ", UUID " << metric.get_uuid()
                                   << " for " << metric.get_component_type() << " (" << metric.get_component_uuid() << ")");

    agent_framework::module::get_manager<Metric>().get_entry_reference(metric.get_uuid()).get_raw_ref() = metric;
    ec->poll_update_event(metric.get_component(), metric.get_uuid(), metric.get_parent_uuid());
}


void remove(const agent_framework::model::Metric& metric, event_collector::EventCollectorInterfacePtr ec) {
    log_info("telemetry", "Removed " << agent_framework::model::Metric::get_component().to_string()
                                     << ", UUID " << metric.get_uuid() << " for " << metric.get_component_type()
                                     << " (" << metric.get_component_uuid() << ")");

    ec->poll_remove_event(metric.get_component(), metric.get_uuid(), metric.get_parent_uuid());
    agent_framework::module::get_manager<model::Metric>().remove_entry(metric.get_uuid());
}


void add(const agent_framework::model::MetricDefinition& metric_def,
                   event_collector::EventCollectorInterfacePtr ec) {
    using agent_framework::model::MetricDefinition;
    auto& manager = agent_framework::module::get_manager<MetricDefinition>();
    if (!manager.entry_exists(metric_def.get_uuid())) {
        log_info("telemetry",
                 "Added " << MetricDefinition::get_component().to_string() << ", UUID " << metric_def.get_uuid());
        manager.add_entry(metric_def);
        ec->poll_add_event(metric_def.get_component(), metric_def.get_uuid(), metric_def.get_parent_uuid());
    }
}

}
}
}
}
}
