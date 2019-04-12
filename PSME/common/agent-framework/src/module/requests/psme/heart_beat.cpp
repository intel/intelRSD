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
 * @file module/requests/psme/heart_beat.cpp
 *
 * @brief PSME heart_beat request
 * */

#include "agent-framework/module/requests/psme/heart_beat.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::requests;

HeartBeat::HeartBeat(const std::string& gami_id): m_gami_id{gami_id} {}

json::Json HeartBeat::to_json() const {
    json::Json value = json::Json();
    value[literals::HeartBeat::GAMI_ID] = get_gami_id();
    return value;
}

HeartBeat HeartBeat::from_json(const json::Json& value) {
    return HeartBeat{value[literals::HeartBeat::GAMI_ID].get<std::string>()};
}
