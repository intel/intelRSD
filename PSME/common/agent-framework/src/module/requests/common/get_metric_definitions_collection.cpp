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
 * @file get_metric_definitions_collection.cpp
 */

#include "agent-framework/module/requests/common/get_metric_definitions_collection.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::requests;


GetMetricDefinitionsCollection::GetMetricDefinitionsCollection() {}


json::Json GetMetricDefinitionsCollection::to_json() const {
    json::Json json = json::Json();
    return json;
}


GetMetricDefinitionsCollection GetMetricDefinitionsCollection::from_json(const json::Json&) {
    return GetMetricDefinitionsCollection{};
}
