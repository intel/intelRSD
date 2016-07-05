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
 * @file deserializer.cpp
 *
 * @brief JSON deserializer implementation
 * */

#include "json/deserializer.hpp"

#include <safe-string/safe_lib.hpp>
#include <cmath>
#include <cstring>

using namespace json;

/*! Error parsing code */
using Code = Deserializer::Error::Code;

/*! UTF-16 surrogate pair */
using Surrogate = std::pair<unsigned, unsigned>;

/*! Maximu characters to parse per single JSON value. Stack protection */
const size_t Deserializer::MAX_LIMIT_PER_OBJECT = 0xFFFFFFFF;

static constexpr char JSON_NULL[] = "null";
static constexpr char JSON_TRUE[] = "true";
static constexpr char JSON_FALSE[] = "false";
static constexpr size_t UNICODE_LENGTH = 4;

/*!
 * @brief   Get string length without null termination '\0'
 * @return  String length
 * */
template<size_t N>
constexpr size_t length(const char (&)[N]) { return (N - 1); }

/*!
 * @brief   Get array size based on the type of given object
 * @return  Number of elements in the array
 * */
template<class T, size_t N>
constexpr size_t array_size(T (&)[N]) { return N; }

Deserializer::Deserializer() :
    m_array{},
    m_begin(nullptr),
    m_current(nullptr),
    m_end(nullptr),
    m_limit(MAX_LIMIT_PER_OBJECT),
    m_error_code(Code::NONE) { }

Deserializer::Deserializer(const Deserializer& deserializer) : Deserializer() {
    m_array = deserializer.m_array;
}

Deserializer::Deserializer(Deserializer&& deserializer) : Deserializer() {
    m_array = std::move(deserializer.m_array);
    deserializer.m_array.clear();

    deserializer.m_begin = nullptr;
    deserializer.m_current = nullptr;
    deserializer.m_end = nullptr;
    deserializer.m_error_code = Code::NONE;
}

Deserializer::Deserializer(const char* str) : Deserializer() {
    (*this) << str;
}

Deserializer::Deserializer(const String& str) : Deserializer() {
    (*this) << str;
}

Deserializer& Deserializer::operator=(const Deserializer& deserializer) {
    if (this == &deserializer) { return *this; }

    m_array = deserializer.m_array;

    m_begin = nullptr;
    m_current = nullptr;
    m_end = nullptr;
    m_error_code = Code::NONE;

    return *this;
}

Deserializer& Deserializer::operator=(Deserializer&& deserializer) {
    if (this == &deserializer) { return *this; }

    m_array = std::move(deserializer.m_array);
    deserializer.m_array.clear();

    m_begin = nullptr;
    m_current = nullptr;
    m_end = nullptr;
    m_error_code = Code::NONE;

    deserializer.m_begin = nullptr;
    deserializer.m_current = nullptr;
    deserializer.m_end = nullptr;
    deserializer.m_error_code = Code::NONE;

    return *this;
}

Deserializer& Deserializer::operator<<(const char* str) {
    clear_error();

    size_t str_size = 0;

    if (nullptr != str) {
        str_size = strnlen_s(str, RSIZE_MAX_STR);
    }

    m_begin = str;
    m_current = m_begin;
    m_end = str + str_size;

    Value value;
    if (read_value(value)) {
        if (!read_whitespaces()) {
            clear_error();
            m_array.push_back(std::move(value));
        } else {
            set_error(Code::INVALID_WHITESPACE);
        }
    }

    return *this;
}

Deserializer& Deserializer::operator<<(const String& str) {
    clear_error();

    m_begin = str.cbegin().base();
    m_current = m_begin;
    m_end = str.cend().base();

    Value value;
    if (read_value(value)) {
        if (!read_whitespaces()) {
            clear_error();
            m_array.push_back(std::move(value));
        } else {
            set_error(Code::INVALID_WHITESPACE);
        }
    }

    return *this;
}

Deserializer& Deserializer::operator>>(Value& value) {
    if (m_array.empty()) {
        value = Value::Type::NIL;
    }
    else {
        value = std::move(m_array.back());
        m_array.pop_back();
    }

    return *this;
}

