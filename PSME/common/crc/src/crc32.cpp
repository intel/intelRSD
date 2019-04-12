/*!
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
 * @file crc/crc32.cpp
 */

#include "crc/crc32.hpp"

#include <limits>
#include <stdexcept>

namespace zlib {

extern "C" {
#include <zlib.h>
}

}

using namespace crc;

Crc32::BaseType Crc32::calculate(Crc32::BaseType seed_crc, const uint8_t* buffer, std::size_t size) {
    if (size > std::numeric_limits<zlib::uInt>::max()) {
        throw std::runtime_error("CRC32: too many bytes in the buffer!");
    }

    auto zlib_crc = zlib::crc32(seed_crc, buffer, static_cast<unsigned int>(size));

    if (zlib_crc > std::numeric_limits<Crc32::BaseType>::max()) {
        throw std::runtime_error("CRC32: calculated CRC is too large!");
    }
    return static_cast<Crc32::BaseType>(zlib_crc);
}

Crc32::BaseType Crc32::init() {
    auto zlib_crc = zlib::crc32(0L, Z_NULL, 0);

    if (zlib_crc > std::numeric_limits<Crc32::BaseType>::max()) {
        throw std::runtime_error("CRC32: calculated initial CRC is too large!");
    }
    return static_cast<Crc32::BaseType>(zlib_crc);
}
