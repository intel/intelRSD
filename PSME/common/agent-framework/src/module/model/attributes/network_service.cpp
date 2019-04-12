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
 *
 * */

#include "agent-framework/module/model/attributes/network_service.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

NetworkService::NetworkService() { }

NetworkService::~NetworkService() { }

json::Json NetworkService::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkService::NAME] = get_name().to_string();
    result[literals::NetworkService::ENABLED] = get_enabled();
    result[literals::NetworkService::PORT] = get_port();
    return result;
}

NetworkService NetworkService::from_json(const json::Json& response) {
    attribute::NetworkService service;
    service.set_name(enums::NetworkServiceName::from_string(
                response[literals::NetworkService::NAME]));
    service.set_port(response[literals::NetworkService::PORT]);
    service.set_enabled(response[literals::NetworkService::ENABLED]);
    return service;
}
