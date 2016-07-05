/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file type.hpp
 *
 * @brief TypeValidator interface
 * */

#ifndef CONFIGURATION_TYPE_VALIDATOR_HPP
#define CONFIGURATION_TYPE_VALIDATOR_HPP

#include "validator.hpp"

namespace json { class Value; }

namespace configuration {

/*! Json type validator class */
class TypeValidator final : public Validator<json::Value> {
public:
    /*!
     * @brief Construct TypeValidator for given type
     * @param type value type
     */
    explicit TypeValidator(const std::string& type) : m_type{type} {}
    ~TypeValidator() = default;

    /*!
     * @brief Validate type for given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    bool is_valid(const json::Value& value) const override final;

    /*!
     * @brief Get type error message
     * @return error message
     */
    std::string get_error() const override final;

private:
    std::string m_type;
};

}

#endif /* CONFIGURATION_TYPE_VALIDATOR_HPP */
