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
 * @file address.hpp
 *
 * @brief AddressValidator interface
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include "validator.hpp"
#include <regex>



namespace configuration {

/*! Json address validator class */
class AddressValidator final : public Validator<json::Json> {
public:
    /*!
     * @brief Construct AddressValidator for given address pattern
     * @param address address pattern
     */
    explicit AddressValidator(const std::string& address);
    ~AddressValidator() = default;

    /*!
     * @brief Validate address for given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    bool is_valid(const json::Json& value) const override final;

    /*!
     * @brief Get address error message
     * @return error message
     */
    std::string get_error() const override final;

private:
    std::regex make_regex(const std::string& address);

    /*! address regex object */
    std::regex m_address_regex;
};

}

