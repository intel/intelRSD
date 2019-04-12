/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation.
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
 * @brief Utils for string operations
 * @file utils/string_utils.cpp
 */

#include "utils/string_utils.hpp"
#include <algorithm>

using namespace utils;

bool StringUtils::starts_with(const std::string& source, const std::string& target) {
    return source.substr(0, target.length()) == target;
}

std::string StringUtils::trim_left(std::string input) {
    input.erase(input.begin(),
        std::find_if(input.begin(), input.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return input;
}

std::string StringUtils::trim_right(std::string input) {
    input.erase(std::find_if(input.rbegin(), input.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
        input.end());
    return input;
}

std::string StringUtils::trim(std::string input) {
    return trim_left(trim_right(input));
}

std::string StringUtils::substring_after_last_of(const std::string& input, char character) {
    std::size_t found = input.find_last_of(character);
    if (std::string::npos != found) {
        return input.substr(found + 1);
    }
    return empty();
}

const std::string& StringUtils::empty() {
    static const std::string empty{};
    return empty;
}
