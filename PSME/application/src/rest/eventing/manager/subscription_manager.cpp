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
 * */

#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/eventing/event_service.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "configuration/utils.hpp"

#include <fstream>



namespace psme {
namespace rest {
namespace eventing {
namespace manager {

SubscriptionManager::SubscriptionManager() {

    m_db = database::Database::create("Subscription");

    /* read all subscriptions from the DB */
    database::IdValue id{};
    database::String json{};
    m_db->start();
    while (m_db->next(id, json)) {
        try {
            json::Json root = json::Json();
            if (configuration::string_to_json(json.get(), root)) {
                /* don't check origin resources: endpoints might not be initialized yet */
                Subscription subscription = Subscription::from_json(root, false);
                subscription.set_id(id.get());
                m_subscriptions[id.get()] = subscription;
                /* make subscription valid */
                m_db->put(id, json);
                /* get new id for "new" subscription */
                if (m_id < id.get()) {
                    m_id = id.get();
                }
            }
            else {
                log_error("rest", "Cannot parse subscription " << id << ": " << json);
                m_db->remove(id);
            }
        }
        catch (const std::exception& e) {
            log_error("rest", "Cannot add subscription " << id << ": " << e.what() << " " << json);
            m_db->remove(id);
        }
    }
    m_db->end();
}

SubscriptionManager::~SubscriptionManager() {
    clean_up();
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

void SubscriptionManager::for_each(const SubscriptionCallback& handle) const {
    std::lock_guard<std::mutex> lock{m_mutex};
    for (const auto& entry : m_subscriptions) {
        handle(entry.second);
    }
}

uint64_t SubscriptionManager::add(Subscription subscription) {
    std::lock_guard<std::mutex> lock{m_mutex};

    /* find first not used subscription ID */
    while (m_subscriptions.count(++m_id) != 0);
    subscription.set_id(m_id);

    /* check for duplicates */
    auto found = std::find_if(std::begin(m_subscriptions), std::end(m_subscriptions),
        [&subscription](const SubscriptionMap::value_type& v) {
            return equal_subscriptions(v.second, subscription);
        });
    if (found != std::end(m_subscriptions)) {
        log_warning("rest", " subscription " << m_id << " has duplicate id=" << found->second.get_id()
                                             << ": " << found->second.to_json());
    }

    m_db->put(database::IdValue(m_id), database::String(configuration::json_value_to_string(subscription.to_json())));
    m_subscriptions[subscription.get_id()] = std::move(subscription);
    return m_id;
}

void SubscriptionManager::del(uint64_t subscription_id) {
    std::lock_guard<std::mutex> lock{m_mutex};
    auto it = m_subscriptions.find(subscription_id);
    if (it == m_subscriptions.end()) {
        throw agent_framework::exceptions::NotFound(std::string{"Subscription (ID: "}
            + std::to_string(subscription_id) + ") not found.");
    }
    m_subscriptions.erase(it);
    m_db->remove(database::IdValue(subscription_id));
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

void SubscriptionManager::clean_up() {
    if (m_db) {
        m_db->remove();
        m_db.reset();
    }
}

}
}
}
}
