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
 * @file wildcard.cpp
 */


#include "agent-framework/module/model/attributes/wildcard.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


Wildcard::Wildcard() {}


Wildcard::~Wildcard() {}


json::Json attribute::Wildcard::to_json() const {
    json::Json json = json::Json();
    json[literals::Wildcard::NAME] = m_name;
    json[literals::Wildcard::VALUES] = m_values.to_json();
    return json;
}


Wildcard Wildcard::from_json(const json::Json& json) {
    attribute::Wildcard wildcard;
    wildcard.set_name(json[literals::Wildcard::NAME]);
    wildcard.set_values(Values::from_json(json[literals::Wildcard::VALUES]));
    return wildcard;
}
