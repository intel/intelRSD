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

#include "psme/rest/endpoints/event_service.hpp"
#include "psme/rest/constants/constants.hpp"
#include "json/json.hpp"

using namespace psme::rest;
using namespace psme::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EventService";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EventService.v1_0_0.EventService";
    r[Common::ID] = "EventService";
    r[Common::NAME] = "Event Service";
    r[Common::DESCRIPTION] = "Event Service description";
    r[Common::STATUS][Common::STATE] = "Enabled";
    r[Common::STATUS][Common::HEALTH] = "OK";
    r[Common::STATUS][Common::HEALTH_ROLLUP] = "OK";
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[EventService::SERVICE_ENABLED] = true;
    r[EventService::DELIVERY_RETRY_ATTEMPTS] = 3;
    r[EventService::DELIVERY_RETRY_INTERVAL_SECONDS] = 60;
    r[EventService::EVENT_TYPES_FOR_SUBSCRIPTION] = json::Value::Type::ARRAY;
    r[EventService::EVENT_TYPES_FOR_SUBSCRIPTION].push_back(EventService::STATUS_CHANGE);
    r[EventService::EVENT_TYPES_FOR_SUBSCRIPTION].push_back(EventService::RESOURCE_UPDATED);
    r[EventService::EVENT_TYPES_FOR_SUBSCRIPTION].push_back(EventService::RESOURCE_ADDED);
    r[EventService::EVENT_TYPES_FOR_SUBSCRIPTION].push_back(EventService::RESOURCE_REMOVED);
    r[EventService::EVENT_TYPES_FOR_SUBSCRIPTION].push_back(EventService::ALERT);
    r[EventService::SUBSCRIPTIONS][Common::ODATA_ID] = "/redfish/v1/EventService/Subscriptions";

    json::Value test;
    r[EventService::ACTIONS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}
}

endpoint::EventService::EventService(const std::string& path) : EndpointBase(path) {}

endpoint::EventService::~EventService() {}

void endpoint::EventService::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();

    set_response(res, r);
}
