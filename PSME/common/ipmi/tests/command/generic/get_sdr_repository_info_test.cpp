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
 * @file get_sdr_repository_info_test.cpp
 */

#include "ipmi/command/generic/get_sdr_repository_info.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;

namespace {

constexpr const std::uint16_t RECORD_COUNT = 0x8900;
constexpr const std::uint16_t FREE_SPACE = 0x1007;
constexpr const std::uint32_t LAST_ADDITION_TIMESTAMP = 0x01020304;
constexpr const std::uint32_t LAST_DELETE_TIMESTAMP = 0x05060708;
// 00101111 = overflow not set, non-modal sdr repo, supports: get allocation, reserve SDR, partial add, delete sdr
constexpr const std::uint8_t SUPPORTED_OPERATIONS = 0x2F;

}

TEST(GetSdrRepositoryInfoRequest, PackRequestData) {
    request::GetSdrRepositoryInfo request{};
    const auto req_data = request.do_pack();
    ASSERT_TRUE(req_data.empty());
}


TEST(GetSdrRepositoryInfoResponse, GetSdrRepositoryInfoUnpack) {
    ipmi::IpmiInterface::ByteBuffer data{
        ipmi::COMPLETION_CODE_NORMAL,
        response::GetSdrRepositoryInfo::IPMI2_SDR_VERSION,
        std::uint8_t(RECORD_COUNT), // LS byte first
        std::uint8_t(RECORD_COUNT >> 8),
        std::uint8_t(FREE_SPACE), // LS byte first
        std::uint8_t(FREE_SPACE >> 8),
        std::uint8_t(LAST_ADDITION_TIMESTAMP), // LS byte first
        std::uint8_t(LAST_ADDITION_TIMESTAMP >> 8),
        std::uint8_t(LAST_ADDITION_TIMESTAMP >> 16),
        std::uint8_t(LAST_ADDITION_TIMESTAMP >> 24),
        std::uint8_t(LAST_DELETE_TIMESTAMP), // LS byte first
        std::uint8_t(LAST_DELETE_TIMESTAMP >> 8),
        std::uint8_t(LAST_DELETE_TIMESTAMP >> 16),
        std::uint8_t(LAST_DELETE_TIMESTAMP >> 24),
        SUPPORTED_OPERATIONS
    };
    response::GetSdrRepositoryInfo response;
    ASSERT_EQ(0, response.get_sdr_version());
    ASSERT_EQ(0, response.get_record_count());
    ASSERT_EQ(0, response.get_free_space());
    ASSERT_EQ(0, response.get_free_space());
    ASSERT_FALSE(response.is_overflow_flag_set());
    ASSERT_EQ(response::GetSdrRepositoryInfo::UpdateMode::UNSPECIFIED, response.get_supported_update_mode());
    ASSERT_FALSE(response.is_delete_sdr_supported());
    ASSERT_FALSE(response.is_partial_add_supported());
    ASSERT_FALSE(response.is_reserve_sdr_supported());
    ASSERT_FALSE(response.is_get_sdr_allocation_supported());
    response.do_unpack(data);
    ASSERT_EQ(response::GetSdrRepositoryInfo::IPMI2_SDR_VERSION, response.get_sdr_version());
    ASSERT_EQ(RECORD_COUNT, response.get_record_count());
    ASSERT_EQ(LAST_ADDITION_TIMESTAMP, response.get_last_addition_timestamp());
    ASSERT_EQ(LAST_DELETE_TIMESTAMP, response.get_last_delete_timestamp());
    ASSERT_FALSE(response.is_overflow_flag_set());
    ASSERT_EQ(response::GetSdrRepositoryInfo::UpdateMode::NON_MODAL, response.get_supported_update_mode());
    ASSERT_TRUE(response.is_delete_sdr_supported());
    ASSERT_TRUE(response.is_partial_add_supported());
    ASSERT_TRUE(response.is_reserve_sdr_supported());
    ASSERT_TRUE(response.is_get_sdr_allocation_supported());
}
