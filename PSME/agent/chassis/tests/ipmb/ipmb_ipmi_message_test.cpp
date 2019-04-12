/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @section DESCRIPTION
 * */


#include "ipmb/ipmi_message.hpp"
#include "ipmb/utils.hpp"
#include "agent-framework/exceptions/invalid_value.hpp"

#include <gtest/gtest.h>
#include <vector>

using namespace agent::chassis::ipmb;
using namespace std;

class ChassisIpmbIpmiMessageTest: public ::testing::Test {
protected:
    ChassisIpmbIpmiMessageTest() {}
    virtual ~ChassisIpmbIpmiMessageTest();
};

ChassisIpmbIpmiMessageTest::~ChassisIpmbIpmiMessageTest() { }

TEST_F(ChassisIpmbIpmiMessageTest, all_setters_and_getters) {
    IpmiMessage im{};
    im.set_dest(1);
    ASSERT_EQ(1, im.get_dest());
    im.set_netfn(2);
    ASSERT_EQ(2, im.get_netfn());
    im.set_dlun(3);
    ASSERT_EQ(3, im.get_dlun());
    im.set_h_chcksum(4);
    ASSERT_EQ(4, im.get_h_chcksum());
    im.set_src(5);
    ASSERT_EQ(5, im.get_src());
    im.set_slun(6);
    ASSERT_EQ(6, im.get_slun());
    im.set_seq(7);
    ASSERT_EQ(7, im.get_seq());
    im.set_cmd(8);
    ASSERT_EQ(8, im.get_cmd());
    CompletionCode cc{CompletionCode::CC_OK};
    im.set_cc(cc);
    ASSERT_EQ(0, im.get_cc());
    im.set_d_chcksum(10);
    ASSERT_EQ(10, im.get_d_chcksum());
    im.add_len(100);
    ASSERT_EQ(100, im.get_len());
    im.add_len(1000);
    ASSERT_EQ(IPMI_MAX_MSG_LENGTH, im.get_len());
    im.set_to_request();
    ASSERT_TRUE(im.is_request());
    im.set_to_response();
    ASSERT_FALSE(im.is_request());
    im.print_msg();
}

TEST_F(ChassisIpmbIpmiMessageTest, data) {
    IpmiMessage im{};
    static constexpr uint8_t data[] = {0, 1, 2, 3, 4, 5};
    im.set_data(data, sizeof(data));
    ASSERT_EQ(data[2], im.get_data()[2]);
    im.print_msg();
    im.add_len(sizeof(data) + IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN);
    auto v = im.as_byte_array();
    ASSERT_EQ(data[2], v[IPMB_OFFSET_DATA + 2]);
}
