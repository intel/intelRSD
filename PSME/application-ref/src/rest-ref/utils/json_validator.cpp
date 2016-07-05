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
 * */

#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/utils/json_validator.hpp"

#include "configuration/schema_validator.hpp"
#include "configuration/schema_errors.hpp"
#include "configuration/schema_reader.hpp"
#include "configuration/schema_property.hpp"
#include "agent-framework/logger_ext.hpp"

#include <json/json.hpp>

using namespace configuration;
using namespace psme::rest::utils;
using namespace psme::rest::server;
using namespace psme::rest::error;

constexpr const char JsonValidator::STRING_TYPE[];
constexpr const char JsonValidator::DOUBLE_TYPE[];
constexpr const char JsonValidator::ARRAY_TYPE[];
constexpr const char JsonValidator::BOOL_TYPE[];
constexpr const char JsonValidator::UINT_TYPE[];
constexpr const char JsonValidator::OBJECT_TYPE[];
constexpr const char JsonValidator::NUMBER_TYPE[];

json::Value JsonValidator::validate_request_body(const Request& request,
                                  const json::Value& schema) {
    json::Deserializer deserializer(request.get_body());

    if (deserializer.is_invalid()) {
        log_error(GET_LOGGER("rest"), "Malformed JSON: " << request.get_body());
        ServerError error = ErrorFactory::create_invalid_payload_error();
        error.add_extended_message("Requested JSON is malformed.");
        throw ServerException(error);
    }

    json::Value json;
    deserializer >> json;

    return validate_request_body(json, schema);
}

json::Value JsonValidator::validate_request_body(const json::Value& json,
                                  const json::Value& schema) {
    SchemaErrors errors;
    SchemaValidator validator;
    SchemaReader reader;
    reader.load_schema(schema, validator);
    validator.validate(json, errors);

    if (errors.count()) {
        log_error(GET_LOGGER("rest"), "Bad request:\n" << errors.to_string());
        ServerError error = ErrorFactory::create_invalid_payload_error();
        for (const auto& e : errors.get_errors()) {
            for (const auto& message : e.get_messages()) {
                error.add_extended_message(message);
            }
        }
        throw ServerException(error);
    }

    log_debug(GET_LOGGER("rest"), "Request validation passed.")
    return json;
}

json::Value JsonValidator::any_of(const std::string& type_to_validate,
                                  const json::Value& any_of_values,
                                  bool mandatory) {

    using json::Pair;
    return json::Value({
        Pair(SchemaProperty::VALIDATOR, true),
        Pair(SchemaProperty::TYPE, type_to_validate),
        Pair(SchemaProperty::ANY_OF, any_of_values),
        Pair(SchemaProperty::MANDATORY, mandatory)
    });
}

json::Value JsonValidator::type_of(const std::string& type_to_validate,
                                   bool mandatory) {

    using json::Pair;
    return json::Value({
        Pair(SchemaProperty::VALIDATOR, true),
        Pair(SchemaProperty::TYPE, type_to_validate),
        Pair(SchemaProperty::MANDATORY, mandatory)
    });
}

json::Value JsonValidator::more_than(const double min_value, bool mandatory) {

    using json::Pair;
    return json::Value({
        Pair(SchemaProperty::VALIDATOR, true),
        Pair(SchemaProperty::MORE_THAN, min_value),
        Pair(SchemaProperty::MANDATORY, mandatory)
    });
}
