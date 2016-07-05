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
 * @file writter/counter.hpp
 *
 * @brief JSON writter interface
 * */

#ifndef JSON_CXX_WRITTER_COUNTER_HPP
#define JSON_CXX_WRITTER_COUNTER_HPP

#include "json/writter.hpp"

#include <cstring>

namespace json {
namespace writter {

/*!
 * @brief Writter that counts characters
 * */
class Counter : public Writter {
public:
    /*!
     * @brief Default constructor
     * */
    Counter() : m_count(0) { }

    /*!
     * @brief Pop character
     * */
    void pop_back() { --m_count; }

    /*!
     * @brief Push character
     * */
    void push_back(char) { ++m_count; }

    /*!
     * @brief Append repeated character
     *
     * @param[in]   count   Character repeated count times
     * */
    void append(std::size_t count, char) { m_count += count; }

    /*!
     * @brief Append array of characters terminated with '\0'
     *
     * @param[in]   str     Array of characters terminated with '\0'
     * */
    void append(const char* str) { m_count += strlen(str); }

    /*!
     * @brief Append with string
     *
     * @param[in]   str     String object
     * */
    void append(const std::string& str) { m_count += str.size(); }

    /*!
     * @brief Get counted characters
     *
     * @return  Counted characters
     * */
    size_t get_count() { return m_count; }

    /*! Destructor */
    ~Counter();
private:
    size_t m_count;
};

}
}

#endif /* JSON_CXX_WRITTER_COUNTER_HPP */
