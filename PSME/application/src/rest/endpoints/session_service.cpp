/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file session_service.cpp
 * */

#include "psme/rest/endpoints/session_service.hpp"
#include "psme/rest/security/session/session_service_manager.hpp"
#include "psme/rest/constants/constants.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/session_service.hpp"

#include "agent-framework/module/enum/common.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;
using namespace psme::rest::server;
using namespace psme::rest::security;
using namespace agent_framework::model;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#SessionService.SessionService";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#SessionService.v1_1_2.SessionService";

    r[Common::ID] = "SessionService";
    r[Common::NAME] = "Session Service";
    r[Common::DESCRIPTION] = "Session Service";

    r[Common::STATUS][Common::STATE] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH] = json::Json::value_t::null;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Json::value_t::null;

    r[SessionService::SERVICE_ENABLED] = json::Json::value_t::null;
    r[SessionService::SESSION_TIMEOUT] = json::Json::value_t::null;

    r[SessionService::SESSIONS][Common::ODATA_ID] = "/redfish/v1/SessionService/Sessions";

    return r;
}
}


endpoint::SessionService::SessionService(const std::string& path) : EndpointBase(path) {}


endpoint::SessionService::~SessionService() {}


void endpoint::SessionService::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    r[Common::STATUS][Common::STATE] = session::SessionServiceManager::get_instance()->is_service_enabled()
                                       ? enums::State(enums::State::Enabled).to_string() : enums::State(
            enums::State::Disabled).to_string();

    r[constants::SessionService::SERVICE_ENABLED] = session::SessionServiceManager::get_instance()->is_service_enabled();
    r[constants::SessionService::SESSION_TIMEOUT] = session::SessionServiceManager::get_instance()->get_session_timeout().count();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    set_response(response, r);
}


void endpoint::SessionService::patch(const server::Request& request, server::Response& response) {
    const auto& json = JsonValidator::validate_request_body<schema::SessionServicePatchSchema>(request);
    auto current_service_enabled = session::SessionServiceManager::get_instance()->is_service_enabled();
    auto current_session_timeout = session::SessionServiceManager::get_instance()->get_session_timeout().count();
    auto service_enabled = json.value(constants::SessionService::SERVICE_ENABLED, bool{current_service_enabled});
    auto session_timeout = json.value(constants::SessionService::SESSION_TIMEOUT,
                                      std::int64_t{current_session_timeout});

    session::SessionServiceManager::get_instance()->set_session_timeout(std::chrono::seconds(session_timeout));
    session::SessionServiceManager::get_instance()->set_service_enabled(service_enabled);

    response.set_status(server::status_2XX::OK);
    get(request, response);
}