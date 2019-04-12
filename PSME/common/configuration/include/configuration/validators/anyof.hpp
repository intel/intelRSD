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
 * @file anyof.hpp
 *
 * @brief AnyOfValidator interface
 * */

#pragma once

#include "validator.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include <algorithm>
#include <vector>
#include <sstream>
#include <iterator>

namespace configuration {

/*!
* @brief Convert container list into string
*
* @tparam T Container template parameter
* @param values container
* @param delimiter Delimiter
*
* @return string object
*/
template <typename T>
std::string join(const T& values, const char* delimiter = " ") {
    std::ostringstream str{};
    std::copy(std::begin(values), std::end(values),
                std::ostream_iterator<typename T::value_type>(str, delimiter));
    return str.str();
}

template <typename T>
class AnyOfValidator : public Validator<json::Json> {
public:
    /*!
     * @brief Construct AnyOfValidator for given parameters
     * @param args argument list
     */
    template <typename... Args>
    AnyOfValidator(Args&&... args) : m_constraints{std::forward<Args>(args)...} {}
    virtual ~AnyOfValidator() = default;

    /*!
     * @brief Validate  for given JSON object
     * @param value JSON object
     * @return On success true is returned, otherwise false
     */
    virtual bool is_valid(const json::Json& value) const override;

    /*!
     * @brief Get anyof error message
     * @return error message
     */
    virtual std::string get_error() const override final;

    /*!
     * @brief Add constraint to list
     * @param args constraint
     */
    template <typename... Args>
    void add_constraint(Args&&... args) {
        m_constraints.emplace_back(std::forward<Args>(args)...);
    }

    /*!
     * @brief Check if value is valid type
     * @param value JSON value to check
     * @return On success true, otherwise false
     */
    bool check_type(const json::Json& value) const;

    /*!
     * @brief Get value of template type
     * @param value JSON object
     * @return json
     */
    T get_value(const json::Json& value) const;

private:

    using types_t = std::vector<T>;
    types_t m_constraints;
};

template <typename T>
bool AnyOfValidator<T>::is_valid(const json::Json& value) const {
    if (!check_type(value)) {
        return false;
    }
    const T val = get_value(value);
    auto pred = [&val](const T& type) { return val == type; };
    return std::any_of(m_constraints.cbegin(), m_constraints.cend(), pred);
}

template <typename T>
std::string AnyOfValidator<T>::get_error() const {
    return "Field is not in constraint values: [ " + join(m_constraints) + "]";
}

template<>
bool AnyOfValidator<int>::check_type(const json::Json& value) const {
    return value.is_number();
}

template<>
bool AnyOfValidator<double>::check_type(const json::Json& value) const {
    return value.is_number();
}

template<>
bool AnyOfValidator<std::string>::check_type(const json::Json& value) const {
    return value.is_string();
}

template<>
int AnyOfValidator<int>::get_value(const json::Json& value) const {
    return value.get<int>();
}

template<>
double AnyOfValidator<double>::get_value(const json::Json& value) const {
    return value.get<double>();
}

template<>
std::string AnyOfValidator<std::string>::get_value(const json::Json& value) const {
    return value.get<std::string>();
}

}

