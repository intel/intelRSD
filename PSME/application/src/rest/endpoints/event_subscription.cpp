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

#include "psme/rest/endpoints/event_subscription.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/server/error/server_exception.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::server;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::endpoint::utils;



namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventDestination.EventDestination";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EventDestination.v1_1_1.EventDestination";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = json::Json::value_t::null;
    r[Common::DESCRIPTION] = json::Json::value_t::null;
    r[EventSubscription::DESTINATION] = json::Json::value_t::null;
    r[EventSubscription::CONTEXT] = json::Json::value_t::null;
    r[EventSubscription::PROTOCOL] = json::Json::value_t::null;
    r[EventSubscription::EVENT_TYPES] = json::Json::value_t::array;
    r[EventSubscription::ORIGIN_RESOURCES] = json::Json::value_t::array;

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
    response.set_status(server::status_2XX::NO_CONTENT);
}