Deserializer json::operator>>(const char* str, Value& val) {
    return Deserializer(str) >> val;
}

Deserializer json::operator>>(const String& str, Value& val) {
    return Deserializer(str) >> val;
}

bool Deserializer::empty() const {
    return m_array.empty();
}

size_t Deserializer::size() const {
    return m_array.size();
}

void Deserializer::set_limit(size_t limit) {
    m_limit = limit;
}

inline void Deserializer::clear_error() {
    m_error_code = Code::NONE;
}

Deserializer::Error Deserializer::get_error() const {
    Error error;

    error.code = m_error_code;
    error.line = 1;
    error.column = 1;
    error.offset = size_t(m_current - m_begin);
    error.size = size_t(m_end - m_begin);

    const char* search = m_begin;

    while (search < m_current) {
        if ('\n' == *search) {
            ++error.line;
            error.column = 1;
        }
        else {
            ++error.column;
        }
        ++search;
    }

    return error;
}

bool Deserializer::is_invalid() const {
    return Code::NONE != m_error_code;
}

bool Deserializer::read_object(Value& value) {
    if (!read_whitespaces()) { return false; }

    value.m_type = Value::Type::OBJECT;
    new (&value.m_object) Object();

    if ('}' == *m_current) {
        ++m_current;
        return true;
    }

    size_t count = 0;

    return read_object_member(value, count);
}

bool Deserializer::read_object_member(Value& value, size_t& count) {
    String key;
    Value tmp;

    if (!read_quote()) { return false; }
    if (!read_string(key)) { return false; }
    if (!read_colon()) { return false; }
    if (!read_value(tmp)) { return false; }
    if (!read_whitespaces()) { return false; }

    ++count;

    if (',' == *m_current) {
        ++m_current;
        if (!read_object_member(value, count)) { return false; }
        value.m_object[--count].first = std::move(key);
        value.m_object[count].second = std::move(tmp);
    }
    else if ('}' == *m_current) {
        ++m_current;
        value.m_object.resize(count);
        value.m_object[--count].first = std::move(key);
        value.m_object[count].second = std::move(tmp);

    }
    else {
        return set_error(Code::MISS_CURLY_CLOSE);
    }

    return true;
}

inline bool Deserializer::read_string(String& str) {
    size_t capacity = 1;
    char ch;

    if (!count_string_chars(capacity)) {
        return set_error(Code::END_OF_FILE);
    }

    str.reserve(capacity);

    while (m_current < m_end) {
        ch = *(m_current++);
        switch (ch) {
        case '"':
            return true;
        case '\\':
            if (!read_string_escape(str)) { return false; };
            break;
        default:
            str.push_back(ch);
            break;
        }
    }

    return set_error(Code::END_OF_FILE);
}

bool Deserializer::read_string_escape(String& str) {
    char ch = *m_current;

    switch (ch) {
    case '"':
    case '\\':
    case '/':
        break;
    case 'n':
        ch = '\n';
        break;
    case 'r':
        ch = '\r';
        break;
    case 't':
        ch = '\t';
        break;
    case 'b':
        ch = '\b';
        break;
    case 'f':
        ch = '\f';
        break;
    case 'u':
        ++m_current;
        return read_string_unicode(str);
    default:
        return set_error(Code::INVALID_ESCAPE);
    }

    ++m_current;
    str.push_back(ch);

    return true;
}

static inline
uint32_t decode_utf16_surrogate_pair(const Surrogate& surrogate) {
    return 0x10000
        | ((0x03F & surrogate.first) << 10)
        |  (0x3FF & surrogate.second);
}

