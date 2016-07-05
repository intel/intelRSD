/*!
 * @copyright
 * Copyright (c) 2015, Tymoteusz Blazejczyk
 *
 * @copyright
 * All rights reserved.
 *
 * @copyright
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * @copyright
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * @copyright
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * @copyright
 * * Neither the name of json-cxx nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * @copyright
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file json/number.hpp
 *
 * @brief JSON number interface
 * */

#ifndef JSON_CXX_NUMBER_HPP
#define JSON_CXX_NUMBER_HPP

#include <cstdint>

namespace json {

/*! Unsigned integer */
using Uint = unsigned int;

/*! Signed integer */
using Int = int;

/*! Unsigned 64-bit integer */
using Uint64 = std::uint64_t;

/*! Signed 64-bit integer */
using Int64 = std::int64_t;

/*! Double */
using Double = double;

/*!
 * @brief JSON number that contains unsigned, signed or double number
 * representation
 * */
class Number {
public:
    friend class Deserializer;

    /*! JSON number type */
    enum class Type {
        INT,        /*!< Signed integer number */
        UINT,       /*!< Unsigned integer number */
        DOUBLE      /*!< Double number */
    };

    /*!
     * @brief Default constructor
     *
     * On default JSON number will be created as signed integer number
     * with initialized to zero
     * */
    Number() : m_type(Type::INT), m_int(0) { }

    /*!
     * @brief Create JSON number as signed integer number
     *
     * @param[in]  value    Value initialization
     * */
    Number(Int value) : m_type(Type::INT), m_int(value) { }

    /*!
     * @brief Create JSON number as unsigned integer number
     *
     * @param[in]  value    Value initialization
     * */
    Number(Uint value) : m_type(Type::UINT), m_uint(value) { }

    /*!
     * @brief Create JSON number as double number
     *
     * @param[in]  value    Value initialization
     * */
    Number(Double value) : m_type(Type::DOUBLE), m_double(value) { }

    /*!
     * @brief Add value
     *
     * This operation does't change number type. Be aware that, when adding
     * double to integer value, fractional part will be lost
     *
     * @param[in]   number  The value that will be added
     *
     * @return Actual JSON number object
     * */
    Number& operator+=(const Number& number);

    /*!
     * @brief Convert JSON number to signed integer value
     * */
    explicit operator Int() const;

    /*!
     * @brief Convert JSON number to unsigned integer value
     * */
    explicit operator Uint() const;

    /*!
     * @brief Convert JSON number to 64-bit signed integer
     * */
    explicit operator Int64() const;

    /*!
     * @brief Convert JSON number to 64-bit unsigned integer
     * */
    explicit operator Uint64() const;

    /*!
     * @brief Convert JSON number to double
     * */
    explicit operator Double() const;

    /*!
     * @brief Check if JSON number is signed integer
     *
     * @return  When success return true, otherwise false
     * */
    bool is_int() const { return Type::INT == m_type; }

    /*!
     * @brief Check if JSON number is unsigned integer
     *
     * @return  When success return true, otherwise false
     * */
    bool is_uint() const { return Type::UINT == m_type; }

    /*!
     * @brief Check if JSON number is double
     *
     * @return  When success return true, otherwise false
     * */
    bool is_double() const { return Type::DOUBLE == m_type; }

    /*!
     * @brief Get JSON number type
     *
     * @return JSON number type
     * */
    Type get_type() const { return m_type; }

    /*!
     * @brief Comparison between two JSON numbers
     *
     * @return When equal return true, otherwise false
     * */
    friend bool operator==(const Number&, const Number&);

    /*!
     * @brief Comparison between two JSON numbers
     *
     * @return When equal return false, otherwise true
     * */
    friend bool operator!=(const Number&, const Number&);

    /*!
     * @brief Less comparison of two JSON numbers
     *
     * @return When first number is less than second number return true,
     *         otherwise false
     * */
    friend bool operator<(const Number&, const Number&);
private:
    enum Type m_type;

    union {
        Int64 m_int;
        Uint64 m_uint;
        Double m_double;
    };
};

bool operator==(const Number&, const Number&);
bool operator<(const Number&, const Number&);
inline bool operator!=(const Number& num1, const Number& num2) {
    return !(num1 == num2);
}

} /* namespace json */

#endif /* JSON_CXX_NUMBER_HPP */
