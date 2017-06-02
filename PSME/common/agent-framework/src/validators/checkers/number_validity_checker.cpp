/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/number_validity_checker.hpp"

#include <safe-string/safe_lib.hpp>
#include <cassert>
#include <cmath>



using namespace jsonrpc;


NumberValidityChecker::NumberValidityChecker() :
    type(ProcedureValidator::NumberType::TYPE_ANYTHING), has_min(false), has_max(false) {
    memset_s(&min, sizeof(ValueUnion), 0);
    memset_s(&max, sizeof(ValueUnion), 0);
}


NumberValidityChecker::NumberValidityChecker(va_list args) {
    memset_s(&min, sizeof(ValueUnion), 0);
    memset_s(&max, sizeof(ValueUnion), 0);

    unsigned t = va_arg(args, unsigned);
    has_min = ((t & VALID_NUMERIC_TYPE(ANYTHING, MIN)) == VALID_NUMERIC_TYPE(ANYTHING, MIN));
    has_max = ((t & VALID_NUMERIC_TYPE(ANYTHING, MAX)) == VALID_NUMERIC_TYPE(ANYTHING, MAX));

    /* clear bits responsible for number of arguments to be taken from args */
    type = static_cast<ProcedureValidator::NumberType>
    (t & (~VALID_NUMERIC_TYPE(ANYTHING, BOTH)));

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
            assert(fail("Not allowed number type"));
            type = ProcedureValidator::NumberType::WRONG_TYPE;
            break;
    }
}


bool NumberValidityChecker::is_integral(double d) {
    double result = std::fabs(std::fmod(d, 1.0));
    if (result < -0.5) {
        result += 1.0;
    }
    else if (result > 0.5) {
        result -= 1.0;
    }
    if (result < 0) {
        result = -result;
    }
    return result < 1.0e-10;
}


void NumberValidityChecker::validate(const Json::Value& value) const {
    ValidityChecker::validate(value);

    switch (type) {
        case ProcedureValidator::NumberType::TYPE_ANYTHING:
            if (value.isBool()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            if (!value.isNumeric()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid number type.", value);
            }
            break;

        case ProcedureValidator::NumberType::TYPE_INT32:
            if (!value.isNumeric() || !value.isConvertibleTo(Json::ValueType::intValue)) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid 32-bit integer type.", value);
            }
            if (value.isDouble() && (!is_integral(value.asDouble()))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if ((has_min && (value.asInt() < min.i32)) ||
                (has_max && (value.asInt() > max.i32))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "32-bit integer value out of range.", value);
            }
            if (value.isBool()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;

        case ProcedureValidator::NumberType::TYPE_UINT32:
            if (!value.isNumeric() || !value.isConvertibleTo(Json::ValueType::uintValue)) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid unsigned 32-bit number type.", value);
            }
            if (value.isDouble() && (!is_integral(value.asDouble()))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if ((has_min && (value.asUInt() < min.ui32)) ||
                (has_max && (value.asUInt() > max.ui32))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Unsigned 32-bit integer value out of range.", value);
            }
            if (value.isBool()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;

        case ProcedureValidator::NumberType::TYPE_INT64:
            if (!value.isNumeric()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      " Property value is not valid number type.", value);
            }
            if (value.isDouble() && (!is_integral(value.asDouble()))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if (!value.isInt64() || value.type() == Json::ValueType::realValue ||
                (has_min && (value.asInt64() < min.i64)) ||
                (has_max && (value.asInt64() > max.i64))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Value out of Int64 range.", value);
            }
            if (value.isBool()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_UINT64:
            if (!value.isNumeric()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid unsigned number type.", value);
            }
            if (value.isDouble() && (!is_integral(value.asDouble()))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Provided number value with a fractional part.", value);
            }
            if (!value.isUInt64() ||(has_min && (value.asUInt() < min.ui64)) ||
                (has_max && (value.asUInt() > max.ui64))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Value out of UInt64 range.", value);
            }
            if (value.isBool()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;
        case ProcedureValidator::NumberType::TYPE_DOUBLE:
            if (!value.isNumeric()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Property value is not valid number type.", value);
            }
            if ((has_min && (value.asDouble() < min.d)) ||
                (has_max && (value.asDouble() > max.d))) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_VALUE_FORMAT,
                      "Value out of range.", value);
            }
            if (value.isBool()) {
                THROW(ValidityChecker::ValidationException, "agent-framework",
                      agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                      "Boolean value is not a number.", value);
            }
            break;

        case ProcedureValidator::NumberType::WRONG_TYPE:
        default:
            assert(fail("Allowed number type"));
            THROW(ValidityChecker::ValidationException, "agent-framework",
                  agent_framework::exceptions::ErrorCode::INVALID_FIELD_TYPE,
                  "Not allowed number type.", value);
            break;
    }
}
