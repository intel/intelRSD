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
 * @file url.hpp
 *
 * @brief URL validator interface
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include "validator.hpp"
#include <regex>



namespace configuration {

/*! Json url validator class */
class UrlValidator final : public Validator<json::Json> {
public:
    /*!
     * @brief Construct UrlValidator for given pattern
     * @param url URL pattern
     */
    explicit UrlValidator(const std::string& url = {});
    ~UrlValidator() = default;

    /*!
     * @brief Validate URL for given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    bool is_valid(const json::Json& value) const override final;

    /*!
     * @brief Get URL error message
     * @return error message
     */
    std::string get_error() const override final;

private:
    /*!
     * @brief Create regex object for given url pattern
     * @param url URL pattern
     * @return regex object
     */
    std::regex make_regex(const std::string& url);

    /*! url regex object */
    std::regex m_url_regex;
};

}

