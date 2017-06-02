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
using json::Pair;

constexpr const char JsonValidator::STRING_TYPE[];
constexpr const char JsonValidator::DOUBLE_TYPE[];
constexpr const char JsonValidator::ARRAY_TYPE[];
constexpr const char JsonValidator::BOOL_TYPE[];
constexpr const char JsonValidator::UINT_TYPE[];
constexpr const char JsonValidator::OBJECT_TYPE[];
constexpr const char JsonValidator::NUMBER_TYPE[];

namespace {
    void set_mandatory(json::Value& constraints, bool is_mandatory) {
        constraints[SchemaProperty::MANDATORY] = is_mandatory;
    }
}

json::Value JsonValidator::validate_request_body(const Request& request, const jsonrpc::ProcedureValidator& schema) {
    auto json = deserialize_json_from_request(request);
    schema.validate(json);
    log_debug(GET_LOGGER("rest"), "Request validation passed.");
    return json;
}

json::Value JsonValidator::validate_request_body(const Request& request, const json::Value& schema) {
    auto json = deserialize_json_from_request(request);
    log_debug(GET_LOGGER("rest"), "Request validation passed.");
    return validate_request_body(json, schema);
}

json::Value JsonValidator::validate_request_body(const json::Value& json, const json::Value& schema) {
    SchemaErrors errors;
    SchemaValidator validator;
    SchemaReader reader;

    reader.load_schema(schema, validator);
    validator.validate(json, errors);

    // Error(s) occurred
    if (errors.count() > 0) {
        log_error(GET_LOGGER("rest"), "Bad request:\n" << errors.to_string());
        ServerError error = ErrorFactory::create_invalid_payload_error();
        for (const auto& e : errors.get_errors()) {
            MessageObject message_object{error.get_code(), e.get_message()};
            for (const auto& path : e.get_paths()) {
                message_object.add_related_property(path);
            }
            error.add_extended_message(message_object);
        }
        throw ServerException(error);
    }

    log_debug(GET_LOGGER("rest"), "Request validation passed.");
    return json;
}


void JsonValidator::validate_empty_request(const rest::server::Request& request) {
    if (!request.get_body().empty()) {
        validators::JsonValidator::validate_request_body<validators::schema::EmptyObjectSchema>(request);
    }
}


json::Value JsonValidator::any_of(const std::string& type_to_validate, const json::Value& any_of_values) {
    return json::Value({
        Pair(SchemaProperty::VALIDATOR, true),
        Pair(SchemaProperty::TYPE, type_to_validate),
        Pair(SchemaProperty::ANY_OF, any_of_values)
    });
}

json::Value JsonValidator::has_type(const std::string& type_to_validate) {
    return json::Value({
        Pair(SchemaProperty::VALIDATOR, true),
        Pair(SchemaProperty::TYPE, type_to_validate)
    });
}

json::Value JsonValidator::more_than(const double min_value) {
    return json::Value({
        Pair(SchemaProperty::VALIDATOR, true),
        Pair(SchemaProperty::MORE_THAN, min_value)
    });
}

json::Value JsonValidator::optional(const std::string& field_name, json::Value constraints) {
    set_mandatory(constraints, false);
    return Pair(field_name, constraints);
}

json::Value JsonValidator::mandatory(const std::string& field_name, json::Value constraints) {
    set_mandatory(constraints, true);
    return Pair(field_name, constraints);
}

json::Value JsonValidator::object(const std::string& field_name, const json::Value& object_of) {
    return Pair(field_name, object_of);
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
