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
 * @file schema_validator.hpp
 *
 * @brief SchemaValidator interface
 * */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

#include <vector>
#include <memory>



namespace configuration {

class SchemaErrors;
class SchemaProperty;

/*! Validate JSON object for given property list */
class SchemaValidator {
public:
    explicit SchemaValidator();
    ~SchemaValidator();

    /*!
     * @brief Add property to check
     * @param property SchemaProperty object
     */
    void add_property(const SchemaProperty& property);

    /*!
     * @brief Validate given JSON object
     * @param json JSON object to validate
     * @param errors JSON object errors
     */
    void validate(const json::Json& json, SchemaErrors& errors);

private:
    /*! pimpl idiom */
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}

