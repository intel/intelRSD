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
 * @file tests/byte_buffer.cpp
 */

#include "utils/byte_buffer.hpp"

#include "gtest/gtest.h"

using namespace utils;

namespace testing {

TEST(ByteBufferTests, LittleEndianGets) {
    ByteBuffer buf(10);
    EXPECT_EQ(buf.size(), 10);
    // 01 02 03 04 05 06 07 08 09 0A
    for (unsigned i = 0; i < 10; ++i) {
        buf[i] = uint8_t(i + 1);
    }
    EXPECT_EQ(buf.get_le16(1), 0x0302);
    EXPECT_EQ(buf.get_le16(3), 0x0504);
    EXPECT_EQ(buf.get_le16(5), 0x0706);
    EXPECT_EQ(buf.get_le16(7), 0x0908);
    EXPECT_EQ(buf.get_le32(1), 0x05040302);
    EXPECT_EQ(buf.get_le32(5), 0x09080706);
    EXPECT_EQ(buf.get_le48(3), 0x090807060504);
    EXPECT_EQ(buf.get_le48(2), 0x080706050403);
    EXPECT_EQ(buf.get_le48(1), 0x070605040302);
    EXPECT_EQ(buf.get_le64(1), 0x0908070605040302);
}

TEST(ByteBufferTests, BigEndianGets) {
    ByteBuffer buf(10);
    EXPECT_EQ(buf.size(), 10);
    // 01 02 03 04 05 06 07 08 09 0A
    for (unsigned i = 0; i < 10; ++i) {
        buf[i] = uint8_t(i + 1);
    }
    EXPECT_EQ(buf.get_be16(1), 0x0203);
    EXPECT_EQ(buf.get_be16(3), 0x0405);
    EXPECT_EQ(buf.get_be16(5), 0x0607);
    EXPECT_EQ(buf.get_be16(7), 0x0809);
    EXPECT_EQ(buf.get_be32(1), 0x02030405);
    EXPECT_EQ(buf.get_be32(5), 0x06070809);
    EXPECT_EQ(buf.get_be48(1), 0x020304050607);
    EXPECT_EQ(buf.get_be48(2), 0x030405060708);
    EXPECT_EQ(buf.get_be48(3), 0x040506070809);
    EXPECT_EQ(buf.get_be64(1), 0x0203040506070809);
}

TEST(ByteBufferTests, LittleEndianSets) {
    ByteBuffer buf(10);
    EXPECT_EQ(buf.size(), 10);

    buf.set_le16(4, 0x1234);
    EXPECT_EQ(buf[4], 0x34);
    EXPECT_EQ(buf[5], 0x12);

    buf = ByteBuffer(10);
    buf.set_le32(3, 0x12345678);
    EXPECT_EQ(buf[3], 0x78);
    EXPECT_EQ(buf[4], 0x56);
    EXPECT_EQ(buf[5], 0x34);
    EXPECT_EQ(buf[6], 0x12);

    buf = ByteBuffer(10);
    buf.set_le48(2, 0x123456789ABC);
    EXPECT_EQ(buf[2], 0xBC);
    EXPECT_EQ(buf[3], 0x9A);
    EXPECT_EQ(buf[4], 0x78);
    EXPECT_EQ(buf[5], 0x56);
    EXPECT_EQ(buf[6], 0x34);
    EXPECT_EQ(buf[7], 0x12);

    buf = ByteBuffer(10);
    buf.set_le64(2, 0x0123456789ABCDEF);
    EXPECT_EQ(buf[2], 0xEF);
    EXPECT_EQ(buf[3], 0xCD);
    EXPECT_EQ(buf[4], 0xAB);
    EXPECT_EQ(buf[5], 0x89);
    EXPECT_EQ(buf[6], 0x67);
    EXPECT_EQ(buf[7], 0x45);
    EXPECT_EQ(buf[8], 0x23);
    EXPECT_EQ(buf[9], 0x01);
}

TEST(ByteBufferTests, BigEndianSets) {
    ByteBuffer buf(10);
    EXPECT_EQ(buf.size(), 10);

    buf = ByteBuffer(10);
    buf.set_be16(4, 0x1234);
    EXPECT_EQ(buf[5], 0x34);
    EXPECT_EQ(buf[4], 0x12);

    buf = ByteBuffer(10);
    buf.set_be32(3, 0x12345678);
    EXPECT_EQ(buf[6], 0x78);
    EXPECT_EQ(buf[5], 0x56);
    EXPECT_EQ(buf[4], 0x34);
    EXPECT_EQ(buf[3], 0x12);

    buf = ByteBuffer(10);
    buf.set_be48(2, 0x123456789ABC);
    EXPECT_EQ(buf[7], 0xBC);
    EXPECT_EQ(buf[6], 0x9A);
    EXPECT_EQ(buf[5], 0x78);
    EXPECT_EQ(buf[4], 0x56);
    EXPECT_EQ(buf[3], 0x34);
    EXPECT_EQ(buf[2], 0x12);

    buf = ByteBuffer(10);
    buf.set_be64(2, 0x0123456789ABCDEF);
    EXPECT_EQ(buf[9], 0xEF);
    EXPECT_EQ(buf[8], 0xCD);
    EXPECT_EQ(buf[7], 0xAB);
    EXPECT_EQ(buf[6], 0x89);
    EXPECT_EQ(buf[5], 0x67);
    EXPECT_EQ(buf[4], 0x45);
    EXPECT_EQ(buf[3], 0x23);
    EXPECT_EQ(buf[2], 0x01);
}

}
