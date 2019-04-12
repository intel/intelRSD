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
 * @file session.cpp
 * */

#include "psme/rest/endpoints/session.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/security/session/session_manager.hpp"
#include "psme/rest/security/session/session_service_manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/server/error/server_exception.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::server;
using namespace psme::rest::security::session;
using namespace psme::rest::endpoint::utils;


json::Json endpoint::Session::make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Session.Session";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#Session.v1_1_0.Session";
    r[Common::ID] = json::Json::value_t::null;
    r[Common::NAME] = json::Json::value_t::null;
    r[Common::DESCRIPTION] = json::Json::value_t::null;
    r[constants::Session::USER_NAME] = json::Json::value_t::null;
    r[constants::Session::PASSWORD] = json::Json::value_t::null;
    r[constants::Session::OEM] = json::Json::value_t::null;

    return r;
}


endpoint::Session::Session(const std::string& path) : EndpointBase(path) {}


endpoint::Session::~Session() {}


void endpoint::Session::get(const server::Request& request, server::Response& response) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(request).build();

    const auto& id = id_to_uint64(request.params[PathParam::SESSION_ID]);
    const auto& session = SessionManager::get_instance()->get(id);

    session.fill_json(r);
    set_response(response, r);
}


void endpoint::Session::del(const server::Request& request, server::Response& response) {
    if (!SessionServiceManager::get_instance()->is_service_enabled()) {
        // TODO replace invalid_payload_error with new appropriate error type
        auto error = error::ErrorFactory::create_invalid_payload_error("SessionService disabled");
        throw error::ServerException(error);
    }
    const auto& id = id_to_uint64(request.params[PathParam::SESSION_ID]);
    SessionManager::get_instance()->del(id);
    response.set_status(server::status_2XX::NO_CONTENT);
}
