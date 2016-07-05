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

#include "psme/rest-ref/endpoints/event_subscription.hpp"
#include "psme/rest-ref/endpoints/utils.hpp"
#include "psme/rest-ref/eventing/manager/subscription_manager.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "json/json.hpp"

#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/server/error/server_exception.hpp"

using namespace psme::rest::server;
using namespace psme::rest;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::endpoint::utils;
using namespace app::rest;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventDestination.EventDestination";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EventDestination.1.0.0.EventDestination";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = json::Value::Type::NIL;
    r[Common::DESCRIPTION] = json::Value::Type::NIL;
    r[EventSubscription::DESTINATION] = json::Value::Type::NIL;
    r[EventSubscription::CONTEXT] = json::Value::Type::NIL;
    r[EventSubscription::PROTOCOL] = json::Value::Type::NIL;
    r[EventSubscription::EVENT_TYPES] = json::Value::Type::ARRAY;
    return r;
}

void to_json(const Subscription& subscription, json::Value& json) {
    json[Common::NAME] = subscription.get_name();
    json[EventSubscription::DESTINATION] = subscription.get_destination();
    json[EventSubscription::CONTEXT] = subscription.get_context();
    json[EventSubscription::PROTOCOL] = subscription.get_protocol();
    json::Value event_types_json(json::Value::Type::ARRAY);
    for (const auto& event_type : subscription.get_event_types().get()){
        event_types_json.push_back(event_type.to_string());
    }
    json[EventSubscription::EVENT_TYPES] = event_types_json;
}
}

endpoint::Subscription::Subscription(const std::string& path) : Endpoint(path) {}
endpoint::Subscription::~Subscription() {}

void endpoint::Subscription::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = request.get_url();
    r[Common::ID] = request.params[PathParam::SUBSCRIPTION_ID];
    const auto& id = id_to_uint64(request.params[PathParam::SUBSCRIPTION_ID]);
    try {
        const auto& subscription = SubscriptionManager::get_instance()->get(id);
        to_json(subscription, r);
        set_response(response, r);
    }
    catch (const std::exception& ex) {
        throw(agent_framework::exceptions::NotFound(std::string("Unable to get subscription. ") + ex.what()));

    }
}

void endpoint::Subscription::del(const server::Request& request, server::Response& response) {
    const auto& id = id_to_uint64(request.params[PathParam::SUBSCRIPTION_ID]);
    try {
        SubscriptionManager::get_instance()->del(id);
        response.set_status(server::status_2XX::NO_CONTENT);
    }
    catch (const std::exception& ex) {
        response.set_status(server::status_4XX::NOT_FOUND);
        log_warning(GET_LOGGER("rest"), "Unable to delete subscription. " << ex.what());
    }
}
