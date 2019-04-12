/*!
 * @brief Utils for switching endianness.

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
 * @file utils/byte_swap.hpp
 */

#pragma once

#include <cstdint>

namespace utils {

/*!
 * @brief Switches the endianness of a 16-bit integer.
 * @param value 16-bit unsigned value
 * @return The byte swapped value
 */
std::uint16_t byte_swap_16(std::uint16_t value);

/*!
 * @brief Switches the endianness of a 32-bit integer.
 * @param value 32-bit unsigned value
 * @return The byte swapped value
 */
std::uint32_t byte_swap_32(std::uint32_t value);

/*!
 * @brief Switches the endianness of a 64-bit integer.
 * @param value 64-bit unsigned value
 * @return The byte swapped value
 */
std::uint64_t byte_swap_64(std::uint64_t value);

}