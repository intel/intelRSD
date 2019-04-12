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
 * */


#include "psme/rest/eventing/model/subscription.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "agent-framework/exceptions/invalid_value.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include <algorithm>



using namespace psme::rest::constants;

namespace psme {
namespace rest {
namespace eventing {
namespace model {

bool Subscription::is_subscribed_for(const Event& event) const {
    // 1. Check if the subscription is for the event's EventType. If it's not, return false.
    // 2. If no origin resources are specified, the subscription is for events from all resources. Return true.
    // 3. Otherwise, check if the event's origin resource matches one of the subscription's origin resources.
    auto event_types = m_event_types.get();
    if (std::find(event_types.begin(), event_types.end(), event.get_type()) == event_types.end()) {
        return false;
    }
    if (m_origin_resources.empty()) {
        return true;
    }
    return std::find(m_origin_resources.begin(), m_origin_resources.end(), event.get_origin_of_condition())
           != m_origin_resources.end();
}


json::Json Subscription::to_json() const {
    json::Json j = json::Json();
    fill_json(j);
    return j;
}


void Subscription::fill_json(json::Json& json) const {
    json[Common::NAME] = m_name;
    json[EventSubscription::DESTINATION] = m_destination;
    json[EventSubscription::CONTEXT] = m_context;
    json[EventSubscription::PROTOCOL] = m_protocol.to_string();
    json::Json event_types_json(json::Json::array());
    for (const auto& event_type : m_event_types.get()) {
        event_types_json.push_back(event_type.to_string());
    }
    json[EventSubscription::EVENT_TYPES] = event_types_json;
    json[EventSubscription::ORIGIN_RESOURCES] = json::Json::array();
    for (const auto& origin_resource : m_origin_resources) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = origin_resource;
        json[EventSubscription::ORIGIN_RESOURCES].push_back(link);
    }
}


Subscription Subscription::from_json(const json::Json& json, bool validate_origin_resources) {
    const auto& name = json[Common::NAME].get<std::string>();
    const auto& destination = json[EventSubscription::DESTINATION].get<std::string>();
    const auto& context = json[EventSubscription::CONTEXT].get<std::string>();
    const auto& protocol = json[EventSubscription::PROTOCOL].get<std::string>();
    EventTypes event_types{};
    for (const auto& event_type : json[EventSubscription::EVENT_TYPES]) {
        event_types.add(EventType::from_string(event_type.get<std::string>()));
    }
    Subscription subscription{};
    subscription.set_name(name);
    subscription.set_destination(destination);
    subscription.set_context(context);
    subscription.set_protocol(SubscriptionProtocol::from_string(protocol));
    subscription.set_event_types(event_types);

    if (json.count(EventSubscription::ORIGIN_RESOURCES) &&
        !json.value(EventSubscription::ORIGIN_RESOURCES, json::Json()).is_null()) {
        if (validate_origin_resources) {
            auto& mp = *(psme::rest::server::Multiplexer::get_instance());
            for (const auto& origin_resource : json[EventSubscription::ORIGIN_RESOURCES]) {
                const auto& resource_url = origin_resource[Common::ODATA_ID].get<std::string>();
                if (mp.is_correct_endpoint_url(resource_url)) {
                    subscription.add_origin_resource(resource_url);
                }
                else {
                    throw agent_framework::exceptions::InvalidValue(
                        "Origin resource '" + resource_url + "' is not a valid endpoint in /redfish/v1 namespace.");
                }
            }
        }
        else {
            for (const auto& origin_resource : json[EventSubscription::ORIGIN_RESOURCES]) {
                const auto& resource_url = origin_resource[Common::ODATA_ID].get<std::string>();
                subscription.add_origin_resource(resource_url);
            }
        }
    }
    return subscription;
}


void EventTypes::add(EventType event_type) {
    m_event_types.push_back(event_type);
}


EventTypes::EventTypeVec EventTypes::get() const {
    return m_event_types;
}


bool equal_subscriptions(const Subscription& s1, const Subscription& s2) {
    return s1.get_destination() == s2.get_destination()
           && s1.get_context() == s2.get_context()
           && s1.get_event_types().get() == s2.get_event_types().get()
           && s1.get_origin_resources() == s2.get_origin_resources()
           && s1.get_name() == s2.get_name()
           && s1.get_protocol() == s2.get_protocol();
}

}
}
}
}
