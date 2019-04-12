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

#include "ipmi/command/sdv/rmm/get_fan_tachs.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;

TEST(GetFanTachsResponse, GetTachReadings) {
    ipmi::IpmiInterface::ByteBuffer data =
        {0x00, 0x0a, 0xf2, 0x0a, 0xe8, 0x0a, 0xd7, 0x0a, 0xcf, 0x0a, 0xe5, 0x0a, 0xc4};
    response::GetFanTachs response;

    response.do_unpack(data);

    ASSERT_EQ(0x0af2, response.get_tach_reading(0));
    ASSERT_EQ(0x0ae8, response.get_tach_reading(1));
    ASSERT_EQ(0x0ad7, response.get_tach_reading(2));
    ASSERT_EQ(0x0acf, response.get_tach_reading(3));
    ASSERT_EQ(0x0ae5, response.get_tach_reading(4));
    ASSERT_EQ(0x0ac4, response.get_tach_reading(5));
}

TEST(GetFanTachsResponse, GetTachReadingWithInvalidIndex) {
    response::GetFanTachs response;

    ASSERT_EQ(0, response.get_tach_reading(6));
}
