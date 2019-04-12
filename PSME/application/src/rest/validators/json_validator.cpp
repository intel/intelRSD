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
 * @file json_validator.cpp
 * @brief JSON validator for REST API
 * */

#include "psme/rest/server/error/message_object.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/common.hpp"

#include "configuration/schema_validator.hpp"
#include "configuration/schema_errors.hpp"
#include "configuration/schema_reader.hpp"
#include "configuration/schema_property.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace configuration;
using namespace psme::rest::validators;
using namespace psme::rest::server;
using namespace psme::rest::error;

json::Json JsonValidator::validate_request_body(const Request& request, const jsonrpc::ProcedureValidator& schema) {
    auto json = deserialize_json_from_request(request);
    schema.validate(json);
    log_debug("rest", "Request validation passed.");
    return json;
}


void JsonValidator::validate_empty_request(const rest::server::Request& request) {
    if (!request.get_body().empty()) {
        validators::JsonValidator::validate_request_body<validators::schema::EmptyObjectSchema>(request);
    }
}


json::Json JsonValidator::deserialize_json_from_request(const Request& request) {
    json::Json json = json::Json();
    try {
        json = json::Json::parse(request.get_body());
    }
    catch (const std::invalid_argument& e) {
        throw ServerException(ErrorFactory::create_malformed_json_error(e.what()));
    }
    return json;
}
