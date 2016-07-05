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
 * @file writter/string.hpp
 *
 * @brief JSON writter interface
 * */

#ifndef JSON_CXX_WRITTER_STRING_HPP
#define JSON_CXX_WRITTER_STRING_HPP

#include "json/writter.hpp"

namespace json {
namespace writter {

/*!
 * @brief Writter that writes data to string
 * */
class String : public Writter {
public:
    /*!
     * @brief Default constructor
     * */
    String(size_t reserve = 0) : m_string {} { m_string.reserve(reserve); }

    /*!
     * @brief Pop character
     * */
    void pop_back() { m_string.pop_back(); }

    /*!
     * @brief Push character
     * */
    void push_back(char ch) { m_string.push_back(ch); }

    /*!
     * @brief Append repeated character
     *
     * @param[in]   count   Character repeated count times
     * @param[in]   ch      Char character
     * */
    void append(size_t count, char ch) { m_string.append(count, ch); }

    /*!
     * @brief Append array of characters terminated with '\0'
     *
     * @param[in]   str     Array of characters terminated with '\0'
     * */
    void append(const char* str) { m_string.append(str); }

    /*!
     * @brief Append with string
     *
     * @param[in]   str     String object
     * */
    void append(const std::string& str) { m_string.append(str); }

    /*!
     * @brief Get string
     *
     * @return String that contains written data
     * */
    const std::string& get_string() const { return m_string; }

    /*!
     * @brief Move string that contains written data using move schematics
     *
     * Moved data is invalid
     *
     * @return Moved string that contains written data. Moved data is invalid
     * */
    std::string&& move_string() { return std::move(m_string); }

    /*! Destructor */
    ~String();
private:
    std::string m_string;
};

}
}

#endif /* JSON_CXX_WRITTER_STRING_HPP */
