/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/attributes/status.hpp"
#include "agent-framework/module/constants/common.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


Status::Status() {}


Status::~Status() {}


Status::Status(const enums::State state, const OptionalField<enums::Health>& health) :
    m_state(state), m_health(health) {}


json::Json Status::to_json() const {
    json::Json json = json::Json();
    json[literals::Status::STATE] = m_state.to_string();
    json[literals::Status::HEALTH] = m_health;
    return json;
}

Status Status::from_json(const json::Json& json) {
    attribute::Status status{};
    status.set_state(enums::State::from_string(json[literals::Status::STATE]));
    status.set_health(json[literals::Status::HEALTH]);
    return status;
}
