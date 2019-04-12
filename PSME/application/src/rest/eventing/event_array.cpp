/*!
 * @brief Definition of EventArray class carrying events for a subscriber
 *
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
 * @file rest/eventing/event_array.cpp
 */

#include "psme/rest/eventing/event_array.hpp"
#include "psme/rest/constants/constants.hpp"

#include <algorithm>
#include <map>



using namespace psme::rest::eventing;

uint64_t EventArray::EVENT_ARRAY_ID = 1;

void EventArray::assign_new_id() {
    m_id = EVENT_ARRAY_ID++;
    if (EVENT_ARRAY_ID == 0) {
        log_warning("rest", "Counter of Event array Ids has overflown!");
    }
}

void EventArray::remove_duplicates() {
    std::map<std::string, std::vector<EventType>> present_events{};
    auto it = std::remove_if(m_events.begin(), m_events.end(), [&present_events] (const Event& event) {
        if (present_events.count(event.get_origin_of_condition()) == 0) {
            present_events.emplace(event.get_origin_of_condition(), std::vector<EventType>{event.get_type()});
            return false;
        }
        else {
            auto types = present_events[event.get_origin_of_condition()];
            if (std::find(types.begin(), types.end(), event.get_type()) == types.end()) {
                types.push_back(event.get_type());
                present_events[event.get_origin_of_condition()] = types;
                return false;
            }

            // Remove event with duplicate URL and type
            return true;
        }
    });
    m_events.erase(it, m_events.end());
}

json::Json EventArray::to_json() const {
    json::Json json(json::Json::object());
    json[constants::Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Event.Event";
    json[constants::Common::ODATA_ID] = "/redfish/v1/EventService/Events/" + std::to_string(m_id);
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
