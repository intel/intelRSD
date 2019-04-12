/*!
 * @brief util for converting numbers to pretty hexadecimal strings
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file to_hex_string.hpp
 */

#pragma once



#include <string>
#include <limits>
#include <iomanip>

namespace agent_framework {
namespace model {
namespace utils {

/*!
 * @brief Template function that converts numeric value into its hexadecimal string
 * representation. It works on various unsigned types, and lets user to decide
 * how many bytes should be shown (first template parameter), and whether to show base
 * or not. Examples:
 *     to_hex_string<3>(0x10203040)        = "0x203040"
 *     to_hex_string<3, false>(0x10203040) = "203040"
 *     to_hex_string<2>(0x00000010)        = "0x0010"
 * @param[in] value Numeric value to be converted
 * @return Input converted to the string
 * */
template<int BYTES_TO_SHOW, bool SHOW_BASE = true, typename T>
std::string to_hex_string(T value) {
    static_assert(std::is_unsigned<T>::value, "Expected unsigned integral type");
    static_assert(BYTES_TO_SHOW > 0 && BYTES_TO_SHOW <= sizeof(T), "BYTES_TO_SHOW exceeds type size");
    constexpr auto size_in_chars = 2 * BYTES_TO_SHOW;
    constexpr T mask = std::numeric_limits<T>::max() >> ((sizeof(T) - BYTES_TO_SHOW) * 8);
    std::stringstream str{};
    if (SHOW_BASE) {
        str << "0x";
    }
    str << std::hex << std::setfill('0') << std::setw(size_in_chars) << (value & mask);
    return str.str();
}

}
}
}
