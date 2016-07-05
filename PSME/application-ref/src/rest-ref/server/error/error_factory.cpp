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
 *
 * */
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/server/status.hpp"

using namespace psme::rest::error;
using namespace psme::rest::server::status_4XX;
using namespace psme::rest::server::status_5XX;

ServerError ErrorFactory::create_not_found_error(const std::string& message) {
    ServerError server_error{NOT_FOUND};
    server_error.set_code("InvalidEndpoint");
    server_error.set_message("Invalid endpoint in /redfish/v1 namespace");
    if(!message.empty()) {
        server_error.add_extended_message(message);
    }
    return server_error;
}

ServerError ErrorFactory::create_malformed_uri_error(const std::string& message) {
    ServerError server_error{BAD_REQUEST};
    server_error.set_code("MalformedUri");
    server_error.set_message("Malformed URI");
    if(!message.empty()) {
        server_error.add_extended_message(message);
    }
    return server_error;
}

ServerError ErrorFactory::create_method_not_allowed_error() {
    ServerError server_error{METHOD_NOT_ALLOWED};
    server_error.set_code("MethodNotAllowed");
    server_error.set_message("Method Not Allowed");
    return server_error;
}

ServerError ErrorFactory::create_unauthorized_error() {
    ServerError server_error{UNAUTHORIZED};
    server_error.set_code("Unauthorized");
    server_error.set_message("Unauthorized");
    return server_error;
}

ServerError ErrorFactory::create_invalid_payload_error() {
    ServerError server_error{BAD_REQUEST};
    server_error.set_code("InvalidPayload");
    server_error.set_message("Request payload is invalid or missing");
    return server_error;
}

ServerError ErrorFactory::create_conflict_error() {
    ServerError server_error{CONFLICT};
    server_error.set_code("Conflict");
    server_error.set_message("Conflict");
    return server_error;
}

ServerError ErrorFactory::create_agent_unreachable_error() {
    ServerError server_error{SERVICE_UNAVAILABLE};
    server_error.set_code("ServiceUnavailable");
    server_error.set_message("Agent unreachable");
    return server_error;
}

ServerError ErrorFactory::create_agent_methond_not_implemented_error() {
    ServerError server_error{NOT_IMPLEMENTED};
    server_error.set_code("NotImplemented");
    server_error.set_message("Method not implemented in agent");
    return server_error;
}
