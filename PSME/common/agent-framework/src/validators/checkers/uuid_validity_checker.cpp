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
 * @file uuid_validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/uuid_validity_checker.hpp"



using namespace jsonrpc;


UuidValidityChecker::UuidValidityChecker() {}


void UuidValidityChecker::validate(const json::Json& value) const {
    ValidityChecker::validate(value);

    if (!value.is_string()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
              "Property value is not valid string type.", value);
    }

    std::string sval = value;
    static const std::string uuid_pattern{"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"};
    if (sval.length() != uuid_pattern.length()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
              "Incorrect UUID length.", value);
    }

    for (unsigned i = 0; i < uuid_pattern.length(); i++) {
        if (uuid_pattern.at(i) == 'x') {
            if (!is_hex(sval.at(i))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Invalid UUID hex number.", value);
            }
        }
        else if (uuid_pattern.at(i) != sval.at(i)) {
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                  "Invalid UUID format.", value);
        }
    }
}


constexpr bool UuidValidityChecker::is_hex(char c) {
    return ((c >= '0') && (c <= '9')) ||
           ((c >= 'a') && (c <= 'f')) ||
           ((c >= 'A') && (c <= 'F'));
}
