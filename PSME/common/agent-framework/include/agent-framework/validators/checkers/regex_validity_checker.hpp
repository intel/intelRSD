/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file regex_validity_checker.hpp
 * */

#pragma once

#include "agent-framework/validators/procedure_validator.hpp"

#include <stdarg.h>
#include <regex>

namespace jsonrpc {

class RegexValidityChecker final : public ValidityChecker {

public:

    RegexValidityChecker(va_list& args);

    virtual ~RegexValidityChecker();

    RegexValidityChecker(const RegexValidityChecker&) = default;

    virtual void validate(const json::Json& value) const;

private:

    RegexValidityChecker& operator=(const RegexValidityChecker&) = delete;

    RegexValidityChecker() = delete;

    std::regex matching_regex{};

};

}
