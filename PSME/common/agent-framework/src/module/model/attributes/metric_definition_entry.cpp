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
 * @file metric_definition_entry.cpp
 */


#include "agent-framework/module/model/attributes/metric_definition_entry.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


MetricDefinitionEntry::MetricDefinitionEntry() {}


MetricDefinitionEntry::~MetricDefinitionEntry() {}


json::Json MetricDefinitionEntry::to_json() const {
    json::Json entry = json::Json();
    entry[literals::MetricDefinition::METRIC_DEFINITION] = get_metric_definition();
    return entry;
}


MetricDefinitionEntry MetricDefinitionEntry::from_json(const json::Json& json) {
    MetricDefinitionEntry entry;
    entry.set_metric_definition(json[literals::MetricDefinition::METRIC_DEFINITION]);
    return entry;
}
