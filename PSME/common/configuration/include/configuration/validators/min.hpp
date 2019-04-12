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
 * @file min.hpp
 *
 * @brief MinValidator interface
 * */

#pragma once
#include "validator.hpp"
#include "json-wrapper/json-wrapper.hpp"


namespace configuration {

/*! Json min validator class */
class MinValidator final : public Validator<json::Json> {
public:
    /*!
     * @brief Construct MinValidator for given value
     * @param value minimal accepted value
     */
    explicit MinValidator(const int value) : m_min_value{value} {}
    ~MinValidator() = default;

    /*!
     * @brief Validate value for given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    bool is_valid(const json::Json& value) const override final;

    /*!
     * @brief Get min error message
     * @return error message
     */
    std::string get_error() const override final;

private:
    /*! minimal valid value */
    int m_min_value;
};

}

