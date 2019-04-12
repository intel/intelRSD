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
 * @file utils/byte_swap.cpp
 */

#include "utils/byte_swap.hpp"

namespace utils {

std::uint16_t byte_swap_16(std::uint16_t value) {
    return (std::uint16_t)((value << 8) | (value >> 8));
}

std::uint32_t byte_swap_32(std::uint32_t value) {
    std::uint32_t low_bytes  = byte_swap_16((std::uint16_t)value);
    std::uint32_t high_bytes = byte_swap_16((std::uint16_t)(value >> 16));

    return (low_bytes << 16 | high_bytes);
}

std::uint64_t byte_swap_64(std::uint64_t value) {
    std::uint64_t low_bytes  = byte_swap_32((std::uint32_t)value);
    std::uint64_t high_bytes = byte_swap_32((std::uint32_t)(value >> 32));

    return (low_bytes << 32 | high_bytes);
}

}