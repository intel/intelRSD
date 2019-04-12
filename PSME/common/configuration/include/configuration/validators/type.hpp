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
 * @file type.hpp
 *
 * @brief TypeValidator interface
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include "validator.hpp"

namespace configuration {

/*! Json type validator class */
class TypeValidator final : public Validator<json::Json> {
public:
    /*!
     * @brief Construct TypeValidator for given type
     * @param type value type
     */
    explicit TypeValidator(const std::string& type) : m_type{type} { }

    /*! @brief Default destructor */
    ~TypeValidator() = default;

    /*!
     * @brief Validate type for given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    bool is_valid(const json::Json& value) const override final;

    /*!
     * @brief Get type error message
     * @return error message
     */
    std::string get_error() const override final;

private:
    std::string m_type;
};

}

