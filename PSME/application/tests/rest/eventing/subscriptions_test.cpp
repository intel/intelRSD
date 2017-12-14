/*!
 * @brief Subscriptions tests
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
 * @file subscriptions_test.hpp
 */

#include "psme/rest/eventing/model/subscription.hpp"
#include "json/value.hpp"
#include "gtest/gtest.h"
#include "json/deserializer.hpp"
#include "configuration/utils.hpp"

using namespace testing;
using namespace psme::rest::eventing::model;
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

}

TEST(SubscriptionsTest, EqualSubscriptions) {
    Subscription sub1{};
    Subscription sub2{};
    ASSERT_TRUE(equal_subscriptions(sub1, sub2));
    ASSERT_EQ(sub1.get_id(), sub2.get_id());
    sub1.set_id(5u);
    ASSERT_NE(sub1.get_id(), sub2.get_id());
    ASSERT_TRUE(equal_subscriptions(sub1, sub2));
    json::Value subscription_json;
    if (!configuration::string_to_json(SUBSCRIPTION, subscription_json)) {
        throw std::runtime_error("Malformed JSON: ");
    }
    sub1 = Subscription::from_json(subscription_json, false);
    ASSERT_FALSE(equal_subscriptions(sub1, sub2));
}
