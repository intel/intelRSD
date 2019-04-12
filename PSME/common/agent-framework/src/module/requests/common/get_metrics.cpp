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
 * @file get_metrics_collection.cpp
 */

#include "agent-framework/module/requests/common/get_metrics.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;


GetMetrics::GetMetrics() {}


GetMetrics::GetMetrics(const std::string& metric_uuid) : m_metric{metric_uuid} {}


json::Json GetMetrics::to_json() const {
    json::Json json = json::Json();
    json[literals::Metric::METRIC] = m_metric;
    json[literals::Metric::COMPONENT] = m_component;
    json[literals::MetricDefinition::METRIC_DEFINITION] = m_metric_definition;
    json[literals::Metric::NAME] = m_name;
    return json;
}


GetMetrics GetMetrics::from_json(const json::Json& json) {
    GetMetrics request;
    if (json.count(literals::Metric::METRIC)) {
        request.set_metric(json[literals::Metric::METRIC]);
    }
    if (json.count(literals::Metric::COMPONENT)) {
        request.set_component(json[literals::Metric::COMPONENT]);
    }
    if (json.count(literals::Metric::METRIC_DEFINITION)) {
        request.set_metric_definition(json[literals::Metric::METRIC_DEFINITION]);
    }
    if (json.count(literals::Metric::NAME)) {
        request.set_name(json[literals::Metric::NAME]);
    }
    return request;
}
