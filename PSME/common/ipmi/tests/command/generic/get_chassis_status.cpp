/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "ipmi/command/generic/get_chassis_status.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;

/*
 * IPMI specification v2-rev1 defines Get Chassis Status command as a 4/5 byte long. The last byte is optional.
 * This tests verify proper deserialization of IPMI Get Chassis Status command for 4 and 5 bytes.
 */
TEST(GetChassisStatusResponseUnpack, Unpack5Bytes) {
    // Example of 5 byte long command starting from completion code.
    vector<uint8_t> byte_vector = {0x00, 0x21, 0x10, 0x40, 0x30};
    response::GetChassisStatus status;

    status.do_unpack(byte_vector);

    ASSERT_TRUE(status.is_power_on());

}

TEST(GetChassisStatusResponseUnpack, Unpack4Bytes) {
    // Example of 4 byte long command starting from completion code.
    vector<uint8_t> byte_vector = {0x00, 0x21, 0x10, 0x40};
    response::GetChassisStatus status;

    status.do_unpack(byte_vector);

    ASSERT_TRUE(status.is_power_on());
}

TEST(GetChassisStatusResponseUnpack, NonZeroCompletionCode) {
    vector<uint8_t> byte_vector = {0xD5};
    response::GetChassisStatus status;

    ASSERT_THROW(status.do_unpack(byte_vector), ipmi::ResponseError);
}

TEST(GetChassisStatusResponseUnpack, TooShortResponseBody) {
    vector<uint8_t> byte_vector = {0x00, 0x00};
    response::GetChassisStatus status;

    ASSERT_THROW(status.do_unpack(byte_vector), ipmi::ResponseLengthError);
}
