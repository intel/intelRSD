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
 * @file formatter.cpp
 *
 * @brief JSON formatter implementation
 * */

#include "json/formatter.hpp"

#include <algorithm>

using namespace json;

constexpr const char Formatter::JSON_NULL[];
constexpr const char Formatter::JSON_TRUE[];
constexpr const char Formatter::JSON_FALSE[];

std::string Formatter::escape_characters(const std::string& str) {
    std::string::difference_type count =
        std::count_if(str.cbegin(), str.cend(),
            [](const char& ch) { return (('\\' == ch) || ('\"' == ch)); }
        );

    if (0 == count) { return str; }

    std::string escaped {};
    escaped.reserve(str.size() + std::string::size_type(count));

    std::for_each(str.cbegin(), str.cend(),
        [&escaped](const char& ch) {
            if (('\\' == ch) || ('\"' == ch)) {
                escaped.push_back('\\');
            }
            escaped.push_back(ch);
        }
    );

    return escaped;
}

Formatter::~Formatter() { }
