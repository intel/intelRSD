/*!
 * @brief GetPresence IPMI command tests.
 *
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
 * @file get_presence_test.cpp
 */

#include "ipmi/command/sdv/rmm/get_chassis_presence.hpp"
#include "ipmi/command/sdv/rmm/get_fan_presence.hpp"
#include "ipmi/command/sdv/rmm/get_pzone_presence.hpp"
#include "ipmi/command/sdv/rmm/get_psu_presence.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::sdv;

namespace {

constexpr const std::uint8_t PRESENCE_MASK = 0x5;

void check_is_present(response::GetPresence& response) {
    ipmi::IpmiInterface::ByteBuffer byte_vector = {0x00, PRESENCE_MASK};
    response.do_unpack(byte_vector);
    for (std::uint8_t idx = 0; idx < response.get_slot_count(); ++idx) {
        ASSERT_EQ(bool((1 << idx) & PRESENCE_MASK), response.is_present(idx));
    }
}

void check_is_present_without_unpack(const response::GetPresence& response) {
    for (std::uint8_t idx = 0; idx < response.get_slot_count(); ++idx) {
        ASSERT_FALSE(response.is_present(idx));
    }
}

void check_is_present_for_out_of_range_index(const response::GetPresence& response) {
    const auto out_of_range = std::uint8_t(response.get_slot_count());
    ASSERT_FALSE(response.is_present(out_of_range));
}

}

TEST(GetChassisPresenseRequest, PackRequest) {
    request::GetChassisPresence request;
    auto request_data = request.do_pack();
    ASSERT_TRUE(request_data.empty());
}

TEST(GetFanPresenseRequest, PackRequest) {
    request::GetFanPresence request;
    auto request_data = request.do_pack();
    ASSERT_TRUE(request_data.empty());
}

TEST(GetPzonePresenseRequest, PackRequest) {
    request::GetPzonePresence request;
    auto request_data = request.do_pack();
    ASSERT_TRUE(request_data.empty());
}

TEST(GetPsuPresenseRequest, PackRequest) {
    request::GetPsuPresence request;
    auto request_data = request.do_pack();
    ASSERT_TRUE(request_data.empty());
}

TEST(GetChassisPresenseResponse, IsPresentWithoutUnpack) {
    response::GetChassisPresence response;
    check_is_present_without_unpack(response);
}

TEST(GetFanPresenseResponse, IsPresentWithoutUnpack) {
    response::GetFanPresence response;
    check_is_present_without_unpack(response);
}

TEST(GetPzonePresenseResponse, IsPresentWithoutUnpack) {
    response::GetPzonePresence response;
    check_is_present_without_unpack(response);
}

TEST(GetChassisPresenseResponse, IsPresent) {
    response::GetChassisPresence response;
    check_is_present(response);
}

TEST(GetFanPresenseResponse, IsPresent) {
    response::GetFanPresence response;
    check_is_present(response);
}

TEST(GetPzonePresenseResponse, IsPresent) {
    response::GetPzonePresence response;
    check_is_present(response);
}

TEST(GetPsuPresenseResponse, IsPresent) {
    response::GetPsuPresence response;
    check_is_present(response);
}

TEST(GetChassisPresenseResponse, IsPresentIndexOutOfRange) {
    response::GetChassisPresence response;
    check_is_present_for_out_of_range_index(response);
}

TEST(GetFanPresenseResponse, IsPresentIndexOutOfRange) {
    response::GetFanPresence response;
    check_is_present_for_out_of_range_index(response);
}

TEST(GetPzonePresenseResponse, IsPresentIndexOutOfRange) {
    response::GetPzonePresence response;
    check_is_present_for_out_of_range_index(response);
}

TEST(GetPsuPresenseResponse, IsPresentIndexOutOfRange) {
    response::GetPsuPresence response;
    check_is_present_for_out_of_range_index(response);
}
