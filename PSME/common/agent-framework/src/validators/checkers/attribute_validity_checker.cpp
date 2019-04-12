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
 * @file attribute_validity_checker.cpp
 * */

#include "agent-framework/validators/checkers/attribute_validity_checker.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include "agent-framework/exceptions/exception.hpp"

#include <stdarg.h>


using namespace jsonrpc;
using namespace agent_framework;


AttributeValidityChecker::AttributeValidityChecker(va_list& args) {
    static_procedure_getter_t getter = va_arg(args, static_procedure_getter_t);
    if (getter) {
        validator = &(getter());
    }
}


AttributeValidityChecker::~AttributeValidityChecker() {}


void AttributeValidityChecker::validate(const json::Json& value) const {
    ValidityChecker::validate(value);
    if (!value.is_object()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              exceptions::ErrorCode::INVALID_FIELD_TYPE, "Property value is not valid object type.", value);
    }

    /* validate using given procedure_validator */
    if (validator) {
        try {
            validator->validate(value);
        }
        catch (const exceptions::GamiException& ex) {
            /* Throw internal exception */
            throw ValidityChecker::ValidationException(
                ex.get_error_code(), ex.get_message(),
                exceptions::InvalidField::get_field_value_as_json_from_json_data(ex.get_data()),
                exceptions::InvalidField::get_field_name_from_json_data(ex.get_data())
            );
        }
    }
}
