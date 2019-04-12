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
 * @file is_requested_metric.cpp
 */

#include "agent-framework/module/model/metric.hpp"
#include "agent-framework/module/requests/common/get_metrics.hpp"
#include "agent-framework/module/utils/is_requested_metric.hpp"

using Metric = agent_framework::model::Metric;
using GetMetrics = agent_framework::model::requests::GetMetrics;

namespace {
bool is_requested_metric(const GetMetrics& request, const Metric& metric) {
    if ( (request.get_metric().has_value() && request.get_metric() != metric.get_uuid()) ||
         (request.get_component().has_value() && request.get_component() != metric.get_component_uuid()) ||
         (request.get_metric_definition().has_value() &&
             request.get_metric_definition() != metric.get_metric_definition_uuid()) ||
         (request.get_name().has_value() && request.get_name() != metric.get_name()) ) {
        return false;
    }
    return true;
}
}


std::function<bool(const Metric&)>
agent_framework::model::utils::is_requested_metric_filter(const GetMetrics& request) {
    return std::bind(is_requested_metric, request, std::placeholders::_1);
}
