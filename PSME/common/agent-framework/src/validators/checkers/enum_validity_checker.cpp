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
 * @file enum_validity_checker.cpp
 * */

#include "agent-framework/validators/checkers/enum_validity_checker.hpp"

#include "agent-framework/exceptions/exception.hpp"

#include <iterator>
#include <stdarg.h>


using namespace jsonrpc;

namespace {

template<typename T>
std::string join(const T& values, const char* delimiter = " ") {
    std::ostringstream joined{};
    std::copy(std::begin(values), std::end(values),
              std::ostream_iterator<typename T::value_type>(joined, delimiter));
    return joined.str();
}

static const constexpr char EMPTY_VALUE[] = "<empty>";

}


EnumValidityChecker::EnumValidityChecker(va_list& args) {
    is_allowable_value = va_arg(args, is_allowable_value_t);
    get_values = va_arg(args, get_values_t);
}


EnumValidityChecker::~EnumValidityChecker() {}


void EnumValidityChecker::validate(const json::Json& value) const {
    ValidityChecker::validate(value);
    if (!value.is_string()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
              "Property value is not valid string type.",
              value);
    }
    if (!is_allowable_value(value)) {
        if (value.empty()) {
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  agent_framework::exceptions::ErrorCode::INVALID_ENUM,
                  "Empty value is not in constraint values: [ " + join(get_values()) + "]",
                  EMPTY_VALUE
            );
        }
        else {
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  agent_framework::exceptions::ErrorCode::INVALID_ENUM,
                  "Value '" + value.get<std::string>() + "' is not in constraint values: [ " + join(get_values()) + "]",
                  value
            );
        }
    }
}
