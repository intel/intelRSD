/*!
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
 *
 * @brief System model interface
 * */

#pragma once
#include "is_framework_enum.hpp"
#include "json_converter.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "optional/optional.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <stdexcept>
#include <ostream>
#include <type_traits>

namespace agent_framework {
namespace module {
namespace utils {

/*!
 * @brief Class representing single data field in REST server
 * OptionalField class is based on std::experimental::optional class, but with
 * simplified interface resembling that of built-in types rather than smart pointers.
 * */
template<typename T>
class OptionalField : public std::experimental::optional<T> {
    template<class U>
    using optional = std::experimental::optional<U>;

public:
    /* Enable copy and move construction */
    OptionalField() = default;
    OptionalField(const OptionalField&) = default;
    OptionalField(OptionalField&&) = default;
    ~OptionalField() = default;
    /* Enable assignment and move assignment */
    OptionalField& operator=(const OptionalField& in_optional_field) = default;
    OptionalField& operator=(OptionalField&& in_optional_field) = default;

    /*!
     * @brief Converting constructor
     *
     * The template character of this converting constructor is needed so that
     * an OptionalField object can be constructed from an object which is not of
     * the same type as OptionalField template parameter (e.g.
     * OptionalValue<std::string> can be constructed from const char [])
     *
     * @param[in] in_value Value used to construct OptionalField object
     * */
    template<typename V>
    OptionalField(const V& in_value) : optional<T>(T(in_value)) {}

    /*!
     * @brief json::Json to OptionalField conversion constructor
     * */
    OptionalField(const json::Json& json) {
        *this = JsonConverter<json::Json>::from_json<T>(json);
    }

    /*!
     * @brief Assignment operator
     *
     * The template character of this converting constructor is needed so that
     * an OptionalField object can be constructed from an object which is not of
     * the same type as OptionalField template parameter (e.g.
     * OptionalValue<std::string> can be constructed from const char [])
     *
     * @param[in] rhs Value to be assigned to OptionalField object
     * */
    template<typename V>
    OptionalField& operator=(const V& rhs) {
        if(!(this->has_value()) || rhs != *this) { optional<T>::operator=(T(rhs)); }
        return *this;
    }

    /*!
     * @brief ObjectField to template parameter conversion operator
     *
     * @return Value held by OptionalField object; may throw.
     * */
    operator T() const {
        return optional<T>::value();
    }

    /*!
     * @brief Reset the OptionalField object to uninitialized state
     * */
    void reset() {
        *this = OptionalField<T>();
    }

