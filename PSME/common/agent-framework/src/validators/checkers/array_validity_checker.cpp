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
 * @file array_validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/array_validity_checker.hpp"

#include "generic/assertions.hpp"



using namespace jsonrpc;
using namespace agent_framework::exceptions;


ArrayValidityChecker::ArrayValidityChecker(ValidityChecker::Ptr& _checker,
                                           unsigned _min, unsigned _max) : min(_min), max(_max) {
    checker.swap(_checker);
}


void ArrayValidityChecker::validate(const json::Json& value) const {
    ValidityChecker::validate(value);
    if (!value.is_array()) {
        THROW(ValidityChecker::ValidationException, "agent-framework",
              ErrorCode::INVALID_FIELD_TYPE, "Property value is not valid array type.", value);
    }
    if (max < min) {
        assert(generic::FAIL("Wrong array size."));
        return;
    }

    if ((min <= value.size()) && (value.size() <= max)) {
        if (checker) {
            /* Rethrowing an exception, almost copy from ProcedureValidator::validate(). */
            for (unsigned index = 0; index < value.size(); index++) {
                std::string id = std::to_string(index);

                try {
                    checker->validate(value[index]);
                }
                catch (ValidityChecker::ValidationException& ex) {
                    ex.append(id);
                    /* rethrow exception with current field appended */
                    throw ex;
                }
            }
        }
    }
    else {
        THROW(ValidityChecker::ValidationException, "agent-framework", ErrorCode::INVALID_VALUE_FORMAT,
              "Invalid array size. Valid size range is <" + std::to_string(min) + "; " + std::to_string(max) + ">.",
              value);
    }
}
