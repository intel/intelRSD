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
 * @file session_collection.cpp
 * */

#include "psme/rest/endpoints/session.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/server/http_headers.hpp"

#include "psme/rest/constants/constants.hpp"
#include "psme/rest/security/session/session_manager.hpp"
#include "psme/rest/security/session/session_service_manager.hpp"
#include "psme/rest/security/account/account_manager.hpp"
#include "psme/rest/endpoints/session_collection.hpp"

#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/session_collection.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::endpoint;
using namespace psme::rest::security;
using namespace psme::rest::validators;
using namespace psme::rest::server;

namespace {
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#SessionCollection.SessionCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#SessionCollection.SessionCollection";
    r[Common::NAME] = "Session Collection";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}

}


SessionCollection::SessionCollection(const std::string& path) : EndpointBase(path) {}


SessionCollection::~SessionCollection() {}


void SessionCollection::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();
    uint32_t number = 0;
    r[Common::ODATA_ID] = PathBuilder(req).build();
    session::SessionManager::get_instance()->for_each([&r, &req, &number](const session::Session& session) {
        json::Json link_elem(json::Json::value_t::object);
        link_elem[Common::ODATA_ID] = PathBuilder(req).append(session.get_id()).build();
        r[Collection::MEMBERS].push_back(std::move(link_elem));
        number++;
    });
    r[Collection::ODATA_COUNT] = number;
    set_response(res, r);
}


void SessionCollection::post(const server::Request& request, server::Response& response) {

    if (!session::SessionServiceManager::get_instance()->is_service_enabled()) {
        // TODO replace invalid_payload_error with new appropriate error type
        auto error = error::ErrorFactory::create_invalid_payload_error("SessionService disabled");
        throw error::ServerException(error);
    }
    const auto& json = JsonValidator::validate_request_body<schema::SessionCollectionPostSchema>(request);
    const auto& password = json.value(constants::Session::PASSWORD, OptionalField<std::string>());
    const auto& user_name = json.value(constants::Session::USER_NAME, OptionalField<std::string>());


    auto error = error::ErrorFactory::create_unauthorized_error(request.get_url(),
                                                                "Cannot create new session: invalid credentials.");
    if (!password.has_value() || !user_name.has_value()) {

        throw error::ServerException(error);
     }

    session::Session session = session::Session::from_json(json);
    if (account::AccountManager::get_instance()->validate_credentials(session.get_user_name(), password)) {

        session.set_origin_header(request.get_header(http_headers::Origin::ORIGIN));
        std::uint64_t id = session::SessionManager::get_instance()->add(session);

        const auto& new_session = session::SessionManager::get_instance()->get(id);
        endpoint::utils::set_location_header(request, response, PathBuilder(request).append(id).build());
        response.set_header(http_headers::XAuthToken::X_AUTH_TOKEN, new_session.get_session_auth_token());
        response.set_status(server::status_2XX::CREATED);

        auto session_entity_json = endpoint::Session::make_prototype();

        session_entity_json[Common::ODATA_ID] = PathBuilder(request).append(std::to_string(id)).build();

        new_session.fill_json(session_entity_json);
        set_response(response, session_entity_json);
    }
    else {
        throw error::ServerException(error);
    }
}
