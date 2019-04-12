/*!
 * @brief Test for the mdr_region_lock
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
 * @file mdr_region_lock_test.cpp
 */

#include "ipmi/command/sdv/rsd/mdr_region_lock.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

static constexpr uint8_t cc_ok = 0x0;
static constexpr uint8_t cc_used_by_another_user = 0x81;
static constexpr uint8_t cc_region_locked = 0xd5;
static constexpr uint8_t cc_invalid_region = 0xc9;
static constexpr uint8_t intel_rsd = 0x04;
static constexpr uint8_t mdr_handle = 0x01;
static constexpr uint8_t mdr_region = 0x04;
static constexpr uint8_t mdr_lock_type = 0x01;
static constexpr uint16_t timeout = 0x1234;

static constexpr DataRegionId smbios_table{DataRegionId::SMBIOS_TABLE};
static constexpr DataRegionId iscsi_table{DataRegionId::ISCSI_BOOT_OPTIONS};
static constexpr LockType lock_type{LockType::STRICK_LOCK};

class MdrRegionLockTest: public ::testing::Test {

protected:
    ipmi::IpmiInterface::ByteBuffer valid_response = {
        cc_ok,
        intel_rsd,
        mdr_handle
    };

    MdrRegionLockTest();
    virtual void SetUp();
    virtual void TearDown();
    virtual ~MdrRegionLockTest();
};

MdrRegionLockTest::MdrRegionLockTest() {
}
void MdrRegionLockTest::SetUp() {
}
void MdrRegionLockTest::TearDown() {
}
MdrRegionLockTest::~MdrRegionLockTest() {
}

TEST_F(MdrRegionLockTest, ReqHasCorrectDefaultValues) {
    rsd::request::MdrRegionLock req{};
    ASSERT_EQ(req.get_network_function(), NetFn::GROUP_EXTENSION);
    ASSERT_STREQ("MdrRegionLock", req.get_command_name());
    ASSERT_EQ(req.get_command(), Cmd::MDR_REGION_LOCK);
}

TEST_F(MdrRegionLockTest, RequestAllGettersAndSetters) {
    rsd::request::MdrRegionLock req{};
    ASSERT_NO_THROW(req.set_data_region_id(iscsi_table));
    ASSERT_NO_THROW(req.set_lock_type(lock_type));
    ASSERT_NO_THROW(req.set_timeout(timeout));
    ASSERT_NO_THROW(req.set_session_lock_handle(mdr_handle));
    ASSERT_EQ(iscsi_table, req.get_data_region_id());
    ASSERT_EQ(lock_type, req.get_lock_type());
    ASSERT_EQ(timeout, req.get_timeout());
    ASSERT_EQ(mdr_handle, req.get_session_lock_handle());
}

TEST_F(MdrRegionLockTest, RequestPacksCorrectly) {
    rsd::request::MdrRegionLock req{};
    req.set_data_region_id(smbios_table);
    req.set_lock_type(lock_type);
    req.set_timeout(timeout);
    req.set_session_lock_handle(mdr_handle);
    const auto& data = req.do_pack();
    ASSERT_EQ(static_cast<uint8_t>(GroupExtension::RACKSCALE), data[0]);
    ASSERT_EQ(smbios_table, data[rsd::request::MdrRegionLock::DATA_REGION_OFFSET]);
    ASSERT_EQ(mdr_handle, data[rsd::request::MdrRegionLock::SESSION_LOCK_HANDLE_OFFSET]);
    ASSERT_EQ(mdr_lock_type, data[rsd::request::MdrRegionLock::LOCK_TYPE_OFFSET]);
    ASSERT_EQ(uint8_t(timeout & 0xff), data[rsd::request::MdrRegionLock::TIMEOUT_OFFSET]);
    ASSERT_EQ(uint8_t(timeout >> 8) & 0xff, data[rsd::request::MdrRegionLock::TIMEOUT_OFFSET + 1]);
}

TEST_F(MdrRegionLockTest, ResponseUnpacksCorrectly) {
    rsd::response::MdrRegionLock rsp{};
    rsp.do_unpack(valid_response);
    ASSERT_EQ(mdr_handle, rsp.get_session_lock_handle());
}
