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
 * */

#include "json/iterator.hpp"

using namespace json;

template<bool is_const>
base_iterator<is_const>::base_iterator() :
    m_type(Value::Type::ARRAY)
{
    new (&m_array_iterator) array_iterator();
}

namespace json {

/*!
 * @brief Convert nnon-const iterator to const_iterator
 *
 * @param[in]   it  Non-const iterator
 * */
template<>
template<>
base_iterator<true>::base_iterator(const base_iterator<false>& it) :
    m_type(it.m_type)
{
    if (is_array()) {
        new (&m_array_iterator) array_iterator(it.m_array_iterator);
    }
    else if (is_object()) {
        new (&m_object_iterator) object_iterator(it.m_object_iterator);
    }
    else {
        new (&m_value_iterator) value_iterator(it.m_value_iterator);
    }
}

/*!
 * @brief New const_iterator that is assignment from non-const iterator
 *
 * @param[in]   it  Non-const iterator
 *
 * @return New const_iterator based on non-const iterator assignment
 * */
template<>
template<>
base_iterator<true>&
base_iterator<true>::operator=(const base_iterator<false>& it) {
    m_type = it.m_type;

    if (is_array()) {
        m_array_iterator = it.m_array_iterator;
    }
    else if (is_object()) {
        m_object_iterator = it.m_object_iterator;
    }
    else {
        m_value_iterator = it.m_value_iterator;
    }
    return *this;
}

}

template<bool is_const>
base_iterator<is_const>::base_iterator(const value_iterator& it) :
    m_type(it->get_type())
{
    new (&m_value_iterator) value_iterator(it);
}

template<bool is_const>
base_iterator<is_const>::base_iterator(const array_iterator& it) :
    m_type(Value::Type::ARRAY)
{
    new (&m_array_iterator) array_iterator(it);
}

template<bool is_const>
base_iterator<is_const>::base_iterator(const object_iterator& it) :
    m_type(Value::Type::OBJECT)
{
    new (&m_object_iterator) object_iterator(it);
}

template<bool is_const>
bool base_iterator<is_const>::is_array() const {
    return Value::Type::ARRAY == m_type;
}

template<bool is_const>
bool base_iterator<is_const>::is_object() const {
    return Value::Type::OBJECT == m_type;
}

template<bool is_const>
base_iterator<is_const>& base_iterator<is_const>::operator++() {
    if (is_array()) {
        m_array_iterator++;
    }
    else if (is_object()) {
        m_object_iterator++;
    }
    else {
        m_value_iterator++;
    }
    return *this;
}

template<bool is_const>
base_iterator<is_const> base_iterator<is_const>::operator++(int) {
    base_iterator temp(*this);
    operator++();
    return temp;
}

template<bool is_const>
typename base_iterator<is_const>::pointer
base_iterator<is_const>::operator->() const {
    pointer ptr;
    if (is_array()) {
        ptr = &(*m_array_iterator);
    }
    else if (is_object()) {
        ptr = &m_object_iterator->second;
    }
    else {
        ptr = m_value_iterator;
    }
    return ptr;
}

template<bool is_const>
typename base_iterator<is_const>::reference
base_iterator<is_const>::operator*() const {
    return *operator->();
}

template<bool is_const>
typename base_iterator<is_const>::reference
base_iterator<is_const>::operator[](difference_type n) const {
    pointer ptr;

    if (is_array()) {
        ptr = &m_array_iterator[n];
    }
    else if (is_object()) {
        ptr = &m_object_iterator[n].second;
    }
    else {
        ptr = &m_value_iterator[n];
    }

    return *ptr;
}

template<bool is_const>
base_iterator<is_const>&
base_iterator<is_const>::operator+=(difference_type n) {
    if (is_array()) {
        m_array_iterator += n;
    }
    else if (is_object()) {
        m_object_iterator += n;
    }
    else {
        if (n >= 0) {
            while (n--) { ++m_value_iterator; }
        }
        else {
            while (n++) { --m_value_iterator; }
        }
    }

    return *this;
}

template<bool is_const>
base_iterator<is_const>&
base_iterator<is_const>::operator-=(difference_type n) {
    if (is_array()) {
        m_array_iterator += -n;
    }
    else if (is_object()) {
        m_object_iterator += -n;
    }
    else {
        m_value_iterator += -n;
    }

    return *this;
}

