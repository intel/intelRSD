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
 * @file deserializer.hpp
 *
 * @brief JSON deserializer interface
 * */

#ifndef JSON_CXX_DESERIALIZER_HPP
#define JSON_CXX_DESERIALIZER_HPP

#include "json/value.hpp"

#include <array>

namespace json {

/*!
 * @brief JSON deserialization from string stream contains JSON objects {} or
 * arrays [] to JSON C++ value
 * */
class Deserializer {
public:
    /*!
     * @brief Default constructor
     *
     * No JSON values stored, no parsing started
     * */
    Deserializer();

    /*!
     * @brief JSON deserialization from null-terminated character array
     * to JSON C++ object or array
     *
     * This constructor will start parsing string and store valid JSON
     * object {} or array [] on the stack. To pop from stack use operator>>()
     *
     * @param[in]   str     String contains JSON objects {} or arrays [].
     *                      It may contains whitespaces (spaces, newlines,
     *                      tabulations or carriage returns)
     * */
    Deserializer(const char* str);

    /*!
     * @brief JSON deserialization from String to JSON C++ object or array
     *
     * This constructor will start parsing string and store valid JSON
     * objects {} or arrays [] on the stack. To pop from stack
     * use operator>>()
     *
     * @param[in]   str     String contains JSON objects {} or arrays [].
     *                      It may contains whitespaces (spaces, newlines,
     *                      tabulations or carriage returns)
     * */
    Deserializer(const String& str);

    /*!
     * @brief Copy parsed JSON values on the stack to new Deserializer object
     *
     * The errors won't be copied, all counters and errors will be put in to
     * reset state
     * */
    Deserializer(const Deserializer&);

    /*!
     * @brief Move parsed JSON values on the stack to new Deserializer object
     *
     * The errors won't be copied, all counters and errors will be put in to
     * reset state. The given Deserializer object will contain empty stack and
     * also set all counters and error code to reset state
     * */
    Deserializer(Deserializer&&);

    /*!
     * @brief Start parsing null-terminated character array that contains JSON
     * objects {} or arrays []
     *
     * All parsed and valid JSON values are stored on the stack. To pop from
     * stack use operator>>()
     *
     * @param[in]   str     String contains JSON objects {} or arrays [].
     *                      It may contains whitespaces (spaces, newlines,
     *                      tabulations or carriage returns)
     * */
    Deserializer& operator<<(const char* str);

    /*!
     * @brief Start parsing String that constains JSON objects {} or arrays []
     *
     * All parsed and valid JSON values are stored on the stack.
     * To pop from stack use operator>>()
     *
     * @param[in]   str     String contains JSON objects {} or arrays [].
     *                      It may contains whitespaces (spaces, newlines,
     *                      tabulations or carriage returns)
     * */
    Deserializer& operator<<(const String& str);

    /*!
     * @brief Pop parsed single JSON value from the stack
     *
     * JSON value may be a JSON object or an array. Check the stack size
     * with size() or empty()
     *
     * @param[out]  value   JSON value to store from the stack
     * */
    Deserializer& operator>>(Value& value);

    /*!
     * @brief Replaces the contents of the stack with new entries from given
     * Deserializer object
     *
     * The errors won't be copied, all counters and errors will be put in to
     * reset state
     * */
    Deserializer& operator=(const Deserializer&);

    /*!
     * @brief Replaces the contents of the stack with new entries from given
     * Deserializer object using moving semantics
     *
     * The errors won't be copied, all counters and errors will be put in to
     * reset state. The given Deserializer object will contain empty stack and
     * also all counters and error code to reset state
     * */
    Deserializer& operator=(Deserializer&&);

    /*!
     * @brief Parsing given null-terminated character array and store all parsed
     * JSON objects {} or arrays [] on the stack
     *
     * Because this use stack to store all parsed JSON objects, only last
     * parsed data will be pop from stack and stored to given value output.
     * operator>>() behave like input stream and may be used in the chain:
     *
     * @code
     * R"({"key1":1, "key2":2}[1, 2])" >> value2 >> value1;
     * @endcode
     *
     * @param[in]   str     String contains JSON objects {} or arrays [].
     *                      It may contains whitespaces (spaces, newlines,
     *                      tabulations or carriage returns)
     *
     * @param[out]  value   JSON value to store from the stack
     * */
    friend Deserializer operator>>(const char* str, Value& value);

