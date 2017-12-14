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
#include "agent-framework/logger_ext.hpp"

#include <json/json.hpp>

using namespace configuration;
using namespace psme::rest::validators;
using namespace psme::rest::server;
using namespace psme::rest::error;

json::Value JsonValidator::validate_request_body(const Request& request, const jsonrpc::ProcedureValidator& schema) {
    auto json = deserialize_json_from_request(request);
    schema.validate(json);
    log_debug(GET_LOGGER("rest"), "Request validation passed.");
    return json;
}


void JsonValidator::validate_empty_request(const rest::server::Request& request) {
    if (!request.get_body().empty()) {
        validators::JsonValidator::validate_request_body<validators::schema::EmptyObjectSchema>(request);
    }
}


json::Value JsonValidator::deserialize_json_from_request(const Request& request) {
    json::Deserializer deserializer(request.get_body());
    if (deserializer.is_invalid()) {
        auto deserialization_error = deserializer.get_error();

        if (deserialization_error.code == json::Deserializer::Error::Code::DUPLICATE_KEY) {
            std::stringstream message{};
            message << "Detected duplicate key in JSON at line ";
            message << deserialization_error.line;
            message << " and column " << deserialization_error.column;

            log_error(GET_LOGGER("rest"), "Duplicate key in JSON: " << request.get_body());
            throw ServerException(ErrorFactory::create_property_duplicated_error(deserialization_error.data, message.str()));
        } else {
            std::stringstream message{};
            message << "Requested JSON is malformed at line ";
            message << deserialization_error.line;
            message << " and column " << deserialization_error.column;
            message << " recognized as: " << deserialization_error.decode();

            log_error(GET_LOGGER("rest"), "Malformed JSON: " << request.get_body());
            throw ServerException(ErrorFactory::create_malformed_json_error(message.str()));
        }
    }

    json::Value json;
    deserializer >> json;
    return json;
}