template<bool is_const>
const char* base_iterator<is_const>::key() const {
    if (is_object()) {
        return m_object_iterator->first.c_str();
    }
    return "";
}

bool json::operator==(const base_iterator<true>& it1,
        const base_iterator<true>& it2) {
    if (it1.m_type != it2.m_type) { return false; }

    bool result = false;

    if (Value::Type::ARRAY == it1.m_type) {
        result = (it1.m_array_iterator == it2.m_array_iterator);
    }
    else if (Value::Type::OBJECT == it1.m_type) {
        result = (it1.m_object_iterator == it2.m_object_iterator);
    }
    else {
        result = (it1.m_value_iterator == it2.m_value_iterator);
    }

    return result;
}

bool json::operator!=(const base_iterator<true>& it1,
        const base_iterator<true>& it2) {
    return !(it1 == it2);
}

bool json::operator<(const base_iterator<true>& it1,
        const base_iterator<true>& it2) {
    if (it1.m_type != it2.m_type) { return false; }

    bool result = false;

    if (Value::Type::ARRAY == it1.m_type) {
        result = (it1.m_array_iterator < it2.m_array_iterator);
    }
    else if (Value::Type::OBJECT == it1.m_type) {
        result = (it1.m_object_iterator < it2.m_object_iterator);
    }
    else {
        result = (it1.m_value_iterator < it2.m_value_iterator);
    }

    return result;
}

bool json::operator>(const base_iterator<true>& it1,
        const base_iterator<true>& it2) {
    return it2 < it1;
}

bool json::operator<=(const base_iterator<true>& it1,
        const base_iterator<true>& it2) {
    return !(it2 < it1);
}

bool json::operator>=(const base_iterator<true>& it1,
        const base_iterator<true>& it2) {
    return !(it1 < it2);
}

base_iterator<>::difference_type
json::operator-(base_iterator<true> it2,
        base_iterator<true> it1) {
    if (it1.m_type != it2.m_type) { return 0; }

    base_iterator<>::difference_type n = 0;

    if (it1 < it2) {
        while (it1 < it2) { ++n; ++it1; }
    }
    else {
        while (it2 < it1) { --n; ++it2; }
    }

    return n;
}

void json::swap(base_iterator<>& it1, base_iterator<>& it2) {
    if (it1.m_type != it2.m_type) { return; }

    if (Value::Type::ARRAY == it1.m_type) {
        swap(it1.m_array_iterator, it2.m_array_iterator);
    }
    else if (Value::Type::OBJECT == it1.m_type) {
        swap(it1.m_object_iterator, it2.m_object_iterator);
    }
    else {
        Value* tmp = it1.m_value_iterator;
        it1.m_value_iterator = it2.m_value_iterator;
        it2.m_value_iterator = tmp;
    }
}

template<bool is_const>
base_iterator<is_const> json::operator+(
        const base_iterator<is_const>& it,
        typename  base_iterator<is_const>::difference_type n) {
    base_iterator<is_const> tmp = it;
    return tmp += n;
}

template<bool is_const>
base_iterator<is_const> json::operator+(
        typename base_iterator<is_const>::difference_type n,
        const base_iterator<is_const>& it) {
    return it + n;
}

template<bool is_const>
base_iterator<is_const> json::operator-(const base_iterator<is_const>& it,
        typename base_iterator<is_const>::difference_type n) {
    base_iterator<is_const> tmp = it;
    return tmp -= n;
}

namespace json {

/*! Get normal iterator with added offset */
template
base_iterator<> operator+(const base_iterator<>& it,
        base_iterator<>::difference_type n);

/*! Get normal iterator with added offset */
template
base_iterator<> operator+(base_iterator<>::difference_type n,
        const base_iterator<>& it);

/*! Get normal iterator with subtracted offset */
template
base_iterator<> operator-(const base_iterator<>& it,
        base_iterator<>::difference_type n);

/*! Get const_iterator with added offset */
template
base_iterator<true> operator+(const base_iterator<true>& it,
        base_iterator<true>::difference_type n);

/*! Get const_iterator with added offset */
template
base_iterator<true> operator+(base_iterator<true>::difference_type n,
        const base_iterator<true>& it);

/*! Get const_iterator with subtracted offset */
template
base_iterator<true> operator-(const base_iterator<true>& it,
        base_iterator<true>::difference_type n);

template class base_iterator<>;
template class base_iterator<true>;

}
