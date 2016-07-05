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
 * @file schema_property.hpp
 *
 * @brief SchemaProperty interface
 * */

#ifndef CONFIGURATION_SCHEMA_PROPERTY_HPP
#define CONFIGURATION_SCHEMA_PROPERTY_HPP

#include "schema_errors.hpp"
#include <vector>
#include <memory>

namespace json { class Value; }

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

    using ValidatorJsonSPtr = std::shared_ptr<Validator<json::Value>>;

    /*!
     * @brief Construct SchemaProperty object for given name
     * @param path Property path
     */
    explicit SchemaProperty(const std::string& path, bool mandatory);

    /*!
     * @brief Add validator to property validators list
     * @param validator Validator pointer
     */
    void add_validator(ValidatorJsonSPtr validator);

    /*!
     * @brief Validate given JSON object
     * @param value JSON object
     * @return Error object
     */
    SchemaErrors::Error validate(const json::Value& value) const;

    /*!
     * @brief Return number of validator assigned to property
     * @return Number of validators
     */
    std::size_t validator_count() const { return m_validators.size(); }

    /*!
     * @brief Get property path
     * @return Property path
     */
    const std::string& get_path() const { return m_path; }

    /*!
     * @brief Get property name
     * @return Property name
     */
    bool is_mandatory() const { return m_mandatory; }

private:
    /*! validator list type */
    using validators_t = std::vector<ValidatorJsonSPtr>;

    std::string m_path;
    bool m_mandatory;
    validators_t m_validators{};
};
}

#endif /* CONFIGURATION_SCHEMA_PROPERTY_HPP */
