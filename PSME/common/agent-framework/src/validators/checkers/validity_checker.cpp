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
 * @file validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/validity_checker.hpp"



using namespace jsonrpc;


ValidityChecker::~ValidityChecker() {}


void ValidityChecker::validate(const json::Json& value) const {
    /* Optional and null values are not allowed by default */
    if (&value == &(NON_EXISTING_VALUE)) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              agent_framework::exceptions::ErrorCode::MISSING_FIELD,
              "Mandatory field is not present.", value);
    }
    if (value.is_null()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
              "Value null is not allowed.", value);
    }
}


const json::Json ValidityChecker::NON_EXISTING_VALUE;


ValidityChecker::ValidationException::ValidationException(agent_framework::exceptions::ErrorCode code,
                                                          const std::string& message,
                                                          const json::Json& field_value,
                                                          const std::string& field) :
    m_code(code), m_message(message), m_field(field), m_field_value(field_value) {}


void ValidityChecker::ValidationException::append(const std::string& field) {
    if (!m_field.empty()) {
        m_field.insert(0, "/");
    }
    m_field.insert(0, field);
}
