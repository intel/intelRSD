/*!
 * @brief Test for the MDR write command.
 *
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
 */

#include "ipmi/command/sdv/rsd/mdr_region_write.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

static constexpr uint8_t cc_ok = 0x0;
static constexpr uint8_t cc_used_by_another_user = 0x81;
static constexpr uint8_t cc_region_not_locked = 0xd5;
static constexpr uint8_t cc_request_too_greedy = 0xc7;
static constexpr uint8_t cc_invalid_region = 0xc9;
static constexpr uint8_t intel_rsd = 0x04;
static constexpr uint8_t mdr_version = 0x4;
static constexpr uint8_t data_length = 0x02;
static constexpr uint8_t data_valid = 0x00;
static constexpr uint8_t strict_lock = 0x01;
static constexpr uint8_t lock_handle = 0x00;
static constexpr uint16_t data_offset = 0x0034;
static constexpr uint16_t max_length = 0x1234;
static constexpr uint16_t size_used = 0x5678;


static constexpr DataRegionId smbios_table{DataRegionId::SMBIOS_TABLE};
static constexpr DataRegionId iscsi_table{DataRegionId::ISCSI_BOOT_OPTIONS};


class MdrRegionWriteTest: public ::testing::Test {

protected:
    ipmi::IpmiInterface::ByteBuffer valid_response = {
        cc_ok,
        intel_rsd,
        smbios_table,
        data_valid,
        strict_lock,
        uint8_t(max_length & 0xFF),
        uint8_t(max_length >> 8 & 0xFF),
        uint8_t(size_used & 0xFF),
        uint8_t(size_used >> 8 & 0xFF)
    };

    MdrRegionWriteTest();
    virtual void SetUp();
    virtual void TearDown();
    virtual ~MdrRegionWriteTest();
};

MdrRegionWriteTest::MdrRegionWriteTest() { }
void MdrRegionWriteTest::SetUp() { }
void MdrRegionWriteTest::TearDown() { }
MdrRegionWriteTest::~MdrRegionWriteTest() { }


TEST_F(MdrRegionWriteTest, ReqHasCorrectDefaultValues) {
    rsd::request::MdrRegionWrite req{};
    ASSERT_EQ(req.get_network_function(), NetFn::GROUP_EXTENSION);
    ASSERT_STREQ("MdrRegionWrite", req.get_command_name());
    ASSERT_EQ(req.get_command(), Cmd::MDR_REGION_WRITE);
}

TEST_F(MdrRegionWriteTest, RequestAllGettersAndSetters) {
    rsd::request::MdrRegionWrite req{};
    ASSERT_NO_THROW(req.set_data_region_id(smbios_table));
    ASSERT_NO_THROW(req.set_data_length(data_length));
    ASSERT_NO_THROW(req.set_data_offset(data_offset));
    ASSERT_NO_THROW(req.set_session_lock_handle(lock_handle));
    ASSERT_EQ(smbios_table, req.get_data_region_id());
    ASSERT_EQ(data_length, req.get_data_length());
    ASSERT_EQ(data_offset, req.get_data_offset());
    ASSERT_EQ(lock_handle, req.get_session_lock_handle());
}

TEST_F(MdrRegionWriteTest, RequestPacksCorrectly) {
    rsd::request::MdrRegionWrite req{};
    req.set_data_region_id(smbios_table);
    req.set_data_length(data_length);
    req.set_data_offset(data_offset);
    req.set_session_lock_handle(lock_handle);
    ipmi::IpmiInterface::ByteBuffer data = {
        0x01, 0x02, 0x03, 0x04
    };

    req.append_data(data.cbegin(), data.cend());
    const auto& message = req.do_pack();
    ASSERT_EQ(static_cast<uint8_t>(GroupExtension::RACKSCALE), message[0]);
    ASSERT_EQ(lock_handle, message[rsd::request::MdrRegionWrite::HANDLE_OFFSET]);
    ASSERT_EQ(smbios_table, message[rsd::request::MdrRegionWrite::DATA_REGION_ID_OFFSET]);
    ASSERT_EQ(data_length, message[rsd::request::MdrRegionWrite::DATA_LENGTH_OFFSET]);
    ASSERT_EQ(uint8_t(data_offset & 0xff), message[rsd::request::MdrRegionWrite::WHERE_TO_WRITE_OFFSET]);
    ASSERT_EQ(uint8_t(data_offset >> 8) & 0xff, message[rsd::request::MdrRegionWrite::WHERE_TO_WRITE_OFFSET + 1]);
    const auto* packed =  &message[rsd::request::MdrRegionWrite::WHAT_TO_WRITE_OFFSET];
    for (const auto& val : data) {
        ASSERT_EQ(val, *(packed++));
    }
}

TEST_F(MdrRegionWriteTest, ResponseUnpacksCorrectly) {
    rsd::response::MdrRegionWrite rsp{};
    rsp.do_unpack(valid_response);
    ASSERT_EQ(smbios_table, rsp.get_data_region_id());
    ASSERT_EQ(data_valid, rsp.get_valid_data());
    ASSERT_EQ(max_length, rsp.get_max_region_length());
    ASSERT_EQ(size_used, rsp.get_size_of_region_used());
    ASSERT_STREQ("MdrRegionWrite", rsp.get_command_name());
}
