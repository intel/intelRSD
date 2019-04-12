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
 *
 * @file tokenizer.hpp
 * @brief TGT Tokenizer interface
*/

#pragma once
#include <string>

namespace tgt {

/*! Class represents string tokenizer */
class Tokenizer {
public:
    /*!
     * @brief Tokenizer constructor
     * @param text String to split
     * @param delimiter Delimiter string
     */
    explicit Tokenizer(const std::string& text, const std::string& delimiter);

    /*!
     * @brief Check if string have more tokens
     * @return True on success, otherwise false
     */
    bool next_token();

    /*!
     * @brief Return next token
     * @return String token
     */
    const std::string& get_token() const;

private:
    const std::string m_text;
    std::size_t m_offset{};
    std::string m_delimiter;
    std::string m_token{};
};

}

