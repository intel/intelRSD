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
 * @file value.cpp
 *
 * @brief JSON value implementation
 * */

#include "json/value.hpp"
#include "json/iterator.hpp"

#include <limits>
#include <type_traits>
#include <functional>

using namespace json;

/*!
 * @brief Raw aligned template memory
 * */
template<typename T>
using raw_memory = typename std::aligned_storage<sizeof(T),
      std::alignment_of<T>::value>::type;

/*! Create raw aligned memory for Value object and initialize with zeros */
static const raw_memory<Value> g_null_value_raw {};

/*! Omit dereferencing type-punned pointer */
static const void* g_null_value_ref = &g_null_value_raw;

/*! Now we can cast raw memory to JSON value object */
static const Value& g_null_value = *static_cast<const Value*>(g_null_value_ref);

Value::Exception::Exception(const char* str) : runtime_error(str) { }

Value::Exception::Exception(const std::string& str) : runtime_error(str) { }

Value::Exception::~Exception() { }

Value::Value(Type type) : m_type(type) {
    create_container(type);
}

Value::Value(Null) : m_type(Type::NIL) { }

Value::Value(Bool boolean) : m_type(Type::BOOLEAN) {
    m_boolean = boolean;
}

Value::Value(const char* str) : m_type(Type::STRING) {
    new (&m_string) String(str);
}

Value::Value(const String& str) : m_type(Type::STRING) {
    new (&m_string) String(str);
}

Value::Value(const Pair& pair) : m_type(Type::OBJECT) {
    new (&m_object) Object{pair};
}

Value::Value(const char* key, const Value& value) : m_type(Type::OBJECT) {
    new (&m_object) Object{std::make_pair(key, value)};
}

Value::Value(const String& key, const Value& value) : m_type(Type::OBJECT) {
    new (&m_object) Object{std::make_pair(key, value)};
}

