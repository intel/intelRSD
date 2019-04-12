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
 * @file number_validity_checker.cpp
 * */

#include "agent-framework/validators/checkers/number_validity_checker.hpp"

#include "agent-framework/exceptions/exception.hpp"
#include "generic/assertions.hpp"

#include <safe-string/safe_lib.hpp>
#include <cmath>



using namespace jsonrpc;

namespace {

template<typename T>
std::string get_out_of_range_message(const std::string& type, T min, T max) {
    return type + " value out of range [" + std::to_string(min) +
           ", " + std::to_string(max) + "].";

}

}


NumberValidityChecker::NumberValidityChecker() :
    type(ProcedureValidator::NumberType::TYPE_ANYTHING), has_min(false), has_max(false) {
    memset_s(&min, sizeof(ValueUnion), 0);
    memset_s(&max, sizeof(ValueUnion), 0);
}


NumberValidityChecker::NumberValidityChecker(va_list& args) {
    memset_s(&min, sizeof(ValueUnion), 0);
    memset_s(&max, sizeof(ValueUnion), 0);

    unsigned t = va_arg(args, unsigned);
    has_min = ((t & VALID_NUMERIC_TYPE(ANYTHING, MIN)) == VALID_NUMERIC_TYPE(ANYTHING, MIN));
    has_max = ((t & VALID_NUMERIC_TYPE(ANYTHING, MAX)) == VALID_NUMERIC_TYPE(ANYTHING, MAX));

    /* clear bits responsible for number of arguments to be taken from args */
    type = static_cast<ProcedureValidator::NumberType>(t & (~VALID_NUMERIC_TYPE(ANYTHING, BOTH)));

    switch (type) {
        case ProcedureValidator::NumberType::TYPE_INT32:
            if (has_min) {
                min.i32 = va_arg(args, int32_t);
            }
            if (has_max) {
                max.i32 = va_arg(args, int32_t);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_UINT32:
            if (has_min) {
                min.ui32 = va_arg(args, uint32_t);
            }
            if (has_max) {
                max.ui32 = va_arg(args, uint32_t);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_INT64:
            if (has_min) {
                min.i64 = va_arg(args, int64_t);
            }
            if (has_max) {
                max.i64 = va_arg(args, int64_t);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_UINT64:
            if (has_min) {
                min.ui64 = va_arg(args, uint64_t);
            }
            if (has_max) {
                max.ui64 = va_arg(args, uint64_t);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_DOUBLE:
            if (has_min) {
                min.d = va_arg(args, double);
            }
            if (has_max) {
                max.d = va_arg(args, double);
            }
            break;

            /* not allowed types */
        case ProcedureValidator::NumberType::TYPE_ANYTHING:
        case ProcedureValidator::NumberType::WRONG_TYPE:
        default:
            assert(generic::FAIL("Not allowed number type"));
            type = ProcedureValidator::NumberType::WRONG_TYPE;
            break;
    }
}


void NumberValidityChecker::validate(const json::Json& value) const {
    ValidityChecker::validate(value);

    double integral{0.0};
    double fractional{0.0};
    if (value.is_number()) {
        fractional = modf(value, &integral);
    }

    switch (type) {
        case ProcedureValidator::NumberType::TYPE_ANYTHING:
            if (value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid number type.", value);
            }
            break;

        case ProcedureValidator::NumberType::TYPE_INT32:
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid 32-bit integer type.", value);
            }
            if (value.is_number_float() && fractional != 0.0) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if ((has_min && (value.get<std::int64_t>() < min.i32)) ||
                (has_max && (value.get<std::int64_t>() > max.i32))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("32-bit integer", min.i32, max.i32), value);
            }
            if (value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;

        case ProcedureValidator::NumberType::TYPE_UINT32:
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid unsigned 32-bit number type.", value);
            }
            if (value.is_number_float() && fractional != 0.0) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if ((value.get<double>() < 0)) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Property value is not valid a positive number.", value);
            }
            if ((has_min && (value.get<std::uint64_t>() < min.ui32)) ||
                (has_max && (value.get<std::uint64_t>() > max.ui32))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("Unsigned 32-bit integer", min.ui32, max.ui32), value);
            }
            if (value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;

        case ProcedureValidator::NumberType::TYPE_INT64:
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid number type.", value);
            }
            if (value.is_number_float() && fractional != 0.0) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if (value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }

            // Checking min/max integers by double value (approximately) and user defined min/max
            if (value.get<double>() < std::numeric_limits<std::int64_t>::min() ||
                value.get<double>() > std::numeric_limits<std::int64_t>::max()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("64-bit integer",
                          std::numeric_limits<std::int64_t>::min(), std::numeric_limits<std::int64_t>::max()), value);
            }
            if ((has_min && (value.get<std::int64_t>() < min.i64)) ||
                (has_max && (value.get<std::int64_t>() > max.i64))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("64-bit integer", min.i64, max.i64), value);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_UINT64:
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid unsigned number type.", value);
            }
            if (value.is_number_float() && fractional != 0.0) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if ((value.get<double>() < 0)) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Property value is not valid a positive number.", value);
            }
            if (value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }

            // Checking min/max integers by double value (approximately) and user defined min/max
            if (value.get<double>() < std::numeric_limits<std::uint64_t>::min() ||
                value.get<double>() > std::numeric_limits<std::uint64_t>::max()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("Unsigned 64-bit integer",
                          std::numeric_limits<std::uint64_t>::min(), std::numeric_limits<std::uint64_t>::max()), value);
            }
            if ((has_min && (value.get<std::uint64_t>() < min.ui64)) ||
                (has_max && (value.get<std::uint64_t>() > max.ui64))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("Unsigned 64-bit integer", min.ui64, max.ui64), value);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_DOUBLE:
            if (!value.is_number()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid number type.", value);
            }
            if ((has_min && (value.get<double>() < min.d)) ||
                (has_max && (value.get<double>() > max.d))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      ::get_out_of_range_message("Double precision number", min.d, max.d), value);
            }
            if (value.is_boolean()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;

        case ProcedureValidator::NumberType::WRONG_TYPE:
        default:
            assert(generic::FAIL("Not allowed number type."));
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                  "Not allowed number type.", value);
            break;
    }
}
