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
 * @file optional_validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/optional_validity_checker.hpp"



using namespace jsonrpc;


OptionalValidityChecker::OptionalValidityChecker(ValidityChecker::Ptr& _checker) {
    checker.swap(_checker);
}


void OptionalValidityChecker::validate(const json::Json& value) const {
    /* compare object directly! It is internal "marker" */
    if (&value == &(NON_EXISTING_VALUE)) {
        return;
    }
    if (checker) {
        checker->validate(value);
    }
}
