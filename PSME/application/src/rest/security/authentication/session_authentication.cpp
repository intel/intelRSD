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
 * @file session_authentication.cpp
 * */

#include "psme/rest/security/authentication/session_authentication.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/security/session/session_manager.hpp"
#include "psme/rest/server/http_headers.hpp"
#include "psme/rest/server/error/error_factory.hpp"



extern "C" {
#include "microhttpd.h"
}


using namespace psme::rest::security::authentication;
using namespace psme::rest::security::session;
using namespace psme::rest::server;

namespace {
int add_request_headers(void* cls, enum MHD_ValueKind /*kind*/,
                        const char* key, const char* value) {
    Request* request = static_cast<Request*>(cls);
    request->set_header(key, value);

    return MHD_YES;
}
}


AuthStatus
SessionAuthentication::perform(MHD_Connection* connection, const std::string& url, Response& response) {
    Request tmp_req;
    MHD_get_connection_values(connection, MHD_HEADER_KIND,
                              &add_request_headers, &tmp_req);

    if (!SessionManager::get_instance()->is_session_valid(tmp_req.get_header(http_headers::XAuthToken::X_AUTH_TOKEN),
                                                          tmp_req.get_header(http_headers::Origin::ORIGIN))) {
        auto message = std::string("Please create session with valid user name and password.");
        auto error = error::ErrorFactory::create_unauthorized_error(url, message);

        response.set_status(error.get_http_status_code());
        response.set_body(error.as_string());
        response.set_header(http_headers::ContentType::CONTENT_TYPE, http_headers::ContentType::JSON);

        return AuthStatus::FAIL;
    }

    return AuthStatus::SUCCESS;
}
