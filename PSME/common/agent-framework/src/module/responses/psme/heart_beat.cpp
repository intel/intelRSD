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
 * @file module/responses/psme/heart_beat.cpp
 *
 * @brief PSME heart_beat request
 * */

#include "agent-framework/module/responses/psme/heart_beat.hpp"
#include "agent-framework/module/constants/psme.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::responses;

json::Json HeartBeat::to_json() const {
    json::Json value = json::Json();
    value[literals::HeartBeat::TIME_STAMP] = get_time_stamp();
    value[literals::HeartBeat::MIN_DELAY] = get_min_delay();
    return value;
}

HeartBeat HeartBeat::from_json(const json::Json& value) {
    HeartBeat heart_beat{};
    heart_beat.set_time_stamp(value[literals::HeartBeat::TIME_STAMP]);
    heart_beat.set_min_delay(value[literals::HeartBeat::MIN_DELAY]);
    return heart_beat;
}
