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
 * @file formatter/compact.hpp
 *
 * @brief JSON formatter interface
 * */

#ifndef JSON_CXX_FORMATTER_COMPACT_HPP
#define JSON_CXX_FORMATTER_COMPACT_HPP

#include "json/formatter.hpp"

namespace json {
namespace formatter {

/*!
 * @brief Compact formatter
 *
 * Creates serialized compact JSON data that not include whitespace or newlines
 * */
class Compact : public Formatter {
public:
    /*!
     * @brief Serialize JSON value
     *
     * @param[in]   value   JSON value
     * */
    void execute(const Value& value) final override;

    /*! Destructor */
    ~Compact();
protected:
    virtual void write_value(const Value& value);
    virtual void write_object(const Value& value);
    virtual void write_array(const Value& value);
    virtual void write_number(const Value& value);
    virtual void write_string(const Value& value);
    virtual void write_boolean(const Value& value);
    virtual void write_empty(const Value& value);
};

}
}

#endif /* JSON_CXX_FORMATTER_COMPACT_HPP */
