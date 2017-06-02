/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "json/json.hpp"
#include "uuid++.hh"

using psme::rest::eventing::Event;
using namespace psme::rest::utils;

constexpr const char Event::EVENT_TYPE[];
constexpr const char Event::EVENT_ID[];
constexpr const char Event::EVENT_TIMESTAMP[];
constexpr const char Event::SEVERITY[];
constexpr const char Event::MESSAGE[];
constexpr const char Event::MESSAGE_ID[];
constexpr const char Event::MESSAGE_ARGS[];
constexpr const char Event::CONTEXT[];
constexpr const char Event::ORIGIN_OF_CONDITION[];

Event::Event(EventType type, const std::string& origin_of_condition)
    : m_type(type), m_origin_of_condition(origin_of_condition) {
    uuid u;
    u.make(UUID_MAKE_V1);
    char* uuid_char = u.string();
    m_event_id = std::string{uuid_char};
    free(uuid_char);
}

json::Value Event::to_json() const {
    json::Value j(json::Value::Type::OBJECT);

    j[EVENT_TYPE] = m_type.to_string();
    j[EVENT_ID] = m_event_id;
    j[EVENT_TIMESTAMP] = TimeUtils::get_time_with_zone(m_timestamp);
    j[SEVERITY] = m_severity;
    j[MESSAGE] = m_message;
    j[MESSAGE_ID] = m_message_id;

    json::Value args(json::Value::Type::ARRAY);
    for(const auto& argument : m_message_args) {
        args.push_back(argument);
    }
    j[MESSAGE_ARGS] = args;
    j[ORIGIN_OF_CONDITION] = m_origin_of_condition;
    j[CONTEXT] = m_context;

    return j;
}
