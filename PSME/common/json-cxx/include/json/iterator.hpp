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
 * @file json/iterator.hpp
 *
 * @brief JSON iterator interface
 * */

#ifndef JSON_CXX_ITERATOR_HPP
#define JSON_CXX_ITERATOR_HPP

#include "json/value.hpp"

#include <utility>
#include <iterator>

/*! as*/
namespace json {

    /*! asd */
template<bool is_const>
class base_iterator;

/*!
 * @brief Get new iterator with added offset
 * @tparam      is_const When true use const_iterator, otherwise normal iterator
 * @param[in]   it  Iterator
 * @param[in]   n   Offset
 *
 * Offset can be unsigned/signed
 * */
template<bool is_const = false>
base_iterator<is_const> operator+(const base_iterator<is_const>& it,
       typename  base_iterator<is_const>::difference_type n);

/*!
 * @brief Get new iterator with added offset
 *
 * Offset can be unsigned/signed
 * */
template<bool is_const = false>
base_iterator<is_const> operator+(
        typename base_iterator<is_const>::difference_type,
        const base_iterator<is_const>&);

/*!
 * @brief Get new iterator with substrate offset
 *
 * Offset can be unsigned/signed
 * */
template<bool is_const = false>
base_iterator<is_const> operator-(const base_iterator<is_const>&,
        typename base_iterator<is_const>::difference_type);

/*!
 * @brief Base iterator for JSON values
 *
 * Base iterator can be non-const or const based on is_const switch.
 * On default, base iterator instance behave as normal STL iterator.
 * Base iterators work with STL templates and algorithms.
 * */
template<bool is_const = false>
class base_iterator {
public:
    /*! C++11 workaround for missing C++14 standard */
    template<bool B, class T, class F>
    using conditional_t = typename std::conditional<B, T, F>::type;

    /*! Iterator type */
    using value_type = Value;

    /*! Distance difference between iterators */
    using difference_type = std::ptrdiff_t;

    /*! Iterator pointer type */
    using pointer = conditional_t<is_const, const value_type*, value_type*>;

    /*! Iterator reference*/
    using reference = conditional_t<is_const, const value_type&, value_type&>;

    /*! Iterator category */
    using iterator_category = std::random_access_iterator_tag;

    /*! Iterator that point to JSON value */
    using value_iterator = pointer;

    /*! Iterator that point to JSON array */
    using array_iterator = conditional_t<is_const,
          Array::const_iterator, Array::iterator>;

    /*! Iterator that point to JSON object */
    using object_iterator = conditional_t<is_const,
          Object::const_iterator, Object::iterator>;

    /*!
     * @brief Default constructor
     *
     * Create JSON array iterator
     * */
    base_iterator();

    /*!
     * @brief Copy constructor
     * */
    base_iterator(const base_iterator&) = default;

    /*!
     * @brief Move constructor
     * */
    base_iterator(base_iterator&&) = default;

    /*!
     * @brief Constructor that convert normal iterator to const_iterator
     *
     * This constructor is enabled only for const_iterator instance,
     * in other way base_iterator with is_const set to true
     * */
    template<typename = typename std::enable_if<is_const>>
    base_iterator(const base_iterator<>&);

    /*!
     * @brief Create JSON value iterator
     * */
    base_iterator(const value_iterator&);

    /*!
     * @brief Create JSON array iterator
     * */
    base_iterator(const array_iterator&);

    /*!
     * @brief Create JSON object iterator
     * */
    base_iterator(const object_iterator&);

    /*!
     * @brief Get JSON key member on which iterator point
     *
     * @return Key string name when success otherwise empty "" string
     * */
    const char* key() const;

    /*!
     * @brief Check if iterator point to JSON array
     * */
    bool is_array() const;

    /*!
     * @brief Check if iterator point to JSON object
     */
    bool is_object() const;

