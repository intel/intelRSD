/*!
 * @brief Definition of EventArray class carrying events for a subscriber
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file rest/eventing/event_array.cpp
 */

#include "psme/rest/eventing/event_array.hpp"
#include "psme/rest/constants/constants.hpp"
#include "json/json.hpp"

#include <algorithm>

using psme::rest::eventing::EventArray;
using namespace psme::rest;

uint64_t EventArray::EVENT_ARRAY_ID = 1;

void EventArray::assign_new_id() {
    m_id = EVENT_ARRAY_ID++;
    if (EVENT_ARRAY_ID == 0) {
        log_warning(GET_LOGGER("rest"), "Counter of Event array Ids has overflown!");
    }
}

void EventArray::remove_duplicates() {
    using Type = eventing::EventType::base_enum;

    std::map<std::string, Type> present_events{};
    auto it = std::remove_if(m_events.begin(), m_events.end(), [&present_events] (const Event& event) {
        if (present_events.count(event.get_origin_of_condition()) == 0) {
            present_events.emplace(event.get_origin_of_condition(), event.get_type());
            return false;
        }
        else {
            if (present_events[event.get_origin_of_condition()] != event.get_type()) {
                log_warning("rest", "Conflicting events for " + event.get_origin_of_condition() + " : "
                                    + eventing::EventType(present_events[event.get_origin_of_condition()]).to_string()
                                    + " and " + event.get_type().to_string());
            }
            // remove event with duplicate URL
            return true;
        }
    });
    m_events.erase(it, m_events.end());
}

json::Value EventArray::to_json() const {
    json::Value json(json::Value::Type::OBJECT);
    json[constants::Common::ODATA_CONTEXT] = "/rest/v1/$metadata#Event.Event";
    json[constants::Common::ODATA_ID] = "/rest/v1/EventService/Events/" + std::to_string(m_id);
    json[constants::Common::ODATA_TYPE] = "#Event.v1_2_0.Event";
    json[constants::Common::ID] = std::to_string(m_id);
    json[constants::Common::NAME] = "Event Array";
    json[constants::Common::DESCRIPTION] = "Events";

    for (const auto& event: m_events) {
        json[constants::EventArray::EVENTS].push_back(event.to_json());
    }
    json[constants::Event::CONTEXT] = m_context;
    return json;
}
