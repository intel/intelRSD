/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;

static constexpr uint8_t cc_ok = 0x0;
static constexpr uint8_t cc_timeout = 0xc3;
static constexpr uint8_t mdr_version = 0x4;
static constexpr uint8_t data_update_count = 0x1;
static constexpr uint16_t region_size = 146;
static constexpr uint16_t region_size_used = 100;
static constexpr uint8_t checksum = 0x86;

static constexpr DataRegionId smbios_table{DataRegionId::SMBIOS_TABLE};

static constexpr response::GetMdrDataRegionStatus::DataValidation data_valid{
        response::GetMdrDataRegionStatus::DataValidation::VALID
};

static constexpr response::GetMdrDataRegionStatus::LockStatus unlocked{
        response::GetMdrDataRegionStatus::LockStatus::UNLOCKED
};

class GetMdrDataRegionsStatusTest: public ::testing::Test {
protected:
    std::vector<uint8_t> rsp_vec1 {
            cc_ok,
            mdr_version,
            smbios_table,
            data_valid,
            data_update_count,
            unlocked,
            uint8_t(region_size & 0xff),
            uint8_t((region_size >> 8) & 0xff),
            uint8_t(region_size_used & 0x00ff),
            uint8_t((region_size_used & 0xff00) >> 8),
            checksum
    };

    std::vector<uint8_t> rsp_vec2 {
            cc_timeout
    };

    void SetUp() {

    }

public:
    virtual ~GetMdrDataRegionsStatusTest();
};

GetMdrDataRegionsStatusTest::~GetMdrDataRegionsStatusTest() { }

TEST_F(GetMdrDataRegionsStatusTest, RequestIsPackedCorrectly) {
    request::GetMdrDataRegionStatus req{};
    std::vector<std::uint8_t> data{};
    DataRegionId data_region_id {DataRegionId::SMBIOS_TABLE};

    req.set_data_region_id(data_region_id);
    req.pack(data);

    ASSERT_EQ(data[0], req.get_data_region_id());
}

TEST_F(GetMdrDataRegionsStatusTest, ResponseIsCorrect) {

    response::GetMdrDataRegionStatus rsp{};
    rsp.unpack(rsp_vec1);

    ASSERT_EQ(rsp.get_completion_code(), cc_ok);
    ASSERT_EQ(rsp.get_mdr_version(), mdr_version);
    ASSERT_EQ(rsp.get_data_region_id(), smbios_table);
    ASSERT_EQ(rsp.get_data_validation(), data_valid);
    ASSERT_EQ(rsp.get_data_update_count(), data_update_count);
    ASSERT_EQ(rsp.get_lock_status(), unlocked);
    ASSERT_EQ(rsp.get_region_size(), region_size);
    ASSERT_EQ(rsp.get_region_size_used(), region_size_used);
    ASSERT_EQ(rsp.get_region_checksum(), checksum);
}

TEST_F(GetMdrDataRegionsStatusTest, ResponseIsIncorrectTimeout) {
    response::GetMdrDataRegionStatus rsp{};
    rsp.unpack(rsp_vec2);

    ASSERT_EQ(rsp.get_completion_code(), cc_timeout);
}
