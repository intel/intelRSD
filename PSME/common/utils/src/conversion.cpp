/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file utils/conversion.cpp
 */

#include "utils/conversion.hpp"
#include <iomanip>
#include <sstream>

namespace utils {

std::string buffer_to_string(const uint8_t* data, uint8_t length, BufferEndianness endianness) {
    std::stringstream ss;

    if (endianness == BufferEndianness::LittleEndian) {
        for (int i = (length - 1); i >= 0; i--) {
            ss << std::hex << std::setfill('0') << std::setw(2) << int(data[i]);
        }
    }
    else {
        for (int i = 0; i < length; i++) {
            ss << std::hex << std::setfill('0') << std::setw(2) << int(data[i]);
        }
    }

    return ss.str();
}

std::uint64_t buffer_to_integer(const uint8_t* data, uint8_t length, BufferEndianness endianness) {
    std::uint64_t number = 0;

    if (endianness == BufferEndianness::LittleEndian) {
        for (int i = (length - 1); i >= 0; i--) {
            number = (number << 8) + data[i];
        }
    }
    else {
        for (int i = 0 ; i < length; i++) {
            number = (number << 8) + data[i];
        }
    }

    return number;
}

double convert_byte_units(double value, BytePrefix from, BytePrefix to) {
    return (value / ((1 << uint32_t(to)) >> uint32_t(from)));
}

std::string string_to_hex_string(const std::string& ascii_string) {
    std::stringstream ss;

    for (const auto cc : ascii_string) {
        ss << std::setfill('0')
           << std::setw(2)
           << std::uppercase
           << std::hex
           << unsigned(static_cast<uint8_t>(cc));
    }

    return ss.str();
}

std::string hex_string_to_string(const std::string& hex_string) {
    std::string ascii_string;
    int char_ascii_code;

    for (int i = 0; i < (int)hex_string.size(); i += 2) {
        std::stringstream ss(hex_string.substr(i, 2));
        ss >> std::hex >> char_ascii_code;
        ascii_string += static_cast<char>(char_ascii_code);
    }

    return ascii_string;
}

}