    /*!
     * @brief Parsing given String object and store all parsed JSON objects {}
     * or arrays [] on the stack
     *
     * Because this use stack to store all parsed JSON objects, only last
     * parsed data will be pop from stack and stored to given value output.
     * operator>>() behave like input stream and may be used in the chain:
     *
     * @code
     * R"({"key1":1, "key2":2}[1, 2])" >> value2 >> value1;
     * @endcode
     *
     * @param[in]   str     String contains JSON objects {} or arrays [].
     *                      It may contains whitespaces (spaces, newlines,
     *                      tabulations or carriage returns)
     *
     * @param[out]  value   JSON value to store from the stack
     * */
    friend Deserializer operator>>(const String& str, Value& value);

    /*!
     * @brief Set maximum characters to parse per JSON object or array
     *
     * This limitation protect application stack from reach out of the
     * memory or from stack attack
     *
     * @param[in]   limit   Set maximum characters to parse per one valid
     *                      JSON object or array. Invoking method without
     *                      any argument sets default limitation
     * */
    void set_limit(size_t limit = MAX_LIMIT_PER_OBJECT);

    /*!
     * @brief Check if stack that contains parsed JSON values is empty
     *
     * This method is equivalent to size() == 0
     *
     * @return  When empty return true otherwise return false
     * */
    bool empty() const;

    /*!
     * @brief Get how many JSON values are stored on the stack
     *
     * @return Number of JSON values stored on the stack
     * */
    size_t size() const;

    /*! JSON error parsing */
    struct Error {
        /*! Error parsing codes */
        enum class Code {
            NONE,
            END_OF_FILE,
            MISS_VALUE,
            MISS_QUOTE,
            MISS_COLON,
            MISS_CURLY_CLOSE,
            MISS_SQUARE_CLOSE,
            NOT_MATCH_NULL,
            NOT_MATCH_TRUE,
            NOT_MATCH_FALSE,
            INVALID_WHITESPACE,
            INVALID_ESCAPE,
            INVALID_UNICODE,
            INVALID_NUMBER_INTEGER,
            INVALID_NUMBER_FRACTION,
            INVALID_NUMBER_EXPONENT
        };

        /*! Error parsing code */
        Code code;
        /*! Line position indicative error */
        size_t line;
        /*! Column number indicative error */
        size_t column;
        /*! Parsed stream size in bytes/characters */
        size_t size;
        /*! Offset in the parsed stream that indicative error */
        size_t offset;

        /*!
         * @brief Try to decode error code
         *
         * @return  When success return decoded error code as a human readable
         *          message, otherwise return empty string ""
         * */
        const char* decode();
    };

    /*!
     * @brief Check if Deserializer accour any errors while parsing stream
     *
     * @return  true when invalid, otherwise false
     * */
    bool is_invalid() const;

    /*!
     * @brief Get error information
     *
     * @return  Error information
     * */
    Error get_error() const;
private:
    /*! Stack protection */
    static const size_t MAX_LIMIT_PER_OBJECT;

    Array m_array;

    const char* m_begin;
    const char* m_current;
    const char* m_end;
    size_t m_limit;
    Error::Code m_error_code;

    bool read_object(Value& value);
    bool read_object_member(Value& value, size_t& count);
    bool read_string(String& str);
    bool read_string_unicode(String& str);
    bool read_string_escape(String& str);
    bool read_value(Value& value);
    bool read_array(Value& value);
    bool read_array_element(Value& value, size_t& count);
    bool read_colon();
    bool read_quote();
    bool read_true(Value& value);
    bool read_false(Value& value);
    bool read_null(Value& value);
    bool read_number(Value& value);
    bool read_number_digit(Uint64& str);
    bool read_number_integer(Number& number);
    bool read_number_fractional(Number& number);
    bool read_number_exponent(Number& number);
    bool read_unicode(const char** pos, uint32_t& code);
    bool read_whitespaces();

    bool count_string_chars(size_t& count);

    void clear_error();
    bool set_error(Error::Code error_code);
};

Deserializer operator>>(const char* str, Value& value);
Deserializer operator>>(const String& str, Value& value);

}

#endif /* JSON_CXX_DESERIALIZER_HPP */