bool Deserializer::read_string_unicode(String& str) {
    static const Surrogate SURROGATE_MIN(0xD800, 0xDC00);
    static const Surrogate SURROGATE_MAX(0xDBFF, 0xDFFF);

    Surrogate surrogate;
    uint32_t code;

    if (!read_unicode(&m_current, code)) { return false; }

    if (m_current + 2 + UNICODE_LENGTH < m_end) {
        if (('\\' == m_current[0]) && ('u' == m_current[1])) {
            m_current += 2;
            if (read_unicode(&m_current, surrogate.second)) {
                surrogate.first = code;
                if ((SURROGATE_MIN <= surrogate)
                 && (SURROGATE_MAX >= surrogate)) {
                    code = decode_utf16_surrogate_pair(surrogate);
                }
                else { m_current -= (2 + UNICODE_LENGTH); }
            } else { return false; }
        }
    }

    if (code < 0x80) {
        str.push_back(char(code));
    }
    else if (code < 0x800) {
        str.push_back(char(0xC0 | (0x1F & (code >>  6))));
        str.push_back(char(0x80 | (0x3F & (code >>  0))));
    }
    else if (code < 0x10000) {
        str.push_back(char(0xE0 | (0x0F & (code >> 12))));
        str.push_back(char(0x80 | (0x3F & (code >>  6))));
        str.push_back(char(0x80 | (0x3F & (code >>  0))));
    }
    else {
        str.push_back(char(0xF0 | (0x07 & (code >> 18))));
        str.push_back(char(0x80 | (0x3F & (code >> 12))));
        str.push_back(char(0x80 | (0x3F & (code >>  6))));
        str.push_back(char(0x80 | (0x3F & (code >>  0))));
    }

    return true;
}

inline bool Deserializer::read_unicode(const char** pos, uint32_t& code) {
    if (*pos + UNICODE_LENGTH >= m_end) { return set_error(Code::END_OF_FILE); }

    static constexpr uint32_t HEX_0_9 = '0' - 0x0;
    static constexpr uint32_t HEX_A_F = 'A' - 0xA;
    static constexpr uint32_t HEX_a_f = 'a' - 0xA;

    uint32_t ch;
    size_t count = UNICODE_LENGTH;
    code = 0;

    while (count--) {
        ch = uint32_t(**pos);
        code <<= 4;
        if (('0' <= ch) && (ch <= '9')) {
            code |= (ch - HEX_0_9);
        }
        else if (('A' <= ch) && (ch <= 'F')) {
            code |= (ch - HEX_A_F);
        }
        else if (('a' <= ch) && (ch <= 'f')) {
            code |= (ch - HEX_a_f);
        }
        else {
            return set_error(Code::INVALID_UNICODE);
        }
        ++(*pos);
    }

    return true;
}

inline bool Deserializer::count_string_chars(size_t& count) {
    static const Surrogate SURROGATE_MIN(0xD800, 0xDC00);
    static const Surrogate SURROGATE_MAX(0xDBFF, 0xDFFF);

    const char* pos = m_current;

    while (pos < m_end) {
        switch (*pos) {
        case '"':
            return true;
        case '\\':
            if ('u' == *(++pos)) {
                uint32_t code;
                if (!read_unicode(&(++pos), code)) { return false; }
                if (code < 0x80) { ++count; }
                else if (code < 0x800) { count += 2; }
                else {
                    if ((SURROGATE_MIN.first > code)
                     || (SURROGATE_MAX.first < code)) { count += 3; }
                    else { count += 4; }
                }
            } else {
                ++count;
                ++pos;
            }
            break;
        default:
            ++count;
            ++pos;
            break;
        }
    }

    return set_error(Code::END_OF_FILE);
}

bool Deserializer::read_value(Value& value) {
    bool ok = false;
    String str;

    if (!read_whitespaces()) { return false; }

    switch (*m_current) {
    case '"':
        ++m_current;
        ok = read_string(str);
        value.m_type = Value::Type::STRING;
        new (&value.m_string) String(std::move(str));
        break;
    case '{':
        ++m_current;
        ok = read_object(value);
        break;
    case '[':
        ++m_current;
        ok = read_array(value);
        break;
    case 't':
        ok = read_true(value);
        break;
    case 'f':
        ok = read_false(value);
        break;
    case 'n':
        ok = read_null(value);
        break;
    case '-':
        ok = read_number(value);
        break;
    default:
        if (std::isdigit(*m_current)) {
            ok = read_number(value);
        } else { set_error(Code::MISS_VALUE); }
        break;
    }

    return ok;
}

bool Deserializer::read_array(Value& value) {
    if (!read_whitespaces()) { return false; }

    value.m_type = Value::Type::ARRAY;
    new (&value.m_array) Array();

    if (']' == *m_current) {
        ++m_current;
        return true;
    }

    size_t count = 0;

    return read_array_element(value, count);
}

