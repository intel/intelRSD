/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */


#include "psme/rest-ref/eventing/model/subscription.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include <json/json.hpp>
#include <stdexcept>
namespace psme {
namespace rest {
namespace eventing {
namespace model {

using namespace app::rest::constants;

json::Value Subscription::to_json() const {
    json::Value json(json::Value::Type::OBJECT);
    json[Common::NAME] = m_name;
    json[EventSubscription::DESTINATION] = m_destination;
    json[EventSubscription::CONTEXT] = m_context;
    json[EventSubscription::PROTOCOL] = m_protocol;
    json::Value event_types_json(json::Value::Type::ARRAY);
    for (const auto& event_type : m_event_types.get()){
        event_types_json.push_back(event_type.to_string());
    }
    json[EventSubscription::EVENT_TYPES] = event_types_json;
    return json;
}

Subscription Subscription::from_json(const json::Value& json) {
    const auto& name = json[Common::NAME].as_string();
    const auto& destination = json[EventSubscription::DESTINATION].as_string();
    const auto& context = json[EventSubscription::CONTEXT].as_string();
    const auto& protocol = json[EventSubscription::PROTOCOL].as_string();
    EventTypes event_types;
    for (const auto& event_type : json[EventSubscription::EVENT_TYPES]){
        event_types.add(EventType::from_string(event_type.as_string()));
    }
    Subscription subscription;
    subscription.set_name(name);
    subscription.set_destination(destination);
    subscription.set_context(context);
    subscription.set_protocol(protocol);
    subscription.set_event_types(event_types);
    return subscription;
}

void EventTypes::add(EventType event_type) {
   m_event_types.push_back(event_type);
}

EventTypeVec EventTypes::get() const {
    return m_event_types;
}

}
}
}
}
