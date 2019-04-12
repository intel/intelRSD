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
 * @file url.cpp
 *
 * @brief UrlValidator Implementation
 * */

#include "configuration/validators/url.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace configuration;

UrlValidator::UrlValidator(const std::string& url) :
    m_url_regex{make_regex(url)} {}

bool UrlValidator::is_valid(const json::Json& value) const {
    if (!value.is_string()) {
        return false;
    }

    const auto& val = value.get<std::string>();
    return std::regex_match(val, m_url_regex);
}

std::string UrlValidator::get_error() const {
    return "Field value is not valid URL address.";
}

std::regex UrlValidator::make_regex(const std::string& url) {
    (void) url;
    return std::regex{R"(^(https?|ftp)://(-\.)?([^\s/?\.#-]+\.?)+(/[^\s]*)?$)"};
}
