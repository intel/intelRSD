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

#include "ipmi/command/sdv/rmm/get_ps_dc_current.hpp"
#include <gtest/gtest.h>
#include <tuple>

using namespace ipmi::command::sdv;

namespace {

constexpr const std::uint8_t PSU_ID = 2;

// X = Y * 2^N
// X - expected real value (double)
// Y, N from raw reading in linear format
const std::vector<std::tuple<ipmi::IpmiInterface::ByteBuffer, double>> expected_power_values = {
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x00, 0x00}, 0.0),  // N=0  Y=0
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x01, 0x00}, 1.0),  // N=0  Y=1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x01, 0xf0}, 0.25), // N=-2 Y=1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x01, 0xf8}, 0.5),  // N=-1 Y=1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xfc, 0xff}, -2.0), // N=-1 Y=-4
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xff, 0xff}, -0.5),  // N=-1 Y=-1
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xff, 0x7B}, 33521664.0),  // N=15 Y=1023
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x00, 0x7C}, -33554432.0),  // N=15 Y=-1024
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0xff, 0x83}, 0.0156097412109375),  // N=-16 Y=1023
    std::make_tuple<ipmi::IpmiInterface::ByteBuffer, double>({0x00, 0x00, 0x84}, -0.015625)  // N=-16 Y=-1024
};

}

TEST(GetPsDcCurrentRequest, PackRequestData) {
    request::GetPsDcCurrent request;
    request.set_psu_id(PSU_ID);
    const auto req_data = request.do_pack();
    ASSERT_EQ(1, req_data.size());
    ASSERT_EQ(PSU_ID, req_data[0]);
}

TEST(GetPsDcCurrentResponse, GetCurrentValueWithoutUnpack) {
    response::GetPsDcCurrent response;
    ASSERT_DOUBLE_EQ(0.0, response.get_power());
}

TEST(GetPsDcCurrentResponse, GetCurrentValue) {
    response::GetPsDcCurrent response;

    for (const auto& expected: expected_power_values) {
        const auto& bytes = std::get<0>(expected);
        response.do_unpack(bytes);
        ASSERT_DOUBLE_EQ(std::get<1>(expected), response.get_current());
    }
}
