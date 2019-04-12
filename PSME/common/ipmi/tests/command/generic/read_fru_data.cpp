/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "ipmi/command/generic/read_fru_data.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;


TEST(ReadFruData, Pack) {
    request::ReadFruData req{};
    req.set_device_id(0x12);
    req.set_offset(0x3456);
    req.set_bytes_to_read(0x78);
    auto data = req.do_pack();
    ASSERT_EQ(4, data.size());
    ASSERT_EQ(0x12, data[0]);
    ASSERT_EQ(0x56, data[1]);
    ASSERT_EQ(0x34, data[2]);
    ASSERT_EQ(0x78, data[3]);
}

TEST(ReadFruData, UnpackEmpty) {
    ipmi::IpmiInterface::ByteBuffer data = {
        ipmi::COMPLETION_CODE_NORMAL,
        0x0
    };
    response::ReadFruData response;
    response.do_unpack(data);

    ASSERT_TRUE(response.get_data().empty());
}

TEST(ReadFruData, UnpackData) {
    ipmi::IpmiInterface::ByteBuffer data = {
        ipmi::COMPLETION_CODE_NORMAL,
        0x2,
        0x11,
        0x22
    };
    response::ReadFruData response;
    response.do_unpack(data);
    auto read_data = response.get_data();
    ASSERT_EQ(2, read_data.size());
    ASSERT_EQ(0x11, read_data[0]);
    ASSERT_EQ(0x22, read_data[1]);
}

TEST(ReadFruData, UnpackDataWrongSize) {
    ipmi::IpmiInterface::ByteBuffer data = {
        ipmi::COMPLETION_CODE_NORMAL,
        0x2,
        0x11,
        0x22,
        0x33
    };
    response::ReadFruData response;
    response.do_unpack(data);
    auto read_data = response.get_data();
    ASSERT_EQ(2, read_data.size());
    ASSERT_EQ(0x11, read_data[0]);
    ASSERT_EQ(0x22, read_data[1]);
}
