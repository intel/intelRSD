/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tgt/tokenizer.hpp"



using namespace tgt;


Tokenizer::Tokenizer(const std::string& text, const std::string& delimiter) :
    m_text(text),
    m_delimiter(delimiter) {}


bool Tokenizer::next_token() {
    std::size_t i = m_text.find_first_not_of(m_delimiter, m_offset);
    if (std::string::npos == i) {
        m_offset = m_text.length();
        return false;
    }

    std::size_t j = m_text.find_first_of(m_delimiter, i);
    if (std::string::npos == j) {
        m_token = m_text.substr(i);
        m_offset = m_text.length();
        return true;
    }
    m_token = m_text.substr(i, j - i);
    m_offset = j;
    return true;
}


const std::string& Tokenizer::get_token() const {
    return m_token;
}


