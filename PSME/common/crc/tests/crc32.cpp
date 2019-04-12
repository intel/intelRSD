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
 * @file tests/crc.cpp
 */

#include "crc/crc32.hpp"

#include "gtest/gtest.h"

#include <string>
#include <vector>

using namespace crc;

namespace {

uint32_t get_string_crc(const std::string& data) {
    return Crc32(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());;
}

uint32_t get_vector_crc(const std::vector<uint8_t>& data) {
    return Crc32(reinterpret_cast<const uint8_t*>(data.data()), data.size());;
}

}

namespace testing {

/*
 * There are many variations of CRC32 checksums. These tests ensure that the Crc32 implementation class
 * implements the correct CRC32 variation.
 */

TEST(Crc32Test, CrcStringTests) {
    EXPECT_EQ(0x00000000, get_string_crc(""));
    EXPECT_EQ(0xCBF43926, get_string_crc("123456789"));
    EXPECT_EQ(0x414FA339, get_string_crc("The quick brown fox jumps over the lazy dog"));
}

TEST(Crc32Test, CrcVectorTests) {
    // this is an exemplary GPT table header, 92 bytes
    // 4 bytes starting at 16 offset normally store CRC32, but for CRC32 calculations they are zeroed
    std::vector<uint8_t> data = {
        0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54, 0x00, 0x00, 0x01, 0x00, 0x5C, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xAF, 0x90, 0x93, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x8E, 0x90, 0x93, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x96, 0x7C, 0x98, 0xF2, 0x6A, 0x9D, 0x10, 0x47,
        0xB8, 0x5D, 0x03, 0xB5, 0x87, 0x06, 0x8A, 0x2F, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x81, 0x97, 0xF0, 0x09
    };
    ASSERT_EQ(92, data.size());
    EXPECT_EQ(0xF7B988D5, get_vector_crc(data));
}

}
