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
 * */

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/test_event_subscription.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::eventing::model;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventService/Members/Events/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#TestEventDestinationCollection.TestEventDestinationCollection";
    r[Common::NAME] = "Test Event Subscription";
    r[Common::DESCRIPTION] = "Test Event Subscription description";
    r[Collection::ODATA_COUNT] = 0;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

TestEventSubscription::TestEventSubscription(const std::string& path) : EndpointBase(path) {}

TestEventSubscription::~TestEventSubscription() {}

void TestEventSubscription::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    Event event(EventType::ResourceAdded, "rackscale-test-subscription-passed");
    SubscriptionManager::get_instance()->notify(event);
    set_response(res, r);
}