bool Deserializer::read_array_element(Value& value, size_t& count)  {
    Value tmp;

    if (!read_value(tmp)) { return false; }
    if (!read_whitespaces()) { return false; }
    ++count;

    if (',' == *m_current) {
        ++m_current;
        if (!read_array_element(value, count)) { return false; }
        value.m_array[--count] = std::move(tmp);
    }
    else if (']' == *m_current) {
        ++m_current;
        value.m_array.resize(count);
        value.m_array[--count] = std::move(tmp);
    }
    else {
        return set_error(Code::MISS_SQUARE_CLOSE);
    }

    return true;
}

inline bool Deserializer::read_colon() {
    if (!read_whitespaces()) { return false; }
    if (':' != *m_current) {
        return set_error(Code::MISS_COLON);
    }
    ++m_current;
    return true;
}

inline bool Deserializer::read_quote() {
    if (!read_whitespaces()) { return false; }
    if ('"' != *m_current) {
        return set_error(Code::MISS_QUOTE);
    }
    ++m_current;
    return true;
}

bool Deserializer::read_whitespaces() {
    unsigned ch;

    while (m_current < m_end) {
        ch = unsigned(*m_current);
        if ((' '  == ch) || ('\n' == ch) || ('\r' == ch) || ('\t' == ch)) {
            ++m_current;
        } else { return true; }
    }

    return set_error(Code::END_OF_FILE);
}

bool Deserializer::read_number_digit(Uint64& value) {
    using std::isdigit;

    if ((m_current < m_end) && isdigit(*m_current)) {
        value = Uint64(*m_current - '0');
        ++m_current;
    } else { return false; }

    while (m_current < m_end) {
        if (isdigit(*m_current)) {
            value = (10 * value) + Uint64(*m_current - '0');
            ++m_current;
        } else { return true; }
    }

    return true;
}

inline bool Deserializer::read_number_integer(Number& number) {
    using std::isdigit;

    Uint64 value;

    if (!read_number_digit(value)) { return false; }

    if (Number::Type::UINT == number.m_type) {
        number.m_uint = value;
    } else {
        number.m_int = Int64(-value);
    }

    return true;
}

inline bool Deserializer::read_number_fractional(Number& number) {
    using std::isdigit;

    Double step = 0.1;
    Double fractional = 0;

    while (m_current < m_end) {
        if (isdigit(*m_current)) {
            fractional += (step * (*m_current - '0'));
            step = 0.1 * step;
            ++m_current;
        } else {
            if (Number::Type::UINT == number.m_type) {
                Double tmp = Double(number.m_uint);
                number.m_double = tmp + fractional;
            } else {
                Double tmp = Double(number.m_int);
                number.m_double = tmp - fractional;
            }
            number.m_type = Number::Type::DOUBLE;
            return true;
        }
    }

    return set_error(Code::END_OF_FILE);
}

inline bool Deserializer::read_number_exponent(Number& number) {
    using std::pow;

    bool is_negative = false;
    Uint64 value;

    if ('+' == *m_current) {
        ++m_current;
    } else if ('-' == *m_current) {
        is_negative = true;
        ++m_current;
    }

    if (!read_number_digit(value)) { return false; }

    switch (number.m_type) {
    case Number::Type::INT:
        if (is_negative) {
            number.m_double *= pow(0.1, value);
            number.m_type = Number::Type::DOUBLE;
        } else {
            number.m_int *= Int64(pow(10, value));
        }
        break;
    case Number::Type::UINT:
        if (is_negative) {
            number.m_double *= pow(0.1, value);
            number.m_type = Number::Type::DOUBLE;
        } else {
            number.m_uint *= Uint64(pow(10, value));
        }
        break;
    case Number::Type::DOUBLE:
        if (is_negative) {
            number.m_double *= pow(0.1, value);
        }
        else {
            number.m_double *= pow(10, value);
        }
        break;
    default:
        break;
    }

    return true;
}

