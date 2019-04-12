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
 * @file calculation_parameters.cpp
 */


#include "agent-framework/module/model/attributes/calculation_parameters.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


CalculationParameters::CalculationParameters() {}


CalculationParameters::~CalculationParameters() {}


json::Json attribute::CalculationParameters::to_json() const {
    json::Json json = json::Json();
    json[literals::CalculationParameters::SOURCE_METRIC] = m_source_metric;
    json[literals::CalculationParameters::RESULT_METRIC] = m_result_metric;
    return json;
}


CalculationParameters CalculationParameters::from_json(const json::Json& json) {
    attribute::CalculationParameters params;
    params.set_source_metric(json[literals::CalculationParameters::SOURCE_METRIC]);
    params.set_result_metric(json[literals::CalculationParameters::RESULT_METRIC]);
    return params;
}
