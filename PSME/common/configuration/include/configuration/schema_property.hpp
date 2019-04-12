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
 * @file schema_property.hpp
 *
 * @brief SchemaProperty interface
 * */

#pragma once



#include "json-wrapper/json-wrapper.hpp"
#include "schema_errors.hpp"
#include <vector>
#include <memory>

namespace configuration {

template <typename T>
class Validator;

/*! Class store collection of validator assigned to json property */
class SchemaProperty {
public:
    static constexpr const char VALIDATOR[] = "validator";
    static constexpr const char MANDATORY[] = "mandatory";
    static constexpr const char MAX[] = "max";
    static constexpr const char MIN[] = "min";
    static constexpr const char MORE_THAN[] = "morethan";
    static constexpr const char TYPE[] = "type";
    static constexpr const char ADDRESS[] = "address";
    static constexpr const char URL[] = "url";
    static constexpr const char ANY_OF[] = "anyof";
    static constexpr const char ADDR_V[] = "addrV";
    static constexpr const char STRING[] = "string";
    static constexpr const char INT[] = "int";

    using ValidatorJsonSPtr = std::shared_ptr<Validator<json::Json>>;

    /*!
     * @brief Construct SchemaProperty object for given name
     * @param path Property path
     * @param mandatory Property is mandatory
     */
    explicit SchemaProperty(const std::string& path, bool mandatory);

    /*!
     * @brief Add validator to property validators list
     * @param validator Validator pointer
     */
    void add_validator(ValidatorJsonSPtr validator);

    /*!
     * @brief Validate given JSON object
     * @param[in] value JSON object
     * @param errors Collection of errors
     * */
    void validate(const json::Json& value, SchemaErrors& errors) const;

    /*!
     * @brief Return number of validator assigned to property
     * @return Number of validators
     * */
    std::size_t validator_count() const {
        return m_validators.size();
    }

    /*!
     * @brief Get property path
     * @return Property path
     */
    const std::string& get_path() const {
        return m_path;
    }

    /*!
     * @brief Is property mandatory
     * @return Property mandatory
     */
    bool is_mandatory() const {
        return m_mandatory;
    }

private:
    /*! Validator list type */
    using Validators = std::vector<ValidatorJsonSPtr>;

    std::string m_path{};
    bool m_mandatory{};
    Validators m_validators{};
};
}

