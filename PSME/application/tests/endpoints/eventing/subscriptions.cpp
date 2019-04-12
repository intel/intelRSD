/*!
 * @brief Subscriptions tests
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
 * @file subscriptions.cpp
 */

#include "psme/rest/eventing/model/subscription.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "gtest/gtest.h"

#include "configuration/utils.hpp"

using namespace testing;
using namespace psme::rest::eventing::model;
using namespace psme::rest::eventing::manager;
using namespace configuration;

namespace {

constexpr char SUBSCRIPTION[] =
R"({
    "Name": "Event Subscription",
    "Destination": "https://10.3.0.1:8443/rest/EventListener/psme/b90ce8d6-8f25-11e7-8c5f-7f3a13f751a8",
    "Context": "Subscribed by PodM with UUID: 368e4547-8282-4c48-af57-9d73cddc79d1",
    "Protocol": "Redfish",
    "EventTypes": [
      "StatusChange",
      "ResourceUpdated",
      "ResourceAdded",
      "ResourceRemoved",
      "Alert"
    ],
    "OriginResources": []
})";

constexpr char SUBSCRIPTION_RES[] = R"({
    "Name": "Event Subscription",
    "Destination": "https://10.3.0.1:8443/rest/EventListener/psme/b90ce8d6-8f25-11e7-8c5f-7f3a13f751a8",
    "Context": "Subscribed by PodM with UUID: 368e4547-8282-4c48-af57-9d73cddc79d1",
    "Protocol": "Redfish",
    "EventTypes": [
      "StatusChange",
      "ResourceUpdated",
      "ResourceAdded",
      "ResourceRemoved",
      "Alert"
    ],
    "OriginResources": [
        {"@odata.id": "/redfish/v1/Systems/1"},
        {"@odata.id": "/redfish/v1/Systems/2"}
    ]
})";

std::uint32_t get_number_of_subscriptions(const SubscriptionManager& manager) {
    std::uint32_t number = 0;
    manager.for_each([&number](const Subscription&) mutable {
        number++;
    });
    return number;
}

void add_subscription(SubscriptionManager& manager, const char* subscription_txt, std::uint64_t& id) {
    std::uint32_t number = get_number_of_subscriptions(manager);

    json::Json subscription_json = json::Json();
    Subscription subscription;
    ASSERT_TRUE(configuration::string_to_json(subscription_txt, subscription_json))
                        << "Cannot parse subscription JSON";
    /* originResources are not checked */
    ASSERT_NO_THROW(subscription = Subscription::from_json(subscription_json, false))
                        << "Cannot build subscription";
    ASSERT_NO_THROW(id = manager.add(subscription))
                        << "Cannot add subscription to the manager";
    ASSERT_NO_THROW(manager.get(id))
                        << "Added subscription doesn't exist";

    ASSERT_EQ(number+1, get_number_of_subscriptions(manager));
}


}

TEST(SubscriptionsTest, ForEach) {
    SubscriptionManager manager{};

    /* add new entry */
    std::uint64_t id;
    add_subscription(manager, SUBSCRIPTION, id);

    ASSERT_THROW(manager.for_each([id](const Subscription& subscription) {
            if (subscription.get_id() == id) {
                throw std::runtime_error("Subscription found");
            }
        }), std::runtime_error) << "Just added subscription not found in the manager";

    /* at least one entry is to be found */
    std::uint32_t number = 0;
    ASSERT_NO_THROW(manager.for_each([&number, id](const Subscription& subscription) mutable {
        if (subscription.get_id() != id) {
            number++;
        }
    })) << "Cannot count entries in the manager";
    ASSERT_EQ(get_number_of_subscriptions(manager)-1, number)
                << "Wrong number of subscriptions found in the manager";

    ASSERT_NO_THROW(manager.del(id)) << "Cannot delete subscription";

    /* removed instance should not be found */
    ASSERT_NO_THROW(manager.for_each([id](const Subscription& subscription) {
        if (subscription.get_id() == id) {
            throw std::runtime_error("Subscription found");
        }
    })) << "Deleted subscription found in the DB";
}

