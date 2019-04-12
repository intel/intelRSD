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

#include "ipmi/command/sdv/nm/get_cpu_and_memory_temperature.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::sdv;

namespace {
constexpr const std::uint8_t CPU_MASK = 0x03;
constexpr const std::uint16_t MEMORY_MASK = 0xFFFF;

}

TEST(GetCpuAndMemoryTemperatureRequest, PackRequestData) {
    ipmi::IpmiInterface::ByteBuffer expected = {
        INTEL_IANA_LSB_FIRST[0],
        INTEL_IANA_LSB_FIRST[1],
        INTEL_IANA_LSB_FIRST[2],
        std::uint8_t(CPU_MASK),
        std::uint8_t(0xFF),
        std::uint8_t(0xFF),
        std::uint8_t(0xFF),
        std::uint8_t(0xFF),
        std::uint8_t(0x00),
        std::uint8_t(0x00),
        std::uint8_t(0x00),
        std::uint8_t(0x00)};
    request::GetCpuAndMemoryTemperature request;
    request.set_cpu_mask(request::CPU0 | request::CPU1);
    request.select_cpu_set(request::CPU0_TO_3);
    request.set_memory_channel_mask_for_cpu(0, MEMORY_MASK);
    request.set_memory_channel_mask_for_cpu(1, MEMORY_MASK);
    const auto req_data = request.do_pack();
    ASSERT_EQ(expected, req_data);
}

TEST(GetCpuAndMemoryTemperatureResponse, GetReadings) {
    const response::GetCpuAndMemoryTemperature::RawReadingValues expected_readings = {{0x12,0x34,0x56,0x78}};
    ipmi::IpmiInterface::ByteBuffer response_data = {
        ipmi::COMPLETION_CODE_NORMAL,
        INTEL_IANA_LSB_FIRST[0],
        INTEL_IANA_LSB_FIRST[1],
        INTEL_IANA_LSB_FIRST[2]
    };
    for (const auto& reading: expected_readings) {
        response_data.push_back(static_cast<uint8_t>(reading));
    }
    response::GetCpuAndMemoryTemperature response{};
    ASSERT_TRUE(response.get_readings().empty());
    response.do_unpack(response_data);
    ASSERT_EQ(expected_readings, response.get_readings());
}
