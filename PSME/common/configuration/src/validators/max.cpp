/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * @file max.cpp
 *
 * @brief MaxValidator implementation
 * */

#include "json-wrapper/json-wrapper.hpp"
#include "configuration/validators/max.hpp"

using namespace configuration;

bool MaxValidator::is_valid(const json::Json& value) const {
    if (!value.is_number()) {
        return false;
    }
    const auto val = value.get<int>();
    return val <= m_max_value;
}

std::string MaxValidator::get_error() const {
    return "Field value is not lower than " + std::to_string(m_max_value) + ".";
}
