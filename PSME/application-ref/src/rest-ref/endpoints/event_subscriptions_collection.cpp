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

#include "psme/rest-ref/constants/constants.hpp"
#include "psme/rest-ref/endpoints/event_subscriptions_collection.hpp"
#include "psme/rest-ref/eventing/config/subscription_config.hpp"
#include "psme/rest-ref/server/request.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"

using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::eventing::config;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::eventing::model;
using namespace app::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventService/Members/Events/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EventDestinationCollection.EventDestinationCollection";
    r[Common::NAME] = "Event Subscriptions Collection";
    r[Common::DESCRIPTION] = "Collection of Event Subscriptions";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}
}

SubscriptionCollection::SubscriptionCollection(const std::string& path) : Endpoint(path) {}
SubscriptionCollection::~SubscriptionCollection() {}

namespace {
    json::Value validate_post_request(const server::Request& request) {
        using namespace psme::rest::error;
        using namespace psme::rest::utils;
        using json::Pair;

        json::Value schema({
            Pair(Common::NAME,
                    JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
            Pair(EventSubscription::DESTINATION,
                    JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
            Pair(EventSubscription::EVENT_TYPES,
                    JsonValidator::type_of(JsonValidator::ARRAY_TYPE, true)),
            Pair(EventSubscription::CONTEXT,
                    JsonValidator::type_of(JsonValidator::STRING_TYPE, true)),
            Pair(EventSubscription::PROTOCOL,
                    JsonValidator::type_of(JsonValidator::STRING_TYPE, true))
        });

        return JsonValidator::validate_request_body(request, schema);
    }

    Subscription to_model(const json::Value& json) {
        Subscription s;
        const auto& name = json[Common::NAME].as_string();
        const auto& destination = json[EventSubscription::DESTINATION].as_string();
        const auto& context = json[EventSubscription::CONTEXT].as_string();
        const auto& protocol = json[EventSubscription::PROTOCOL].as_string();
        EventTypes event_types;
        for (const auto& event_type : json[EventSubscription::EVENT_TYPES]){
            event_types.add(EventType::from_string(event_type.as_string()));
        }
        s.set_name(name);
        s.set_destination(destination);
        s.set_context(context);
        s.set_protocol(protocol);
        s.set_event_types(event_types);
        return s;
    }
}

void SubscriptionCollection::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Collection::ODATA_COUNT] = SubscriptionManager::get_instance()->size();

    for (const auto& item : SubscriptionManager::get_instance()->get()) {
        json::Value link_elem(json::Value::Type::OBJECT);
        const auto& subscription = item.second;
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(subscription.get_id()).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
    }
    set_response(res, r);
}

void SubscriptionCollection::post(const server::Request& request, server::Response& response) {
    const auto& json = validate_post_request(request);
    Subscription subscription = to_model(json);
    try {
        SubscriptionManager::get_instance()->add(subscription);
        SubscriptionConfig::get_instance()->save();
        response.set_status(server::status_2XX::CREATED);
    }
    catch (const std::exception& ex) {
        response.set_status(server::status_4XX::CONFLICT);
        log_warning(GET_LOGGER("rest"), "Unable to add subscription. " << ex.what());
    }
}
