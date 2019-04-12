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
 * @file validator.hpp
 *
 * @brief Configuration Validator interface
 * */

#pragma once
#include <string>
#include <memory>

/*! Configuration namespace */
namespace configuration {

/*! Abstract validator base class */
template <typename T>
class Validator {
public:
    virtual ~Validator() = default;

    /*!
     * @brief Validate given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    virtual bool is_valid(const T& value) const = 0;

    /*!
     * @brief Get error message
     * @return error message
     */
    virtual std::string get_error() const = 0;
};

}