TEST(SubscriptionsTest, EqualSubscriptions) {
    Subscription sub1{};
    Subscription sub2{};
    ASSERT_TRUE(equal_subscriptions(sub1, sub2));
    ASSERT_EQ(sub1.get_id(), sub2.get_id());
    sub1.set_id(5u);
    ASSERT_NE(sub1.get_id(), sub2.get_id());
    ASSERT_TRUE(equal_subscriptions(sub1, sub2));
    json::Json subscription_json = json::Json();
    ASSERT_TRUE(configuration::string_to_json(SUBSCRIPTION, subscription_json))
                << "Cannot parse subscription JSON";
    sub1 = Subscription::from_json(subscription_json, false);
    ASSERT_FALSE(equal_subscriptions(sub1, sub2));
}

TEST(SubscriptionsTest, ManagerDb) {
    std::uint32_t initial_number_in_manager;
    std::uint64_t id1;
    std::uint64_t id2;

    {
        SubscriptionManager manager{};
        initial_number_in_manager = get_number_of_subscriptions(manager);

        /* add subscription */
        add_subscription(manager, SUBSCRIPTION, id1);

        /* add subscription duplicate, warning log is expected */
        add_subscription(manager, SUBSCRIPTION, id2);

        ASSERT_EQ(initial_number_in_manager+2, get_number_of_subscriptions(manager));
        ASSERT_NO_THROW(manager.del(id2)) << "Cannot delete subscription";
        ASSERT_NE(id1, id2) << "Created subscription duplicate has same ID";
    }

    /* count plain DB entries */
    {
        auto db = database::Database::create("Subscription");
        std::uint32_t number_in_db = 0;
        bool first_found_in_db = false;
        database::IdValue key{};
        database::String value{};
        db->start();
        while (db->next(key, value)) {
            number_in_db++;
            if (key.get() == id1) {
                first_found_in_db = true;
            }
            if (key.get() == id2) {
                FAIL() << "Removed key found in the DB";
            }
        }
        db->end();
        db->remove();
        ASSERT_EQ(initial_number_in_manager+1, number_in_db);
        ASSERT_TRUE(first_found_in_db) << "Added key not in DB";
    }

    {
        SubscriptionManager manager{};
        ASSERT_EQ(initial_number_in_manager + 1, get_number_of_subscriptions(manager));
        ASSERT_NO_THROW(manager.get(id1)) << "Added subscription doesn't exist";
        ASSERT_NO_THROW(manager.del(id1)) << "Cannot delete subscription";
        ASSERT_EQ(initial_number_in_manager, get_number_of_subscriptions(manager));
    }

    {
        SubscriptionManager manager{};
        ASSERT_EQ(initial_number_in_manager, get_number_of_subscriptions(manager));
        ASSERT_THROW(manager.get(id1), agent_framework::exceptions::NotFound);
    }
}

TEST(SubscriptionsTest, WithResource) {
    SubscriptionManager manager{};
    std::uint64_t id;

    json::Json subscription_json = json::Json();
    ASSERT_TRUE(configuration::string_to_json(SUBSCRIPTION_RES, subscription_json))
                        << "Cannot parse subscription JSON";

    add_subscription(manager, SUBSCRIPTION_RES, id);

    /* check if subscription and its json are same, ASSERT_EQ cannot be used here */
    ASSERT_TRUE(subscription_json == manager.get(id).to_json()) << "Subscription from DB doesn't match JSON";
    ASSERT_NO_THROW(manager.del(id)) << "Cannot remove subscription";
}

TEST(SubscriptionsTest, ConsecutiveAdds) {
    std::uint32_t initial_number;
    std::uint64_t id1;
    std::uint64_t id2;
    std::uint64_t id3;

    {
        SubscriptionManager manager{};

        initial_number = get_number_of_subscriptions(manager);
        add_subscription(manager, SUBSCRIPTION_RES, id1);

        add_subscription(manager, SUBSCRIPTION_RES, id2);
        ASSERT_NO_THROW(manager.del(id2)) << "Cannot delete subscription";
        ASSERT_EQ(initial_number+1, get_number_of_subscriptions(manager));
        ASSERT_NE(id1, id2);

        add_subscription(manager, SUBSCRIPTION_RES, id3);
        ASSERT_NO_THROW(manager.del(id3)) << "Cannot delete subscription";
        ASSERT_EQ(initial_number+1, get_number_of_subscriptions(manager));
        ASSERT_NE(id1, id3);
        ASSERT_LT(id2, id3);
    }

    /* cleanup */
    {
        SubscriptionManager manager{};
        ASSERT_NO_THROW(manager.del(id1)) << "Cannot delete subscription";
        ASSERT_EQ(initial_number, get_number_of_subscriptions(manager));
    }

}
