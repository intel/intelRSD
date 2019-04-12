/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file regex_validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/regex_validity_checker.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"

using namespace jsonrpc;
using namespace agent_framework::model::literals;

RegexValidityChecker::RegexValidityChecker(va_list& args)
    : matching_regex(va_arg(args, const char*)) {}

RegexValidityChecker::~RegexValidityChecker() {}

void RegexValidityChecker::validate(const json::Json &value) const {
    ValidityChecker::validate(value);

    if (!value.is_string()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
            agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
            "Property value is not valid string type.",
            value);
    }

    if (!regex_match(value.get<std::string>(), matching_regex)) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
              "Value is malformed or does not match its expected form.",
              value);
    }
}
