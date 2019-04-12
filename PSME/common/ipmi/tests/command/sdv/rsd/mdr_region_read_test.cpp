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
 * @file ipmi/command/sdv/rsd/mdr_region_read_test.cpp
 */

#include "ipmi/command/sdv/rsd/mdr_region_read.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

static constexpr uint8_t cc_ok = 0x0;
static constexpr uint8_t cc_used_by_another_user = 0x81;
static constexpr uint8_t cc_request_too_large = 0xc4;
static constexpr uint8_t cc_request_too_greedy = 0xc7;
static constexpr uint8_t cc_invalid_region = 0xc9;
static constexpr uint8_t intel_rsd = 0x04;
static constexpr uint8_t cc_timeout = 0xc3;
static constexpr uint8_t mdr_version = 0x4;
static constexpr uint16_t offset_to_read = 0x1543;
static constexpr uint8_t data_length = 0x02;
static constexpr uint8_t data_update_count = 0x21;
static constexpr uint8_t region_size = 0x86;
static constexpr uint8_t checksum = 0x86;


static constexpr DataRegionId smbios_table{DataRegionId::SMBIOS_TABLE};
static constexpr DataRegionId iscsi_table{DataRegionId::ISCSI_BOOT_OPTIONS};


class MdrRegionReadTest: public ::testing::Test {

protected:
    ipmi::IpmiInterface::ByteBuffer valid_response = {
        cc_ok,
        intel_rsd,
        data_length,
        data_update_count,
        0x1F,
        0x35

    };
    MdrRegionReadTest();
    virtual void SetUp();
    virtual void TearDown();
    virtual ~MdrRegionReadTest();
};

MdrRegionReadTest::MdrRegionReadTest() { }
void MdrRegionReadTest::SetUp() { }
void MdrRegionReadTest::TearDown() { }
MdrRegionReadTest::~MdrRegionReadTest() { }


TEST_F(MdrRegionReadTest, ReqHasCorrectDefaultValues) {
    rsd::request::MdrRegionRead req{};
    ASSERT_EQ(req.get_network_function(), NetFn::GROUP_EXTENSION);
    ASSERT_STREQ("MdrRegionRead", req.get_command_name());
    ASSERT_EQ(req.get_command(), Cmd::MDR_REGION_READ);
}

TEST_F(MdrRegionReadTest, RequestAllGettersAndSetters) {
    rsd::request::MdrRegionRead req{};
    ASSERT_NO_THROW(req.set_data_region_id(smbios_table));
    ASSERT_NO_THROW(req.set_data_length(data_length));
    ASSERT_NO_THROW(req.set_offset(offset_to_read));
    ASSERT_EQ(smbios_table, req.get_data_region_id());
    ASSERT_EQ(data_length, req.get_data_length());
    ASSERT_EQ(offset_to_read, req.get_offset());
}

TEST_F(MdrRegionReadTest, RequestPacksCorrectly) {
    rsd::request::MdrRegionRead req{};
    req.set_data_region_id(DataRegionId::SMBIOS_TABLE);
    req.set_data_length(data_length);
    req.set_offset(offset_to_read);
    const auto& data = req.do_pack();
    ASSERT_EQ(static_cast<uint8_t>(GroupExtension::RACKSCALE), data[0]);
    ASSERT_EQ(DataRegionId::SMBIOS_TABLE, data[1]);
    ASSERT_EQ(data_length, data[2]);
    ASSERT_EQ(uint8_t(offset_to_read & 0xff), data[3]);
    ASSERT_EQ(uint8_t(offset_to_read >> 8) & 0xff, data[4]);
}
