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
 * */

#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/eventing/event_service.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include <json/json.hpp>
#include <fstream>



namespace psme {
namespace rest {
namespace eventing {
namespace manager {

std::uint64_t SubscriptionManager::id = 1;

void SubscriptionManager::set_subscriptions(const SubscriptionMap& subscriptions) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_subscriptions = subscriptions;
    // set next subscription id after the maximal of those in subscriptions
    uint64_t max_id =
        std::max_element(subscriptions.begin(), subscriptions.end(),
                         [](const SubscriptionMap::value_type& a, const SubscriptionMap::value_type& b) {
                             return a.first < b.first;
                         })->first;
    SubscriptionManager::id = max_id + 1;
}

uint64_t SubscriptionManager::add(Subscription subscription) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto found =  std::find_if(std::begin(m_subscriptions), std::end(m_subscriptions),
                               [&subscription](const SubscriptionMap::value_type& v) {
                                   return equal_subscriptions(v.second, subscription);
                               });
    if (found != std::end(m_subscriptions)) {
        log_warning("rest", " subscription already exists: " << found->second.to_json()
                            << "\n duplicated subscription id: " << id);
    }
    subscription.set_id(id++);
    m_subscriptions[subscription.get_id()] = subscription;
    return subscription.get_id();
}

Subscription SubscriptionManager::get(uint64_t subscription_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_subscriptions.find(subscription_id);
    if (it == m_subscriptions.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Subscription (ID: "}
            + std::to_string(subscription_id) + ") not found.");
    }
    return it->second;
}

SubscriptionMap SubscriptionManager::get() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_subscriptions;
}

void SubscriptionManager::del(uint64_t subscription_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_subscriptions.find(subscription_id);
    if (it == m_subscriptions.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Subscription (ID: "}
            + std::to_string(subscription_id) + ") not found.");
    }
    m_subscriptions.erase(it);
}

uint32_t SubscriptionManager::size() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return static_cast<uint32_t>(m_subscriptions.size());
}

void SubscriptionManager::notify(const Event& event) {
    EventArrayUPtr event_array(new EventArray({event}));
    EventService::post_events_array(std::move(event_array));
}

void SubscriptionManager::notify(const EventVec& events) {
    if (!events.empty()) {
        EventArrayUPtr event_array(new EventArray(events));
        event_array->remove_duplicates();
        EventService::post_events_array(std::move(event_array));
    }
}

SubscriptionManager::~SubscriptionManager() { }

}
}
}
}
