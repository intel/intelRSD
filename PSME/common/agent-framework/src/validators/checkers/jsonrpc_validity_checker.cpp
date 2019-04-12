/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file jsonrpc_validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/jsonrpc_validity_checker.hpp"

#include "generic/assertions.hpp"



using namespace jsonrpc;


JsonrpcValidityChecker::JsonrpcValidityChecker(jsontype_t _type) : type(_type) {}


void JsonrpcValidityChecker::validate(const json::Json& value) const {
    ValidityChecker::validate(value);

    switch (type) {
        case jsontype_t::JSON_STRING:
            if (!value.is_string()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid string type.", value);
            }
            break;
        case jsontype_t::JSON_BOOLEAN:
            if (!value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid boolean type.", value);
            }
            break;
        case jsontype_t::JSON_INTEGER:
            if (!value.is_number_integer()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid integer type.", value);
            }
            break;
        case jsontype_t::JSON_REAL:
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid real type.", value);
            }
            break;
        case jsontype_t::JSON_OBJECT:
            if (!value.is_object()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid object type.", value);
            }
            break;
        case jsontype_t::JSON_ARRAY:
            if (!value.is_array()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid array type.", value);
            }
            break;
        default:
            assert(generic::FAIL("Allowed JSON type"));
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                  "Property value is not allowed JSON type.", value);
            break;
    }
}