bool Deserializer::read_number(Value& value) {
    using std::isdigit;

    /* Prepare JSON number */
    value.m_type = Value::Type::NUMBER;
    new (&value.m_number) Number();

    if ('-' == *m_current) {
        value.m_number.m_type = Number::Type::INT;
        value.m_number.m_int = 0;
        ++m_current;
    } else {
        value.m_number.m_type = Number::Type::UINT;
        value.m_number.m_uint = 0;
    }

    if ('0' == *m_current) {
        ++m_current;
    } else if (!read_number_integer(value.m_number)) {
        return set_error(Code::INVALID_NUMBER_INTEGER);
    }

    if ('.' == *m_current) {
        ++m_current;
        if (!read_number_fractional(value.m_number)) {
            return set_error(Code::INVALID_NUMBER_FRACTION);
        }
    }

    if (('E' == *m_current) || ('e' == *m_current)) {
        ++m_current;
        if (!read_number_exponent(value.m_number)) {
            return set_error(Code::INVALID_NUMBER_EXPONENT);
        }
    }

    return true;
}

inline bool Deserializer::read_true(Value& value) {
    if (m_current + length(JSON_TRUE) > m_end) {
        return set_error(Code::END_OF_FILE);
    }

    if (0 != std::strncmp(m_current, JSON_TRUE, length(JSON_TRUE))) {
        return set_error(Code::NOT_MATCH_TRUE);
    }

    value.m_type = Value::Type::BOOLEAN;
    value.m_boolean = true;

    m_current += length(JSON_TRUE);
    return true;
}

inline bool Deserializer::read_false(Value& value) {
    if (m_current + length(JSON_FALSE) > m_end) {
        return set_error(Code::END_OF_FILE);
    }

    if (0 != std::strncmp(m_current, JSON_FALSE, length(JSON_FALSE))) {
        return set_error(Code::NOT_MATCH_FALSE);
    }

    value.m_type = Value::Type::BOOLEAN;
    value.m_boolean = false;

    m_current += length(JSON_FALSE);
    return true;
}

inline bool Deserializer::read_null(Value& value) {
    if (m_current + length(JSON_NULL) > m_end) {
        return set_error(Code::END_OF_FILE);
    }

    if (0 != std::strncmp(m_current, JSON_NULL, length(JSON_NULL))) {
        return set_error(Code::NOT_MATCH_NULL);
    }

    value.m_type = Value::Type::NIL;

    m_current += length(JSON_NULL);
    return true;
}

inline bool Deserializer::set_error(Error::Code error_code) {
    if (Code::NONE == m_error_code) {
        m_error_code = error_code;
    }
    return false;
}

/*! Internal error codes message */
struct ErrorCodes {
    Deserializer::Error::Code code; /*!< Error parsing code */
    const char* message;            /*!< Error parsing message */
};

static const struct ErrorCodes g_error_codes[] = {
    { Code::NONE,               "No error"},
    { Code::END_OF_FILE,        "End of file reached"},
    { Code::MISS_QUOTE,         "Missing quote '\"' for string"},
    { Code::MISS_COLON,         "Missing colon ':' in member pair"},
    { Code::MISS_CURLY_CLOSE,   "Missing comma ','"
        " or closing curly '}' for object"},
    { Code::MISS_SQUARE_CLOSE,  "Missing comma ','"
        " or closing square ']' for array"},
    { Code::NOT_MATCH_NULL,     "Did you mean 'null'?"},
    { Code::NOT_MATCH_TRUE,     "Did you mean 'true'?"},
    { Code::NOT_MATCH_FALSE,    "Did you mean 'false'?"},
    { Code::MISS_VALUE,         "Missing value in array/member"},
    { Code::INVALID_WHITESPACE, "Invalid whitespace character"},
    { Code::INVALID_ESCAPE,     "Invalid escape character"},
    { Code::INVALID_UNICODE,    "Invalid unicode"},
    { Code::INVALID_NUMBER_INTEGER, "Invalid number integer part"},
    { Code::INVALID_NUMBER_FRACTION,"Invalid number fractional part"},
    { Code::INVALID_NUMBER_EXPONENT,"Invalid number exponent part"}
};

const char* Deserializer::Error::decode() {
    for (const auto& error : g_error_codes) {
        if (error.code == code) {
            return error.message;
        }
    }
    return "Unknown error";
}
