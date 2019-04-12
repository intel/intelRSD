/*!
 * @brief MetricBuilder implementation
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
 * @file metric_builder.cpp
 */


#include "discovery/builders/metric_builder.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;

void MetricBuilder::build_default() { }


MetricBuilder& MetricBuilder::update_from_metric_definition(const MetricDefinition& definition) {
    m_obj.set_metric_definition_uuid(definition.get_uuid());
    m_obj.set_name(definition.get_metric_jsonptr());
    return *this;
}


MetricBuilder& MetricBuilder::update_value(const json::Json& value) {
    m_obj.set_value(value);
    return *this;
}
