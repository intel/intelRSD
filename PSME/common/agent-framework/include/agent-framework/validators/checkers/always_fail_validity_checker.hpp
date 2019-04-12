/*!
 * @copyright Copyright (c) 2016-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file always_fail_validity_checker.hpp
 * */
#pragma once

#include "validity_checker.hpp"

namespace jsonrpc {

/*!
 * @brief Always fail validity checker
 *
 * Always reports error message
 * */
class AlwaysFailValidityChecker final : public ValidityChecker {
public:
    AlwaysFailValidityChecker(const std::string& _message);

    virtual ~AlwaysFailValidityChecker();

    virtual void validate(const json::Json&) const __attribute((noreturn));

private:
    /*! @brief Reported fail message */
    const std::string message;
};

}
