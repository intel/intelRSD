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
 * @file get_sdr_test.cpp
 */

#include "ipmi/command/generic/get_sdr.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;

namespace {

constexpr const std::uint16_t RESERVATION_ID = 0x1234;
constexpr const std::uint16_t RECORD_ID = 0x4321;
constexpr const std::uint8_t OFFSET = 0x01;
constexpr const std::uint8_t LENGTH_TO_READ = 0x05;

}

TEST(GetSdrRequest, PackRequestData) {
    ipmi::IpmiInterface::ByteBuffer expected = {
        std::uint8_t(RESERVATION_ID), // LS byte first
        std::uint8_t(RESERVATION_ID >> 8),
        std::uint8_t(RECORD_ID), // LS byte first
        std::uint8_t(RECORD_ID >> 8),
        OFFSET,
        LENGTH_TO_READ};
    request::GetSdr request;
    request.set_reservation_id(RESERVATION_ID);
    request.set_record_id(RECORD_ID);
    request.set_offset(OFFSET);
    request.set_bytes_to_read(LENGTH_TO_READ);
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetSdrRequest, PackRequestWithoutSetters) {
    ipmi::IpmiInterface::ByteBuffer expected = {
        0, 0,
        0, 0,
        0,
        request::GetSdr::SDR_HEADER_LENGTH};
    request::GetSdr request;
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetSdrResponse, GetRecordData) {
    ipmi::IpmiInterface::ByteBuffer expected_record_data{0,1,2,4,5,6};
    ipmi::IpmiInterface::ByteBuffer response_data = {
        ipmi::COMPLETION_CODE_NORMAL,
        std::uint8_t(RECORD_ID), // LS byte first
        std::uint8_t(RECORD_ID >> 8),
    };
    response_data.insert(response_data.end(), expected_record_data.cbegin(),
                                              expected_record_data.cend());
    response::GetSdr response{};
    ASSERT_TRUE(response.get_record_data().empty());
    ASSERT_EQ(0x0, response.get_next_record_id());
    response.do_unpack(response_data);
    ASSERT_EQ(RECORD_ID, response.get_next_record_id());
    ASSERT_EQ(expected_record_data, response.get_record_data());
}
