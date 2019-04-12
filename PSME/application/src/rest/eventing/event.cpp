/*!
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
 * @file event.cpp
 *
 * @brief Implementation of Event class for PSME eventing
 * */

#include "psme/rest/eventing/event.hpp"
#include "psme/rest/utils/time_utils.hpp"
#include "psme/rest/constants/constants.hpp"

#include "json-wrapper/json-wrapper.hpp"
#include "uuid/uuid.hpp"

using psme::rest::eventing::Event;
using namespace psme::rest::utils;
using namespace psme::rest;

Event::Event(EventType type, const std::string& origin_of_condition)
    : m_type(type), m_origin_of_condition(origin_of_condition), m_event_id(make_v1_uuid()) { }

json::Json Event::to_json() const {
    json::Json j(json::Json::object());

    j[constants::Event::EVENT_TYPE] = m_type.to_string();
    j[constants::Event::EVENT_ID] = m_event_id;
    j[constants::Event::EVENT_TIMESTAMP] = TimeUtils::get_time_with_zone(m_timestamp);
    j[constants::Event::SEVERITY] = m_severity;
    j[constants::Event::MESSAGE] = m_message;
    j[constants::Event::MESSAGE_ID] = m_message_id;

    json::Json args(json::Json::array());
    for(const auto& argument : m_message_args) {
        args.push_back(argument);
    }
    j[constants::Event::MESSAGE_ARGS] = args;
    j[constants::Event::ORIGIN_OF_CONDITION][constants::Common::ODATA_ID] = m_origin_of_condition;

    return j;
}
