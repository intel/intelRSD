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
 * @file formatter/pretty.cpp
 *
 * @brief JSON formatter implementation
 * */

#include "json/formatter/pretty.hpp"

#include "json/iterator.hpp"

#include <iomanip>
#include <sstream>

using namespace json::formatter;

Pretty::~Pretty() { }

void Pretty::write_object(const Value& value) {
    if (value.size() > 0) {
        m_writter->push_back('{');

        ++m_level;
        size_t indent_length = m_indent * m_level;

        const Object& obj = Object(value);
        for (auto it = obj.cbegin(); it < obj.cend(); ++it) {
            m_writter->push_back('\n');
            m_writter->append(indent_length, ' ');
            write_string(it->first);
            m_writter->append(" : ");
            write_value(it->second);
            m_writter->push_back(',');
        };

        m_writter->pop_back();
        m_writter->push_back('\n');

        --m_level;

        m_writter->append(m_indent * m_level, ' ');
        m_writter->push_back('}');
    }
    else {
        m_writter->append("{}");
    }
}

void Pretty::write_array(const Value& value) {
    if (value.size() > 0) {
        m_writter->push_back('[');

        ++m_level;

        size_t indent_length = m_indent * m_level;

        for (const auto& val : value) {
            m_writter->push_back('\n');
            m_writter->append(indent_length, ' ');
            write_value(val);
            m_writter->push_back(',');
        }

        m_writter->pop_back();
        m_writter->push_back('\n');

        --m_level;

        m_writter->append(m_indent * m_level, ' ');
        m_writter->push_back(']');
    }
    else {
        m_writter->append("[]");
    }
}