    /*!
     * @brief Copy assignment that convert normal iterator to const_iterator
     *
     * This operator is enabled only for const_iterator instance,
     * in other way base_iterator with is_const set to true
     * */
    template<typename = typename std::enable_if<is_const>>
    base_iterator& operator=(const base_iterator<>& it);

    /*!
     * @brief Copy assignment
     * */
    base_iterator& operator=(const base_iterator& it) = default;

    /*!
     * @brief Move assignment
     * */
    base_iterator& operator=(base_iterator&& it) = default;

    /*!
     * @brief Post-increment iterator, point to next element
     * */
    base_iterator& operator++();

    /*!
     * @brief Pre-increment iterator, point to next element
     * */
    base_iterator operator++(int);

    /*!
     * @brief Get access to element through iterator
     * */
    reference operator*() const;

    /*!
     * @brief Get access to element through iterator
     * */
    pointer operator->() const;

    /*!
     * @brief Get access to element through iterator as array
     *
     * This should be used only for JSON array/object iterators
     * */
    reference operator[](difference_type) const;

    /*!
     * @brief Add offset to iterator
     *
     * Offset can be unsigned/signed
     * */
    base_iterator& operator+=(difference_type);

    /*!
     * @brief Subtract offset from iterator
     *
     * Offset can be unsigned/signed
     * */
    base_iterator& operator-=(difference_type);

    /*! JSON value friend */
    friend Value;

    /*! const_iterator is friend for normal iterator */
    friend base_iterator<true>;

    /*! Compare two iterators. It1 position is less than it2 position */
    friend bool operator<(const base_iterator<true>&,
            const base_iterator<true>&);

    /*! Compare two iterators. It1 position is greater than it2 position */
    friend bool operator>(const base_iterator<true>&,
            const base_iterator<true>&);

    /*! Compare two iterators. It1 position is less than or equal to it2
     * position */
    friend bool operator<=(const base_iterator<true>&,
            const base_iterator<true>&);

    /*! Compare two iterators. It1 position is greater than or equal to it2
     * position */
    friend bool operator>=(const base_iterator<true>&,
            const base_iterator<true>&);

    /*! Iterators are equal */
    friend bool operator==(
            const base_iterator<true>&,
            const base_iterator<true>&);

    /*! Iterators are not equal */
    friend bool operator!=(
            const base_iterator<true>&,
            const base_iterator<true>&);

    /*! Get iterator with added offset */
    friend base_iterator operator+<>(const base_iterator&,
            base_iterator::difference_type);

    /*! Get iterator with added offset */
    friend base_iterator operator+<>(base_iterator::difference_type,
            const base_iterator&);

    /*! Get iterator with substrate offset */
    friend base_iterator operator-<>(const base_iterator&,
            base_iterator::difference_type);

    /*! Get difference between iterators */
    friend base_iterator::difference_type operator-(
            base_iterator<true>,
            base_iterator<true>);

    /*! Swap iterators */
    friend void swap(base_iterator<>&, base_iterator<>&);
private:
    typename Value::Type m_type;

    union {
        value_iterator m_value_iterator;    /*!< JSON value iterator */
        array_iterator m_array_iterator;    /*!< JSON array iterator */
        object_iterator m_object_iterator;  /*!< JSON object iterator */
    };
};

bool operator<(const base_iterator<true>&, const base_iterator<true>&);

bool operator>(const base_iterator<true>&, const base_iterator<true>&);

bool operator<=(const base_iterator<true>&, const base_iterator<true>&);

bool operator>=(const base_iterator<true>&, const base_iterator<true>&);

bool operator==(const base_iterator<true>&, const base_iterator<true>&);

bool operator!=(const base_iterator<true>&, const base_iterator<true>&);

base_iterator<>::difference_type operator-(base_iterator<true>,
        base_iterator<true>);

void swap(base_iterator<>&, base_iterator<>&);

} /* namespace json */

#endif /* JSON_CXX_ITERATOR_HPP */
