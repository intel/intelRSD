/*!
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 */

#include "ipmi/command/sdv/rsd/get_mdr_data_region_status.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

static constexpr uint8_t cc_ok = 0x0;
static constexpr uint8_t intel_rsd = 0x04;
static constexpr uint8_t cc_timeout = 0xc3;
static constexpr uint8_t mdr_version = 0x4;
static constexpr uint8_t data_update_count = 0x1;
static constexpr uint16_t region_size = 146;
static constexpr uint16_t region_size_used = 100;
static constexpr uint8_t checksum = 0x86;

static constexpr DataRegionId smbios_table{DataRegionId::SMBIOS_TABLE};

static constexpr DataValidation data_valid{DataValidation::VALID};
static constexpr DataValidation data_invalid{DataValidation::INVALID};
static constexpr LockStatus unlocked{LockStatus::UNLOCKED};

class GetMdrDataRegionStatusTest: public ::testing::Test {
protected:
    ipmi::IpmiInterface::ByteBuffer mdr_valid_region_rsp = {
        cc_ok,
        intel_rsd,
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

    ipmi::IpmiInterface::ByteBuffer timeout_rsp = {
        cc_timeout
    };

    ipmi::IpmiInterface::ByteBuffer mdr_invalid_region_rsp = {
        cc_ok,
        intel_rsd,
        mdr_version,
        smbios_table,
        data_invalid,
        data_update_count,
        unlocked,
        uint8_t(region_size & 0xff),
        uint8_t((region_size >> 8) & 0xff),
        uint8_t(region_size_used & 0x00ff),
        uint8_t((region_size_used & 0xff00) >> 8),
        checksum
    };

    ipmi::IpmiInterface::ByteBuffer mdr_empty_region_rsp = {
        cc_ok,
        intel_rsd,
        mdr_version,
        smbios_table,
        data_valid,
        data_update_count,
        unlocked,
        uint8_t(region_size & 0xff),
        uint8_t((region_size >> 8) & 0xff),
        0,
        0,
        checksum
    };

    ipmi::IpmiInterface::ByteBuffer mdr_locked_region_rsp = {
        cc_ok,
        intel_rsd,
        mdr_version,
        smbios_table,
        data_valid,
        data_update_count,
        LockStatus::STRICT_LOCK,
        uint8_t(region_size & 0xff),
        uint8_t((region_size >> 8) & 0xff),
        uint8_t(region_size_used & 0x00ff),
        uint8_t((region_size_used & 0xff00) >> 8),
        checksum
    };

public:
    virtual ~GetMdrDataRegionStatusTest();
};

GetMdrDataRegionStatusTest::~GetMdrDataRegionStatusTest() { }

TEST_F(GetMdrDataRegionStatusTest, RequestSettersAndGetters) {
    rsd::request::GetMdrDataRegionStatus req{};

    DataRegionId data_region_id {DataRegionId::SMBIOS_TABLE};
    ASSERT_NO_THROW(req.set_data_region_id(data_region_id));
    ASSERT_EQ(data_region_id, req.get_data_region_id());

}

TEST_F(GetMdrDataRegionStatusTest, SmbiosRequestIsPackedCorrectly) {
    rsd::request::GetMdrDataRegionStatus req{};
    DataRegionId data_region_id {DataRegionId::SMBIOS_TABLE};

    req.set_data_region_id(data_region_id);
    const auto& data = req.do_pack();

    ASSERT_EQ(Cmd::GET_MDR_DATA_REGION_STATUS, req.get_command());
    ASSERT_EQ(NetFn::GROUP_EXTENSION, req.get_network_function());
    ASSERT_EQ(data[1], req.get_data_region_id());
}

TEST_F(GetMdrDataRegionStatusTest, IscsiRequestIsPackedCorrectly) {
    rsd::request::GetMdrDataRegionStatus req{};
    DataRegionId data_region_id {DataRegionId::ISCSI_BOOT_OPTIONS};

    req.set_data_region_id(data_region_id);
    const auto data = req.do_pack();

    ASSERT_EQ(Cmd::GET_MDR_DATA_REGION_STATUS, req.get_command());
    ASSERT_EQ(NetFn::GROUP_EXTENSION, req.get_network_function());
    ASSERT_EQ(data[0], static_cast<uint8_t>(GroupExtension::RACKSCALE));
    ASSERT_EQ(data[1], req.get_data_region_id());
}


TEST_F(GetMdrDataRegionStatusTest, ResponseIsCorrect) {

    rsd::response::GetMdrDataRegionStatus rsp{};
    rsp.do_unpack(mdr_valid_region_rsp);

    ASSERT_TRUE(rsp.is_rackscale_extension_byte_present());
    ASSERT_EQ(rsp.get_mdr_version(), mdr_version);
    ASSERT_EQ(rsp.get_data_region_id(), smbios_table);
    ASSERT_EQ(rsp.get_data_validation(), data_valid);
    ASSERT_EQ(rsp.get_data_update_count(), data_update_count);
    ASSERT_EQ(rsp.get_lock_status(), unlocked);
    ASSERT_EQ(rsp.get_region_size(), region_size);
    ASSERT_EQ(rsp.get_region_size_used(), region_size_used);
    ASSERT_EQ(rsp.get_region_checksum(), checksum);
}

TEST_F(GetMdrDataRegionStatusTest, ResponseIsIncorrectTimeout) {
    rsd::response::GetMdrDataRegionStatus rsp{};
    ASSERT_THROW(rsp.do_unpack(timeout_rsp), ipmi::TimeOutError);
}

TEST_F(GetMdrDataRegionStatusTest, MdrInvalidRegion) {
    rsd::response::GetMdrDataRegionStatus rsp{};
    rsp.do_unpack(mdr_invalid_region_rsp);

    ASSERT_EQ(DataValidation::INVALID, rsp.get_data_validation());
}

TEST_F(GetMdrDataRegionStatusTest, MdrLockedRegion) {
    rsd::response::GetMdrDataRegionStatus rsp{};
    rsp.do_unpack(mdr_locked_region_rsp);

    ASSERT_EQ(LockStatus::STRICT_LOCK, rsp.get_lock_status());
}

TEST_F(GetMdrDataRegionStatusTest, MdrEmptyRegion) {
    rsd::response::GetMdrDataRegionStatus rsp{};
    rsp.do_unpack(mdr_empty_region_rsp);

    ASSERT_EQ(0, rsp.get_region_size_used());
}
