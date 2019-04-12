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
 * @file utils/byte_buffer.cpp
 */

#include "utils/byte_buffer.hpp"

#include <sstream>

namespace {

template <typename T, unsigned BITS>
T typed_shift(T in) {
    return T(in << BITS);
}

template <typename T, unsigned BITS>
uint8_t typed_mask(T in) {
    return uint8_t((in >> BITS) & 0xFF);
}

}

using namespace utils;

uint16_t ByteBuffer::get_le16(std::size_t pos) const {
    return typed_shift<uint16_t, 0>(at(pos + 0)) |
           typed_shift<uint16_t, 8>(at(pos + 1));
}

uint32_t ByteBuffer::get_le32(std::size_t pos) const {
    return typed_shift<uint32_t,  0>(at(pos + 0)) |
           typed_shift<uint32_t,  8>(at(pos + 1)) |
           typed_shift<uint32_t, 16>(at(pos + 2)) |
           typed_shift<uint32_t, 24>(at(pos + 3));
}

uint64_t ByteBuffer::get_le48(std::size_t pos) const {
    return typed_shift<uint64_t,  0>(at(pos + 0)) |
           typed_shift<uint64_t,  8>(at(pos + 1)) |
           typed_shift<uint64_t, 16>(at(pos + 2)) |
           typed_shift<uint64_t, 24>(at(pos + 3)) |
           typed_shift<uint64_t, 32>(at(pos + 4)) |
           typed_shift<uint64_t, 40>(at(pos + 5));
}

uint64_t ByteBuffer::get_le64(std::size_t pos) const {
    return typed_shift<uint64_t,  0>(at(pos + 0)) |
           typed_shift<uint64_t,  8>(at(pos + 1)) |
           typed_shift<uint64_t, 16>(at(pos + 2)) |
           typed_shift<uint64_t, 24>(at(pos + 3)) |
           typed_shift<uint64_t, 32>(at(pos + 4)) |
           typed_shift<uint64_t, 40>(at(pos + 5)) |
           typed_shift<uint64_t, 48>(at(pos + 6)) |
           typed_shift<uint64_t, 56>(at(pos + 7));
}

uint16_t ByteBuffer::get_be16(std::size_t pos) const {
    return typed_shift<uint16_t, 0>(at(pos + 1)) |
           typed_shift<uint16_t, 8>(at(pos + 0));
}


uint32_t ByteBuffer::get_be32(std::size_t pos) const {
    return typed_shift<uint32_t,  0>(at(pos + 3)) |
           typed_shift<uint32_t,  8>(at(pos + 2)) |
           typed_shift<uint32_t, 16>(at(pos + 1)) |
           typed_shift<uint32_t, 24>(at(pos + 0));
}

uint64_t ByteBuffer::get_be48(std::size_t pos) const {
    return typed_shift<uint64_t,  0>(at(pos + 5)) |
           typed_shift<uint64_t,  8>(at(pos + 4)) |
           typed_shift<uint64_t, 16>(at(pos + 3)) |
           typed_shift<uint64_t, 24>(at(pos + 2)) |
           typed_shift<uint64_t, 32>(at(pos + 1)) |
           typed_shift<uint64_t, 40>(at(pos + 0));
}

uint64_t ByteBuffer::get_be64(std::size_t pos) const {
    return typed_shift<uint64_t,  0>(at(pos + 7)) |
           typed_shift<uint64_t,  8>(at(pos + 6)) |
           typed_shift<uint64_t, 16>(at(pos + 5)) |
           typed_shift<uint64_t, 24>(at(pos + 4)) |
           typed_shift<uint64_t, 32>(at(pos + 3)) |
           typed_shift<uint64_t, 40>(at(pos + 2)) |
           typed_shift<uint64_t, 48>(at(pos + 1)) |
           typed_shift<uint64_t, 56>(at(pos + 0));
}

void ByteBuffer::set_le16(std::size_t pos, uint16_t value) {
    at(pos + 0) = typed_mask<uint16_t, 0>(value);
    at(pos + 1) = typed_mask<uint16_t, 8>(value);
}

void ByteBuffer::set_le32(std::size_t pos, uint32_t value) {
    at(pos + 0) = typed_mask<uint32_t,  0>(value);
    at(pos + 1) = typed_mask<uint32_t,  8>(value);
    at(pos + 2) = typed_mask<uint32_t, 16>(value);
    at(pos + 3) = typed_mask<uint32_t, 24>(value);
}

void ByteBuffer::set_le48(std::size_t pos, uint64_t value) {
    at(pos + 0) = typed_mask<uint64_t,  0>(value);
    at(pos + 1) = typed_mask<uint64_t,  8>(value);
    at(pos + 2) = typed_mask<uint64_t, 16>(value);
    at(pos + 3) = typed_mask<uint64_t, 24>(value);
    at(pos + 4) = typed_mask<uint64_t, 32>(value);
    at(pos + 5) = typed_mask<uint64_t, 40>(value);
}

void ByteBuffer::set_le64(std::size_t pos, uint64_t value) {
    at(pos + 0) = typed_mask<uint64_t,  0>(value);
    at(pos + 1) = typed_mask<uint64_t,  8>(value);
    at(pos + 2) = typed_mask<uint64_t, 16>(value);
    at(pos + 3) = typed_mask<uint64_t, 24>(value);
    at(pos + 4) = typed_mask<uint64_t, 32>(value);
    at(pos + 5) = typed_mask<uint64_t, 40>(value);
    at(pos + 6) = typed_mask<uint64_t, 48>(value);
    at(pos + 7) = typed_mask<uint64_t, 56>(value);
}

void ByteBuffer::set_be16(std::size_t pos, uint16_t value) {
    at(pos + 1) = typed_mask<uint16_t, 0>(value);
    at(pos + 0) = typed_mask<uint16_t, 8>(value);
}

void ByteBuffer::set_be32(std::size_t pos, uint32_t value) {
    at(pos + 3) = typed_mask<uint32_t,  0>(value);
    at(pos + 2) = typed_mask<uint32_t,  8>(value);
    at(pos + 1) = typed_mask<uint32_t, 16>(value);
    at(pos + 0) = typed_mask<uint32_t, 24>(value);
}

void ByteBuffer::set_be48(std::size_t pos, uint64_t value) {
    at(pos + 5) = typed_mask<uint64_t,  0>(value);
    at(pos + 4) = typed_mask<uint64_t,  8>(value);
    at(pos + 3) = typed_mask<uint64_t, 16>(value);
    at(pos + 2) = typed_mask<uint64_t, 24>(value);
    at(pos + 1) = typed_mask<uint64_t, 32>(value);
    at(pos + 0) = typed_mask<uint64_t, 40>(value);
}

void ByteBuffer::set_be64(std::size_t pos, uint64_t value) {
    at(pos + 7) = typed_mask<uint64_t,  0>(value);
    at(pos + 6) = typed_mask<uint64_t,  8>(value);
    at(pos + 5) = typed_mask<uint64_t, 16>(value);
    at(pos + 4) = typed_mask<uint64_t, 24>(value);
    at(pos + 3) = typed_mask<uint64_t, 32>(value);
    at(pos + 2) = typed_mask<uint64_t, 40>(value);
    at(pos + 1) = typed_mask<uint64_t, 48>(value);
    at(pos + 0) = typed_mask<uint64_t, 56>(value);
}
