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
 * @file composite_validity_checker.cpp
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/validators/checkers/composite_validity_checker.hpp"



using namespace jsonrpc;


CompositeValidityChecker::CompositeValidityChecker(ValidityChecker::Ptr& _checker1,
                                                   ValidityChecker::Ptr& _checker2) {
    checker1.swap(_checker1);
    checker2.swap(_checker2);
}


void CompositeValidityChecker::validate(const json::Json& value) const {
    if (checker1) {
        checker1->validate(value);
    }
    if (checker2) {
        checker2->validate(value);
    }
}
