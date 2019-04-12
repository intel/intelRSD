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
 * @file get_metric_definition_info.cpp
 */

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/requests/common/get_metric_definition_info.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


GetMetricDefinitionInfo::GetMetricDefinitionInfo(const std::string& metric_definition)
    : m_metric_definition{metric_definition} {}


json::Json GetMetricDefinitionInfo::to_json() const {
    json::Json value = json::Json();
    value[MetricDefinition::METRIC_DEFINITION] = m_metric_definition;
    return value;
}


GetMetricDefinitionInfo GetMetricDefinitionInfo::from_json(const json::Json& json) {
    return GetMetricDefinitionInfo{json[MetricDefinition::METRIC_DEFINITION].get<std::string>()};
}
