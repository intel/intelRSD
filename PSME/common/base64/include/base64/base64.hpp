/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file base64.hpp
 * @brief Implements RFC4648 Base64 data encoding
 * */

#ifndef BASE64_HPP
#define BASE64_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace base64 {

/*!
 * @brief Base64 util class
 *
 * Allows to encode and decode binary data to and from Base64 encoding.
 * */
class Base64 {
public:

    static constexpr const char BASE64URL_ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    static constexpr const char BASE64_ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /*!
     * @brief Encodes binary data to Base64 representation using
     * specified alphabet (see RFC4648).
     *
     * @param[in] input binary data to be encoded
     * @param[in] alphabet Base64 alphabet to be used for encoding
     * @return Encoded data as string
     * */
    static std::string encode(const std::vector<uint8_t>& input,
                              const char* alphabet);

    /*!
     * @brief Encodes binary data to Base64 representation using
     * standard alphabet (see RFC4648).
     *
     * @param[in] input binary data to be encoded
     * @return Encoded data as string
     * */
    static std::string encode(const std::vector<uint8_t>& input);

    /*!
     * @brief Encodes binary data to Base64 representation using
     * alphabet that allows resulting representation to be used inside
     * URL or file name (see RFC4648).
     *
     * @param[in] input binary data to be encoded
     * @return Encoded data as string
     * */
    static std::string encodeUrl(const std::vector<unsigned char>& input);

    /*!
     * @brief Decodes Base64 RFC4648-compliant string back to binary.
     * Ignores characters not specified in encoding. Accepts and decodes
     * strings encoded using both standard and url-safe alphabets.
     *
     * @param[in] input RFC4648-compliant Base64 string
     * @return Binary data
     * */
    static std::vector<unsigned char> decode(const std::string& input);

};

}

#endif
