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
 * @file base64.cpp
 *
 * @brief Implements RFC4648 Base64 data encoding
 * */

#include <cmath>
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

#include "base64/base64.hpp"

using namespace base64;

constexpr const char Base64::BASE64URL_ALPHABET[];

constexpr const char Base64::BASE64_ALPHABET[];

const char BASE64_PAD = '=';

std::string Base64::encode(const std::vector<uint8_t>& input, const char* alphabet) {
    auto input_size = input.size();
    std::string result{};
    result.reserve(ceil(input_size / 3) * 4);

    std::vector<uint8_t>::size_type pos{0};
    for (; input_size > pos; pos += 3) {
        auto remaining_bytes = input_size - pos;
        if (3 <= remaining_bytes) {
            result.push_back(alphabet[(input[pos] & 0xfc) >> 2]);
            result.push_back(alphabet[((input[pos] & 0x03) << 4) + ((input[pos+1] & 0xf0) >> 4)]);
            result.push_back(alphabet[((input[pos+1] & 0x0f) << 2) + ((input[pos+2] & 0xc0) >> 6)]);
            result.push_back(alphabet[input[pos+2] & 0x3f]);
        }
        else if (2 == remaining_bytes) {
            result.push_back(alphabet[(input[pos] & 0xfc) >> 2]);
            result.push_back(alphabet[((input[pos] & 0x03) << 4) + ((input[pos+1] & 0xf0) >> 4)]);
            result.push_back(alphabet[(input[pos+1] & 0x0f) << 2]);
            result.push_back(BASE64_PAD);
        }
        else {
            result.push_back(alphabet[(input[pos] & 0xfc) >> 2]);
            result.push_back(alphabet[(input[pos] & 0x03) << 4]);
            result.push_back(BASE64_PAD);
            result.push_back(BASE64_PAD);
        }
    }
    return result;
}

std::string Base64::encode(const std::vector<uint8_t>& input) {
    return encode(input, Base64::BASE64_ALPHABET);
}

std::string Base64::encodeUrl(const std::vector<uint8_t>& input) {
    return encode(input, Base64::BASE64URL_ALPHABET);
}

/* This maps encoded Base64 to parser action and output bits.
   Both standard Base64 and Base64URL is supported.
   <0x40 - output bits
   0x80 - ignore
   0x82 - padding
*/
const uint8_t BASE64_DICT[] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x00
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x08
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x10
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x18
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x20
    0x80, 0x80, 0x80, 0x3e, 0x80, 0x3e, 0x80, 0x3f, // 0x28
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, // 0x30
    0x3c, 0x3d, 0x80, 0x80, 0x80, 0x82, 0x80, 0x80, // 0x38
    0x80, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // 0x40
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, // 0x48
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, // 0x50
    0x17, 0x18, 0x19, 0x80, 0x80, 0x80, 0x80, 0x3f, // 0x58
    0x80, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, // 0x60
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, // 0x68
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, // 0x70
    0x31, 0x32, 0x33, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x78
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x80
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x88
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x90
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0x98
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xa0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xa8
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xb0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xb8
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xc0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xc8
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xd0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xd8
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xe0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xe8
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0xf0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80  // 0xf8
};

std::vector<uint8_t> Base64::decode(const std::string& input) {
    std::vector<uint8_t> result{};
    result.reserve(ceil(input.size() / 4) * 3);
    size_t pos = 0;
    size_t q = 0; /* index of a next byte inside quadruplet */
    size_t p = 0; /* padding bytes processed */
    uint8_t tmp[4]; /* quadruplet buffer */

    while (pos < input.size()) {
        uint8_t c = BASE64_DICT[static_cast<uint8_t>(input[pos])];
        if ((0 == p) && (c < 0x40)) {
            /* Data bits received. We only process data until first
               padding byte is received. */
            tmp[q] = c;
            q = (q + 1) % 4;
            if (0 == q) {
                result.push_back((tmp[0] << 2) + ((tmp[1] & 0x30) >> 4));
                result.push_back(((tmp[1] & 0x0f) << 4) + ((tmp[2] & 0x3c) >> 2));
                result.push_back(((tmp[2] & 0x03) << 6) + tmp[3]);
            }
        }
        else if (0x82 == c) {
            /* Padding byte. */
            p++;
        }
        else {
            /* Ignore all other input */
        }
        pos++;
    }

    if (0 != (q + p) % 4) {
        /* After all the input is read and some bytes are properly ignored,
           we should be left with multiple of four of bytes in encoded
           representation. This includes both data and padding bytes. */
        throw std::runtime_error("Failed to parse Base64 input.");
    }

    /* Handle last quadruplet of input data which may include padding. */
    if (0 == p) {
        /* Bytes have been pushed to output during previous
           iteration of parser loop, nothing left to be done. */
    }
    else if (1 == p) {
        result.push_back((tmp[0] << 2) + ((tmp[1] & 0x30) >> 4));
        result.push_back(((tmp[1] & 0x0f) << 4) + ((tmp[2] & 0x3c) >> 2));
    }
    else if (2 == p) {
        result.push_back((tmp[0] << 2) + ((tmp[1] & 0x30) >> 4));
    }
    else {
        /* Too many padding bytes received. */
        throw std::runtime_error("Failed to parse Base64 input.");
    }

    return result;
}

