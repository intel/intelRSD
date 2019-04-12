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
 */


#include "ipmi/command/sdv/rsd/mdr_region_update_complete.hpp"
#include <gtest/gtest.h>

using namespace ipmi::command::sdv;
using namespace ipmi::command::sdv::rsd;

static constexpr DataRegionId smbios_table{DataRegionId::SMBIOS_TABLE};

class MdrRegionUpdateCompleteTest: public ::testing::Test {

protected:
    ipmi::IpmiInterface::ByteBuffer valid_response = {
        0x00,
        0x04
    };
    MdrRegionUpdateCompleteTest();
    virtual void SetUp();
    virtual void TearDown();
    virtual ~MdrRegionUpdateCompleteTest();
};

MdrRegionUpdateCompleteTest::MdrRegionUpdateCompleteTest() { }
void MdrRegionUpdateCompleteTest::SetUp() { }
void MdrRegionUpdateCompleteTest::TearDown() { }
MdrRegionUpdateCompleteTest::~MdrRegionUpdateCompleteTest() { }


TEST_F(MdrRegionUpdateCompleteTest, ReqHasCorrectDefaultValues) {
    request::MdrRegionUpdateComplete req{};
    ASSERT_EQ(req.get_network_function(), NetFn::GROUP_EXTENSION);
    ASSERT_STREQ("MdrRegionUpdateComplete", req.get_command_name());
    ASSERT_EQ(req.get_command(), Cmd::MDR_REGION_UPDATE_COMPLETE);
}

TEST_F(MdrRegionUpdateCompleteTest, RequestAllGettersAndSetters) {
    request::MdrRegionUpdateComplete req{};
    ASSERT_NO_THROW(req.set_data_region_id(smbios_table));
    ASSERT_NO_THROW(req.set_session_lock_handle(0x12));
    ASSERT_EQ(smbios_table, req.get_data_region_id());
    ASSERT_EQ(0x12, req.get_session_lock_handle());
}

TEST_F(MdrRegionUpdateCompleteTest, RequestPacksCorrectly) {
    request::MdrRegionUpdateComplete req{};
    req.set_data_region_id(DataRegionId::SMBIOS_TABLE);
    req.set_session_lock_handle(0x12);
    const auto& data = req.do_pack();
    ASSERT_EQ(static_cast<uint8_t>(GroupExtension::RACKSCALE), data[0]);
    ASSERT_EQ(DataRegionId::SMBIOS_TABLE, data[request::MdrRegionUpdateComplete::DATA_REGION_OFFSET]);
    ASSERT_EQ(0x12, data[request::MdrRegionUpdateComplete::SESSION_LOCK_HANDLE_OFFSET]);
}

TEST_F(MdrRegionUpdateCompleteTest, ResponseUnpacksCorrectly) {
    response::MdrRegionUpdateComplete rsp{};
    rsp.do_unpack(valid_response);
    ASSERT_TRUE(rsp.is_rackscale_extension_byte_present());
}
