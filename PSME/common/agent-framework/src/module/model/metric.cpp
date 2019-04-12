/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * */

#include "agent-framework/module/model/metric.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/utils/time.hpp"
#include "agent-framework/exceptions/exception.hpp"



using namespace agent_framework::model;

const enums::Component Metric::component = enums::Component::Metric;
const enums::CollectionName Metric::collection_name = enums::CollectionName::Metrics;

Metric::Metric(const std::string& parent_uuid, enums::Component parent_type) : Resource{parent_uuid, parent_type} {
    set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
}


Metric::~Metric() { }


Metric Metric::from_json(const json::Json& json) {
    Metric metric{};

    metric.set_uuid(json[literals::Metric::UUID]);
    metric.set_name(json[literals::Metric::NAME]);
    metric.set_value(json[literals::Metric::VALUE]);
    metric.set_timestamp(json[literals::Metric::TIMESTAMP]);
    metric.set_component_uuid(json[literals::Metric::COMPONENT]);
    metric.set_component_type(enums::Component::from_string(json[literals::Metric::COMPONENT_TYPE]));
    metric.set_metric_definition_uuid(json[literals::Metric::METRIC_DEFINITION]);
    metric.set_status(attribute::Status::from_json(json[literals::Metric::STATUS]));

    return metric;
}


json::Json Metric::to_json() const {
    json::Json json = json::Json();

    json[literals::Metric::UUID] = get_uuid();
    json[literals::Metric::NAME] = get_name();
    json[literals::Metric::VALUE] = get_value();
    json[literals::Metric::TIMESTAMP] = get_timestamp();
    json[literals::Metric::COMPONENT] = get_component_uuid();
    json[literals::Metric::COMPONENT_TYPE] = get_component_type().to_string();
    json[literals::Metric::METRIC_DEFINITION] = get_metric_definition_uuid();
    json[literals::Metric::STATUS] = get_status().to_json();

    return json;
}