Value::Value(Uint value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(Int value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(Uint64 value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(Int64 value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(Double value) : m_type(Type::NUMBER) {
    new (&m_number) Number(value);
}

Value::Value(const Number& number) : m_type(Type::NUMBER) {
    new (&m_number) Number(number);
}

Value::Value(size_t count, const Value& value) : m_type(Type::ARRAY) {
    new (&m_array) Array(count, value);
}

Value::Value(std::initializer_list<Pair> init_list) : m_type(Type::OBJECT) {
    new (&m_object) Object();

    for (auto it = init_list.begin(); it < init_list.end(); ++it) {
        (*this)[it->first] = it->second;
    }
}

Value::Value(std::initializer_list<Value> init_list) : m_type(Type::ARRAY) {
    new (&m_array) Array();
    m_array = init_list;
}

Value::~Value() {
    switch (m_type) {
    case Type::OBJECT:
        m_object.~vector();
        break;
    case Type::ARRAY:
        m_array.~vector();
        break;
    case Type::STRING:
        m_string.~basic_string();
        break;
    case Type::NUMBER:
        m_number.~Number();
        break;
    case Type::BOOLEAN:
    case Type::NIL:
    default:
        break;
    }
}

void Value::create_container(Type type) {
    m_type = type;
    switch (type) {
    case Type::OBJECT:
        new (&m_object) Object();
        break;
    case Type::ARRAY:
        new (&m_array) Array();
        break;
    case Type::STRING:
        new (&m_string) String();
        break;
    case Type::NUMBER:
        new (&m_number) Number();
        break;
    case Type::BOOLEAN:
        m_boolean = false;
        break;
    case Type::NIL:
    default:
        break;
    }
}

Value::Value(const Value& value) : m_type(value.m_type) {
    create_container(m_type);
    operator=(value);
}

Value::Value(Value&& value) : m_type(value.m_type) {
    create_container(m_type);
    operator=(std::move(value));
}

Value& Value::operator=(const Value& value) {
    if (&value == this) { return *this; }

    if (value.m_type != m_type) {
        this->~Value();
        create_container(value.m_type);
    }

    switch (m_type) {
    case Type::OBJECT:
        m_object = value.m_object;
        break;
    case Type::ARRAY:
        m_array = value.m_array;
        break;
    case Type::STRING:
        m_string = value.m_string;
        break;
    case Type::NUMBER:
        m_number = value.m_number;
        break;
    case Type::BOOLEAN:
        m_boolean = value.m_boolean;
        break;
    case Type::NIL:
    default:
        break;
    }

    return *this;
}

Value& Value::operator=(Value&& value) {
    if (&value == this) { return *this; }

    if (value.m_type != m_type) {
        this->~Value();
        create_container(value.m_type);
    }

    switch (m_type) {
    case Type::OBJECT:
        m_object = std::move(value.m_object);
        break;
    case Type::ARRAY:
        m_array = std::move(value.m_array);
        break;
    case Type::STRING:
        m_string = std::move(value.m_string);
        break;
    case Type::NUMBER:
        m_number = std::move(value.m_number);
        break;
    case Type::BOOLEAN:
        m_boolean = std::move(value.m_boolean);
        break;
    case Type::NIL:
    default:
        break;
    }

    return *this;
}

Value& Value::operator=(std::initializer_list<Pair> init_list) {
    this->~Value();
    create_container(Type::OBJECT);

    for (auto it = init_list.begin(); it < init_list.end(); ++it) {
        (*this)[it->first] = it->second;
    }

    return *this;
}

Value& Value::operator=(std::initializer_list<Value> init_list) {
    this->~Value();
    create_container(Type::ARRAY);

    m_array = init_list;

    return *this;
}

Value& Value::operator+=(const Value& value) {
    switch (m_type) {
    case Type::OBJECT:
        if (value.is_object()) {
            for (auto it = value.cbegin(); value.cend() != it; ++it) {
                (*this)[it.key()] = *it;
            }
        }
        break;
    case Type::ARRAY:
        if (value.is_array()) {
            m_array.insert(m_array.end(),
                    value.m_array.begin(),
                    value.m_array.end());
        }
        else if (value.is_object()) {
             m_array.insert(m_array.end(),
                    value.m_object.begin(),
                    value.m_object.end());
        }
        else {
            m_array.push_back(value);
        }
        break;
    case Type::STRING:
        if (value.is_string()) {
            m_string += value.m_string;
        }
        break;
    case Type::NUMBER:
        if (value.is_number()) {
            m_number += value.m_number;
        }
        break;
    case Type::NIL:
        *this = value;
        break;
    case Type::BOOLEAN:
    default:
        break;
    }

    return *this;
}

void Value::assign(std::initializer_list<Pair> init_list) {
    *this = init_list;
}

void Value::assign(std::initializer_list<Value> init_list) {
    *this = init_list;
}

void Value::assign(size_t count, const Value& value) {
    this->~Value();
    create_container(Type::ARRAY);
    new (&m_array) Array(count, value);
}

size_t Value::size() const {
    size_t value = 0;

    switch (m_type) {
    case Type::OBJECT:
        value = m_object.size();
        break;
    case Type::ARRAY:
        value = m_array.size();
        break;
    case Type::STRING:
    case Type::NIL:
    case Type::NUMBER:
    case Type::BOOLEAN:
    default:
        break;
    }

    return value;
}

void Value::clear() {
    switch (m_type) {
    case Type::OBJECT:
        m_object.clear();
        break;
    case Type::ARRAY:
        m_array.clear();
        break;
    case Type::STRING:
        m_string.clear();
        break;
    case Type::NUMBER:
        m_number = 0;
        break;
    case Type::BOOLEAN:
        m_boolean = false;
        break;
    case Type::NIL:
    default:
        break;
    }
}

Value& Value::operator[](int index) {
    return (*this)[size_t(index)];
}

const Value& Value::operator[](int index) const {
    return (*this)[size_t(index)];
}

Value& Value::operator[](const String& key) {
    return (*this)[key.c_str()];
}

const Value& Value::operator[](const String& key) const {
    return (*this)[key.c_str()];
}

bool Value::is_member(const std::string& key) const {
    return is_member(key.c_str());
}

bool Value::is_member(const char* key) const {
    if (!is_object()) { return false; }
    for (const auto& pair : m_object) {
        if (key == pair.first) {
            return true;
        }
    }
    return false;
}

size_t Value::erase(const char* key) {
    if (!is_object()) { return 0; }

    for (auto it = m_object.begin(); it != m_object.end(); it++) {
        if (key == it->first) {
            m_object.erase(it);
            return 1;
        }
    }

    return 0;
}

size_t Value::erase(const String& key) {
    return erase(key.c_str());
}

Value::iterator Value::erase(const_iterator pos) {
    iterator tmp;

    if (is_array() && pos.is_array()) {
        tmp = std::move(m_array.erase(pos.m_array_iterator));
    }
    else if (is_object() && pos.is_object()) {
        tmp = std::move(m_object.erase(pos.m_object_iterator));
    }
    else {
        tmp =end();
    }

    return tmp;
}

Value::iterator Value::erase(const_iterator first, const_iterator last) {
    iterator tmp;

    for (auto it = first; it < last; ++it) {
        tmp = erase(it);
    }

    return tmp;
}

Value::iterator Value::insert(const_iterator pos, const Value& value) {
    iterator tmp;

    if (is_array() && pos.is_array()) {
        tmp = std::move(m_array.insert(pos.m_array_iterator, value));
    }
    else if (is_object() && pos.is_object() && value.is_object()) {
        for (auto it = value.cbegin(); value.cend() != it; ++it, ++pos) {
            if (!is_member(it.key())) {
                tmp = std::move(m_object.insert(pos.m_object_iterator,
                            Pair(it.key(), *it)));
            }
        }
    }
    else {
        tmp = end();
    }

    return tmp;
}

Value::iterator Value::insert(const_iterator pos, Value&& value) {
    iterator tmp;

    if (is_array() && pos.is_array()) {
        tmp = std::move(m_array.insert(pos.m_array_iterator, std::move(value)));
    }
    else if (is_object() && pos.is_object() && value.is_object()) {
        for (auto it = value.cbegin(); value.cend() != it; ++it, ++pos) {
            if (!is_member(it.key())) {
                tmp = std::move(m_object.insert(pos.m_object_iterator,
                            Pair(it.key(), std::move(*it))));
            }
        }
    }
    else {
        tmp = end();
    }

    return tmp;
}

Value::iterator Value::insert(const_iterator pos,
        size_t count, const Value& value) {
    iterator tmp;

    while (0 < count--) {
        tmp = insert(pos++, value);
    }

    return tmp;
}

Value::iterator Value::insert(const_iterator pos,
        const_iterator first, const_iterator last) {
    iterator tmp;

    for (auto it = first; it < last; ++it) {
        tmp = insert(pos++, *it);
    }

    return tmp;
}

Value::iterator Value::insert(const_iterator pos,
        std::initializer_list<Value> init_list) {
    return insert(pos, init_list.begin(), init_list.end());
}

String& Value::as_string() {
    if (Type::STRING != m_type) {
        throw Value::Exception("JSON isn't a string");
    }
    return m_string;
}

const String& Value::as_string() const {
    if (Type::STRING != m_type) {
        throw Value::Exception("JSON isn't a string");
    }
    return m_string;
}

const char* Value::as_char() const {
    if (Type::STRING != m_type) {
        throw Value::Exception("JSON isn't a string");
    }
    return m_string.c_str();
}

Bool Value::as_bool() const {
    if (Type::BOOLEAN != m_type) {
        throw Value::Exception("JSON isn't a boolean");
    }
    return m_boolean;
}

Null Value::as_null() const {
    if (Type::NIL != m_type) {
        throw Value::Exception("JSON isn't a null");
    }
    return nullptr;
}

Int Value::as_int() const {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return Int(m_number);
}

Uint Value::as_uint() const {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return Uint(m_number);
}

Int64 Value::as_int64() const {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return Int64(m_number);
}

Uint64 Value::as_uint64() const {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return Uint64(m_number);
}

Double Value::as_double() const {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return Double(m_number);
}

Array& Value::as_array() {
    if (Type::ARRAY != m_type) {
        throw Value::Exception("JSON isn't an array");
    }
    return m_array;
}

Number& Value::as_number() {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return m_number;
}

const Array& Value::as_array() const {
    if (Type::ARRAY != m_type) {
        throw Value::Exception("JSON isn't an array");
    }
    return m_array;
}

const Object& Value::as_object() const {
    if (Type::OBJECT != m_type) {
        throw Value::Exception("JSON isn't an object");
    }
    return m_object;
}

const Number& Value::as_number() const {
    if (Type::NUMBER != m_type) {
        throw Value::Exception("JSON isn't a number");
    }
    return m_number;
}

Value& Value::operator[](const char* key) {
    if (!is_object()) {
        if (is_null()) { *this = Type::OBJECT; }
        else { return *this; }
    }

    for (auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    m_object.emplace_back(key, Value());

    return m_object.back().second;
}

const Value& Value::operator[](const char* key) const {
    if (!is_object()) { return *this; }

    for (const auto& pair : m_object) {
        if (key == pair.first) {
            return pair.second;
        }
    }

    return g_null_value;
}

Value& Value::operator[](const size_t index) {
    if (is_null()) { *this = Type::ARRAY; }

    Value* ptr;

    if (is_array()) {
        if (size() == index) {
            m_array.emplace_back(Value());
        }
        ptr = &m_array[index];
    }
    else  if (is_object()) {
        ptr = &m_object[index].second;
    }
    else {
        ptr = this;
    }

    return *ptr;
}

const Value& Value::operator[](const size_t index) const {
    const Value* ptr;

    if (is_array()) {
        ptr = &m_array[index];
    }
    else if (is_object()) {
        ptr = &m_object[index].second;
    }
    else {
        ptr = this;
    }

    return *ptr;
}

void Value::push_back(const Value& value) {
    if (is_null()) { *this = Value(Type::ARRAY); }

    if (is_array()) {
        m_array.push_back(value);
    }
}

void Value::push_back(const Pair& pair) {
    if (is_null()) { *this = Type::OBJECT; }

    if (is_object()) {
        (*this)[pair.first] = pair.second;
    }
    else if (is_array()) {
        m_array.push_back(pair);
    }
}

void Value::pop_back() {
    if (is_array()) {
        m_array.pop_back();
    }
    else if (is_object()) {
        m_object.pop_back();
    }
    else {
        *this = Type::NIL;
    }
}

void Value::swap(Value& value) {
    Value temp(std::move(value));
    value = std::move(*this);
    *this = std::move(temp);
}

bool json::operator==(const Value& val1, const Value& val2) {
    if (val1.m_type != val2.m_type) { return false; }

    bool result = false;

    switch (val1.m_type) {
    case Value::Type::OBJECT:
        result = (val1.m_object == val2.m_object);
        break;
    case Value::Type::ARRAY:
        result = (val1.m_array == val2.m_array);
        break;
    case Value::Type::STRING:
        result = (val1.m_string == val2.m_string);
        break;
    case Value::Type::NUMBER:
        result = (val1.m_number == val2.m_number);
        break;
    case Value::Type::BOOLEAN:
        result = (val1.m_boolean == val2.m_boolean);
        break;
    case Value::Type::NIL:
        result = true;
        break;
    default:
        break;
    }

    return result;
}

bool json::operator<(const Value& val1, const Value& val2) {
    if (val1.m_type != val2.m_type) { return false; }

    bool result = false;

    switch (val1.m_type) {
    case Value::Type::OBJECT:
        result = (val1.m_object < val2.m_object);
        break;
    case Value::Type::ARRAY:
        result = (val1.m_array < val2.m_array);
        break;
    case Value::Type::STRING:
        result = (val1.m_string < val2.m_string);
        break;
    case Value::Type::NUMBER:
        result = (val1.m_number < val2.m_number);
        break;
    case Value::Type::BOOLEAN:
        result = (val1.m_boolean < val2.m_boolean);
        break;
    case Value::Type::NIL:
    default:
        break;
    }

    return result;
}

Value::iterator Value::begin() {
    iterator tmp;

    if (is_array()) {
        tmp = m_array.begin();
    }
    else if (is_object()) {
        tmp = m_object.begin();
    }
    else {
        tmp = this;
    }

    return tmp;
}

Value::iterator Value::end() {
    iterator tmp;

    if (is_array()) {
        tmp = m_array.end();
    }
    else if (is_object()) {
        tmp = m_object.end();
    }
    else {
        tmp = this;
    }

    return tmp;
}

Value::const_iterator Value::cbegin() const {
    const_iterator tmp;

    if (is_array()) {
        tmp = m_array.cbegin();
    }
    else if (is_object()) {
        tmp = m_object.cbegin();
    }
    else {
        tmp = this;
    }

    return tmp;
}

Value::const_iterator Value::cend() const {
    const_iterator tmp;

    if (is_array()) {
        tmp = m_array.cend();
    }
    else if (is_object()) {
        tmp = m_object.cend();
    }
    else {
        tmp = this;
    }

    return tmp;
}

Value::const_iterator Value::begin() const {
    return cbegin();
}

Value::const_iterator Value::end() const {
    return cend();
}
