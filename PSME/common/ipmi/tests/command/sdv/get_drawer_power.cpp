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

#include "ipmi/command/sdv/rmm/get_drawer_power.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::sdv;

TEST(GetDrawerPower, GetDrawerPowerEmpty) {
    ipmi::IpmiInterface::ByteBuffer byte_vector = {0x00, 0x00};
    response::GetDrawerPower response;

    response.do_unpack(byte_vector);

    ASSERT_EQ(0x00, response.get_slot_count());
    ASSERT_EQ(false, response.is_valid_value_on_slot(0));
    ASSERT_EQ(false, response.is_valid_value_on_slot(1));
    ASSERT_EQ(false, response.is_valid_value_on_slot(2));
    ASSERT_EQ(false, response.is_valid_value_on_slot(3));
    ASSERT_EQ(false, response.is_valid_value_on_slot(4));
}

TEST(GetDrawerPower, GetDrawerPowerSingleValue) {
    ipmi::IpmiInterface::ByteBuffer byte_vector = {0x00, 0x01, 0x00, 0x10};
    response::GetDrawerPower response;

    response.do_unpack(byte_vector);

    ASSERT_EQ(0x01, response.get_slot_count());
    ASSERT_EQ(true, response.is_valid_value_on_slot(0));
    ASSERT_EQ(false, response.is_valid_value_on_slot(1));
    ASSERT_EQ(false, response.is_valid_value_on_slot(2));
    ASSERT_EQ(false, response.is_valid_value_on_slot(3));
    ASSERT_EQ(false, response.is_valid_value_on_slot(4));
    ASSERT_EQ(0x0010, response.get_power_on_slot(0));
}

TEST(GetDrawerPower, GetDrawerPowerMultiValue) {
    ipmi::IpmiInterface::ByteBuffer byte_vector = {0x00, 0x07, 0x10, 0x20, 0xFF, 0xFF, 0x30, 0x40};
    response::GetDrawerPower response;

    response.do_unpack(byte_vector);

    ASSERT_EQ(0x03, response.get_slot_count());
    ASSERT_EQ(true, response.is_valid_value_on_slot(0));
    ASSERT_EQ(false, response.is_valid_value_on_slot(1));
    ASSERT_EQ(true, response.is_valid_value_on_slot(2));
    ASSERT_EQ(false, response.is_valid_value_on_slot(3));
    ASSERT_EQ(false, response.is_valid_value_on_slot(4));
    ASSERT_EQ(0x1020, response.get_power_on_slot(0));
    ASSERT_EQ(0x3040, response.get_power_on_slot(2));
}
