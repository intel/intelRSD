/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file to_hex_string_test.cpp
 */

#include "gtest/gtest.h"
#include "agent-framework/module/utils/to_hex_string.hpp"

using namespace agent_framework::model::utils;

class ToHexStringTest : public ::testing::Test {
public:
    virtual ~ToHexStringTest();
};

ToHexStringTest::~ToHexStringTest() {}

TEST_F(ToHexStringTest, BasicTest) {
    ASSERT_EQ(to_hex_string<1>(0x10203040u), "0x40");
    ASSERT_EQ(to_hex_string<2>(0x10203040u), "0x3040");
    ASSERT_EQ(to_hex_string<3>(0x10203040u), "0x203040");
    ASSERT_EQ(to_hex_string<4>(0x10203040u), "0x10203040");
    ASSERT_EQ((to_hex_string<3, false>(0x10203040u)), "203040");
    ASSERT_EQ(to_hex_string<2>(0x00000010u), "0x0010");
    ASSERT_EQ(to_hex_string<1>(0u), "0x00");
    ASSERT_EQ(to_hex_string<2>(0u), "0x0000");
    ASSERT_EQ(to_hex_string<3>(0u), "0x000000");
    ASSERT_EQ(to_hex_string<1>(uint8_t(0)), "0x00");
}
