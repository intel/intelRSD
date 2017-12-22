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

#include "psme/rest/endpoints/event_subscription.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/eventing/config/subscription_config.hpp"
#include "psme/rest/constants/constants.hpp"
#include "json/json.hpp"

#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/server/error/server_exception.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::server;
using namespace psme::rest::eventing::config;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::endpoint::utils;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventDestination.EventDestination";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EventDestination.v1_1_1.EventDestination";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = json::Value::Type::NIL;
    r[Common::DESCRIPTION] = json::Value::Type::NIL;
    r[EventSubscription::DESTINATION] = json::Value::Type::NIL;
    r[EventSubscription::CONTEXT] = json::Value::Type::NIL;
    r[EventSubscription::PROTOCOL] = json::Value::Type::NIL;
    r[EventSubscription::EVENT_TYPES] = json::Value::Type::ARRAY;
    r[EventSubscription::ORIGIN_RESOURCES] = json::Value::Type::ARRAY;

    return r;
}
}

endpoint::Subscription::Subscription(const std::string& path) : EndpointBase(path) {}

endpoint::Subscription::~Subscription() {}

void endpoint::Subscription::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = request.get_url();
    r[Common::ID] = request.params[PathParam::SUBSCRIPTION_ID];

    const auto& id = id_to_uint64(request.params[PathParam::SUBSCRIPTION_ID]);
    const auto& subscription = SubscriptionManager::get_instance()->get(id);

    subscription.fill_json(r);
    set_response(response, r);
}

void endpoint::Subscription::del(const server::Request& request, server::Response& response) {
    const auto& id = id_to_uint64(request.params[PathParam::SUBSCRIPTION_ID]);
    SubscriptionManager::get_instance()->del(id);
    SubscriptionConfig::get_instance()->save();
    response.set_status(server::status_2XX::NO_CONTENT);
}
