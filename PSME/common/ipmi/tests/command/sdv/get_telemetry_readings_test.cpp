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
 * @file get_telemetry_reading_test.cpp
 */

#include "ipmi/command/sdv/nm/get_telemetry_readings.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::sdv;

namespace {

constexpr const std::uint32_t PACKAGE_ID = 0x12345778;
constexpr const std::uint8_t START_READING_INDEX = 0x01;
constexpr const std::uint8_t NEXT_READING_INDEX = 0x10;

}

TEST(GetTelemetryReadingRequest, PackRequestData) {
    ipmi::IpmiInterface::ByteBuffer expected = {
        INTEL_IANA_LSB_FIRST[0],
        INTEL_IANA_LSB_FIRST[1],
        INTEL_IANA_LSB_FIRST[2],
        std::uint8_t(PACKAGE_ID), // LS byte first
        std::uint8_t(PACKAGE_ID >> 8),
        std::uint8_t(PACKAGE_ID >> 16),
        std::uint8_t(PACKAGE_ID >> 24),
        START_READING_INDEX};
    request::GetTelemetryReadings request;
    request.set_package_guid(PACKAGE_ID);
    request.set_start_reading_index(START_READING_INDEX);
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetTelemetryReadingResponse, GetReadings) {
    const response::GetTelemetryReadings::RawReadingValues expected_readings = {{0x12345678,0x87654321}};
    ipmi::IpmiInterface::ByteBuffer response_data = {
        ipmi::COMPLETION_CODE_NORMAL,
        INTEL_IANA_LSB_FIRST[0],
        INTEL_IANA_LSB_FIRST[1],
        INTEL_IANA_LSB_FIRST[2],
        NEXT_READING_INDEX
    };
    for (const auto& reading: expected_readings) {
        response_data.push_back(static_cast<uint8_t>(reading));
        response_data.push_back(static_cast<uint8_t>(reading >> 8));
        response_data.push_back(static_cast<uint8_t>(reading >> 16));
        response_data.push_back(static_cast<uint8_t>(reading >> 24));
    }
    response::GetTelemetryReadings response{};
    ASSERT_TRUE(response.get_readings().empty());
    ASSERT_EQ(0, response.get_next_reading_index());
    response.do_unpack(response_data);
    ASSERT_EQ(NEXT_READING_INDEX, response.get_next_reading_index());
    ASSERT_EQ(expected_readings, response.get_readings());
}
