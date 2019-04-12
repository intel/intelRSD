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

#include "psme/rest/server/request.hpp"
#include "psme/rest/server/error/error_factory.hpp"

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/endpoints/event_subscriptions_collection.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/subscription_collection.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::eventing::model;
using namespace psme::rest::validators;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventDestinationCollection.EventDestinationCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EventDestinationCollection.EventDestinationCollection";
    r[Common::NAME] = "Event Subscriptions Collection";
    r[Common::DESCRIPTION] = "Collection of Event Subscriptions";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


SubscriptionCollection::SubscriptionCollection(const std::string& path) : EndpointBase(path) {}


SubscriptionCollection::~SubscriptionCollection() {}


void SubscriptionCollection::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();
    uint32_t number = 0;
    r[Common::ODATA_ID] = PathBuilder(req).build();
    SubscriptionManager::get_instance()->for_each([&r, &req, &number](const Subscription& subscription) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(subscription.get_id()).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
        number++;
    });
    r[Collection::ODATA_COUNT] = number;
    set_response(res, r);
}


void SubscriptionCollection::post(const server::Request& request, server::Response& response) {
    const auto& json = JsonValidator::validate_request_body<schema::SubscriptionCollectionPostSchema>(request);
    Subscription subscription = Subscription::from_json(json);
    std::uint64_t id = SubscriptionManager::get_instance()->add(subscription);
    endpoint::utils::set_location_header(request, response, PathBuilder(request).append(id).build());
    response.set_status(server::status_2XX::CREATED);
}
