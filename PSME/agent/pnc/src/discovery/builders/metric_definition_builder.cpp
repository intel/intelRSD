/*!
 * @brief MetricDefinitionBuilder implementation
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
 * @file metric_definition_builder.cpp
 */


#include "discovery/builders/metric_definition_builder.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;

MetricDefinitionBuilder::MetricDefinitionBuilder() : AbstractBuilder<MetricDefinition>() { }

MetricDefinitionBuilder::~MetricDefinitionBuilder() { }

MetricDefinitionBuilder& MetricDefinitionBuilder::prepare_port_health_definition() {
    // clear current definition
    m_obj = agent_framework::model::MetricDefinition();
    m_obj.set_metric_jsonptr("/Health");
    m_obj.set_name("Fabric Port Health metric");
    m_obj.set_implementation(enums::MetricImplementation::PhysicalSensor);
    m_obj.set_calculable(enums::MetricCalculable::NonCalculable);
    m_obj.set_data_type(enums::MetricDataType::String);
    m_obj.set_is_linear(false);
    m_obj.set_physical_context(enums::PhysicalContext::NetworkingDevice);
    m_obj.set_sensing_interval("PT10S");
    m_obj.set_metric_type(enums::MetricType::Discrete);
    for(const auto& allowable_value : enums::Health::get_values()) {
        m_obj.add_discrete_value(allowable_value);
    }


    for (const auto& value: enums::Health::get_values()) {
        m_obj.add_discrete_value(value);
    }
    return *this;
}
