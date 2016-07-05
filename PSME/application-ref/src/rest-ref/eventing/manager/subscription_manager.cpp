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
 *
 * */

#include "psme/rest-ref/eventing/manager/subscription_manager.hpp"
#include "psme/rest-ref/eventing/event_service.hpp"
#include <json/json.hpp>
#include <fstream>

namespace psme {
namespace rest {
namespace eventing {
namespace manager {

uint64_t SubscriptionManager::id = 1;

void SubscriptionManager::add(Subscription subscription) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto sub = m_subscriptions.find(subscription.get_name());
    if (m_subscriptions.end() != sub) {
        throw std::invalid_argument("Subscription '" + subscription.get_name()
                + "' already exists.");
    }
    subscription.set_id(id++);
    m_subscriptions[subscription.get_name()] = subscription;
}

Subscription SubscriptionManager::get(const std::string& subscription_name) {
    std::lock_guard<std::mutex> lock{m_mutex};
    return get_by_name(subscription_name);
}

Subscription SubscriptionManager::get_by_name(const std::string& subscription_name) {
    auto subscription = m_subscriptions.find(subscription_name);
    if (m_subscriptions.end() == subscription) {
        throw std::invalid_argument("Subscription '" + subscription_name
                + "' not found.");
    }
    return subscription->second;
}

Subscription SubscriptionManager::get(uint64_t subscription_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& item : m_subscriptions) {
        const auto& subscription = item.second;
        if (subscription_id == subscription.get_id()) {
            return get_by_name(subscription.get_name());
        }
    }
    throw std::invalid_argument("Subscription id: " + std::to_string(subscription_id)
            + " not found.");
}

void SubscriptionManager::del_by_name(const std::string& subscription_name) {
    auto subscription = m_subscriptions.find(subscription_name);
    if (m_subscriptions.end() == subscription) {
        throw std::invalid_argument("Subscription '"+ subscription_name
                + "' not found.");
    }
    m_subscriptions.erase(subscription);
}

SubscriptionMap SubscriptionManager::get() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_subscriptions;
}

void SubscriptionManager::del(const std::string& subscription_name) {
    std::lock_guard<std::mutex> lock{m_mutex};
    del_by_name(subscription_name);
}

void SubscriptionManager::del(uint64_t subscription_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& item : m_subscriptions) {
        const auto& subscription = item.second;
        if (subscription_id == subscription.get_id()) {
            del_by_name(subscription.get_name());
            return;
        }
    }
    throw std::invalid_argument("Subscription id: " + std::to_string(subscription_id)
            + " not found.");
}

uint32_t SubscriptionManager::size() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return static_cast<uint32_t>(m_subscriptions.size());
}

void SubscriptionManager::notify(const Event& event) {
    std::lock_guard<std::mutex> lock{m_mutex};
    do_notify(event);
}

void SubscriptionManager::notify(const EventVec& events) {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& event : events) {
        do_notify(event);
    }
}

void SubscriptionManager::do_notify(const Event& event) {
    EventUPtr add_event(new Event(event));
    EventService::post_event(std::move(add_event));
    log_info(GET_LOGGER("rest"), "Upstream event enqueued: type=" << event.get_type().to_string()
                                 << ", link=" << event.get_origin_of_condition());
}

SubscriptionManager::~SubscriptionManager() {
}

}
}
}
}
