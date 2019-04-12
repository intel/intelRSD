/*!
 * @brief Implementation of function for converting linear format to double.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file utils/sdv/linear_to_double.cpp
 */

#include <climits>
#include <cstdint>

#include "ipmi/utils/sdv/linear_to_double.hpp"

namespace {

template<typename T, std::size_t N>
constexpr T n_bit_mask() {
    static_assert(N > 0, "number of mask bits should be greater than zero");
    static_assert(sizeof(T) * CHAR_BIT > N, "number of mask bits exceeds type size");
    return (1 << N) - 1;
}

constexpr const std::size_t EXPONENT_BITS = 5;
constexpr const auto EXPONENT_MASK = n_bit_mask<std::uint8_t, EXPONENT_BITS>();
constexpr const auto MAX_EXPONENT_VALUE = EXPONENT_MASK >> 1;

constexpr const std::size_t MANTISSA_BITS = 11;
constexpr const auto MANTISSA_MASK = n_bit_mask<std::uint16_t, MANTISSA_BITS>();
constexpr const auto MAX_MANTISSA_VALUE = MANTISSA_MASK >> 1;

}

double ipmi::sdv::linear_to_double(std::uint16_t linear) {
    std::int8_t exponent = std::int8_t((linear >> MANTISSA_BITS) & EXPONENT_MASK);
    std::int16_t mantissa = linear & MANTISSA_MASK;

    double scaling_factor = (exponent > MAX_EXPONENT_VALUE)
                          ? (1.0 / (1 << ((~exponent + 1) & EXPONENT_MASK))) : (1 << exponent);

    if (mantissa > MAX_MANTISSA_VALUE) {
        mantissa |= ~MANTISSA_MASK;
    }

    return mantissa * scaling_factor;
}