    /*!
     * @brief Checks if an OptionalField object has a value assigned to it.
     *
     * @return True if yes, false otherwise.
     * */
    bool has_value() const {
        return optional<T>::operator bool();
    }
};

/* Standalone functions */
/*!
 * @brief Operator '+' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator+(const OptionalField<T>& lhs, const OptionalField<V>& rhs) {
    if(lhs.has_value() && rhs.has_value()) {
        return OptionalField<T>(lhs.value() + rhs.value());
    }
    if(lhs.has_value() && !rhs.has_value()) {
        return lhs;
    }
    if(rhs.has_value() && !lhs.has_value()) {
        return rhs;
    }
    return OptionalField<T>();
}

/*!
 * @brief Operator '+' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator+(const OptionalField<T>& lhs, const V& rhs) {
    if(lhs.has_value()) {
        return OptionalField<T>(lhs.value() + rhs);
    }
    return OptionalField<T>(rhs);
}

/*!
 * @brief Operator '+' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator+(const T& lhs, const OptionalField<V>& rhs) {
    if(rhs.has_value()) {
        return OptionalField<V>(lhs + rhs.value());
    }
    return OptionalField<V>(lhs);
}

/*!
 * @brief Operator '-' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T>
OptionalField<T> operator-(const OptionalField<T>& lhs, const OptionalField<T>& rhs) {
    if(lhs.has_value() && rhs.has_value()) {
        return OptionalField<T>(lhs.value() - rhs.value());
    }
    if(lhs.has_value() && !rhs.has_value()) {
        return lhs;
    }
    if(rhs.has_value() && !lhs.has_value()) {
        return OptionalField<T>(-rhs.value());
    }
    return OptionalField<T>();
}

/*!
 * @brief Operator '-' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator-(const OptionalField<T>& lhs, const V& rhs) {
    if(lhs.has_value()) {
        return OptionalField<T>(lhs.value() - rhs);
    }
    return OptionalField<T>(-rhs);
}

/*!
 * @brief Operator '-' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator-(const T& lhs, const OptionalField<V>& rhs) {
    if(rhs.has_value()) {
        return OptionalField<V>(lhs - rhs.value());
    }
    return OptionalField<V>(lhs);
}

/*!
 * @brief Operator '*' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property. If at least one operand is
 * uninitialized, the result is also uninitialized.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator*(const OptionalField<T>& lhs, const OptionalField<V>& rhs) {
    if(lhs.has_value() && rhs.has_value()) {
        return OptionalField<T>(lhs.value() * rhs.value());
    }
    return OptionalField<T>();
}

/*!
 * @brief Operator '*' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property. If at least one operand is
 * uninitialized, the result is also uninitialized.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator*(const OptionalField<T>& lhs, const V& rhs) {
    if(lhs.has_value()) {
        return OptionalField<T>(lhs.value() * rhs);
    }
    return OptionalField<T>();
}

/*!
 * @brief Operator '*' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property. If at least one operand is
 * uninitialized, the result is also uninitialized.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator*(const T& lhs, const OptionalField<V>& rhs) {
    if(rhs.has_value()) {
        return OptionalField<T>(lhs * rhs.value());
    }
    return OptionalField<T>();
}

/*!
 * @brief Operator '/' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property. If at least one operand is
 * undefined, the operator throws a std::logic_error.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator/(const OptionalField<T>& lhs, const OptionalField<V>& rhs) {
    if(lhs.has_value() && rhs.has_value()) {
        return OptionalField<T>(lhs.value() / rhs.value());
    }
    if(rhs.has_value()) {
        return OptionalField<T>();
    }
    throw std::logic_error("Division undefined.");
}

/*!
 * @brief Operator '/' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property. If at least one operand is
 * undefined, the operator throws a std::logic_error.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator/(const OptionalField<T>& lhs, const V& rhs) {
    if(lhs.has_value()) {
        return OptionalField<T>(lhs.value() / rhs);
    } else {
        return OptionalField<T>();
    }
}

/*!
 * @brief Operator '/' overload for OptionalField objects.
 *
 * The action of the operator is the same as for the type held by the object, so
 * it that it commutes with optionality property. If at least one operand is
 * undefined, the operator throws a std::logic_error.
 *
 * @param[in] lhs Object on the left-hand side of the operator
 * @param[in] rhs Object on the right-hand side of the operator
 * */
template<typename T, typename V>
OptionalField<T> operator/(const T& lhs, const OptionalField<V>& rhs) {
    if(rhs.has_value()) {
        return OptionalField<T>(lhs / rhs.value());
    } else {
        throw std::logic_error("Division undefined.");
    }
}

/*!
 * @brief Output stream insertion operator.
 *
 * If the OptionalField object is value-initialized, then this value is inserted
 * into the output stream, otherwise an error message is inserted.
 *
 * @param[in] out_stream Stream to which OptionalField should be inserted
 * @param[in] in_field OptionalField to be inserted into the stream
 *
 * @return Reference to a stream to which the value was inserted.
 * */
template<typename T>
std::ostream& operator<<(std::ostream& out_stream, const OptionalField<T>& in_field) {
    if(in_field.has_value()) {
        out_stream << in_field.value();
    } else {
        out_stream << "Value undefined!";
    }
    return out_stream;
}

/*!
 * @brief Logical equals operator for OptionalField holding framework enum object
 * and an object representing framework enum.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if the enum value and the object held by OptionalField are the same, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator==(const OptionalField<T>& lhs, const T& rhs) {
    return ( lhs.has_value() ? lhs.value() == rhs : false );
}

/*!
 * @brief Logical equals operator for OptionalField holding framework enum object
 * and an underlying value for framework enum object.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if the enum value and the object held by OptionalField are the same, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator==(const OptionalField<T>& lhs, const typename T::underlying_type& rhs) {
    return ( lhs.has_value() ? lhs.value() == rhs : false );
}

/*!
 * @brief Logical equals operator for OptionalField holding framework enum object
 * and an object representing framework enum.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if the enum value and the object held by OptionalField are the same, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator==(const T& lhs, const OptionalField<T>& rhs) {
    return rhs == lhs;
}

/*!
 * @brief Logical equals operator for OptionalField holding framework enum object
 * and an underlying value for framework enum object.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if the enum value and the object held by OptionalField are the same, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator==(const typename T::underlying_type& lhs, const OptionalField<T>& rhs) {
    return rhs == lhs;
}

/*!
 * @brief Logical not equals operator for OptionalField holding framework enum object
 * and an object representing framework enum.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if both objects are initialized with the same value, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator!=(const OptionalField<T>& lhs, const T& rhs) {
    return !(lhs == rhs);
}

/*!
 * @brief Logical not equals operator for OptionalField holding framework enum object
 * and an underlying value for framework enum object.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if both objects are initialized with the same value, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator!=(const OptionalField<T>& lhs, const typename T::underlying_type& rhs) {
    return !(lhs == rhs);
}

/*!
 * @brief Logical not equals operator for OptionalField holding framework enum object
 * and an object representing framework enum.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if the enum value and the object held by OptionalField are the same, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator!=(const  T& lhs, const OptionalField<T>& rhs) {
    return !(rhs == lhs);
}

/*!
 * @brief Logical not equals operator for OptionalField holding framework enum object
 * and an underlying value for framework enum object.
 *
 * @param[in] lhs Object on the LHS of the operator.
 * @param[in] rhs Object on the RHS of the operator.
 *
 * @return True if the enum value and the object held by OptionalField are the same, false otherwise.
 * */
template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
bool operator!=(const typename T::underlying_type& lhs, const OptionalField<T>& rhs) {
    return !(rhs == lhs);
}

template<typename T, typename std::enable_if<!model::utils::is_framework_enum<T>::value>::type* = nullptr>
void to_json(json::Json& j, const OptionalField<T>& of) {
    if (of.has_value()) {
        j = of.value();
    }
}

template<typename T, typename std::enable_if<model::utils::is_framework_enum<T>::value>::type* = nullptr>
void to_json(json::Json& j, const OptionalField<T>& of) {
    if (of.has_value()) {
        j = of.value().to_string();
    }
}

}
}
}

/* Alias set for easy usage of OptionalField class outside the namespace */
template <typename T>
using OptionalField = agent_framework::module::utils::OptionalField<T>;
