/*!
 * @brief Tests for reading MDR region on purley platform.
 *
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
 * @file purley_accessor_test.cpp
 */

#include "ipmi/utils/sdv/purley_mdr_region_accessor.hpp"
#include "ipmi/command/sdv/rsd/get_mdr_data_region_status.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_read.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_write.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_update_complete.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_lock.hpp"
#include "ipmi/command/generic/enums.hpp"
#include "purley_test_fixture.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


namespace ipmi {

class MockIpmiController : public ipmi::IpmiController {
public:
    MockIpmiController() : IpmiController(nullptr) {}

    MOCK_METHOD2(send, void(const Request&, Response&));

    void send(const Request&, const BridgeInfo&, Response&) override {
        /* not to be called, send(req, resp) is only allowed */
        throw std::exception();
    }
};

}

using namespace ipmi;

class PurleyMdrRegionTest : public ::testing::Test {
public:
    static constexpr const uint8_t CC_OK = 0x0;
    static constexpr const uint8_t CC_IN_USE = 0x81;
    static constexpr const uint8_t RACKSCALE = 0x4;
    static constexpr const uint8_t MDR_VERSION = 0x4;
    static constexpr const uint8_t DATA_UPDATE_COUNT = 0x1;
    static constexpr const uint8_t CHECKSUM = 0x82;
    static constexpr const uint8_t LOCK_HANDLE = 0x02;
    static constexpr const uint8_t ISCSI_MDR = 0x04;

protected:
    const std::vector<uint8_t>& expected_mdr = purley_fixture::MDR_SMBIOS_REGION;

    IpmiInterface::ByteBuffer mdr_region_status_response_buffer = {
        CC_OK,
        RACKSCALE,
        MDR_VERSION,
        command::sdv::DataRegionId::SMBIOS_TABLE,
        ipmi::command::sdv::DataValidation::VALID,
        DATA_UPDATE_COUNT,
        ipmi::command::sdv::LockStatus::UNLOCKED,
        uint8_t(expected_mdr.size() & 0xff),
        uint8_t((expected_mdr.size() >> 8) & 0xff),
        uint8_t(expected_mdr.size() & 0x00ff),
        uint8_t((expected_mdr.size() & 0xff00) >> 8),
        CHECKSUM
    };

    IpmiInterface::ByteBuffer mdr_region_lock_response_buffer = {
            CC_OK,
            RACKSCALE,
            LOCK_HANDLE
    };

    IpmiInterface::ByteBuffer get_expected_mdr_chunk(int& chunk_index) {
        using command::sdv::rsd::request::MdrRegionRead;

        auto expected_mdr_size = int(expected_mdr.size());
        auto num_chunks = expected_mdr_size / MdrRegionRead::MAX_DATA_COUNT;
        auto last_chunk_length = expected_mdr_size % MdrRegionRead::MAX_DATA_COUNT;
        if (0 != last_chunk_length) {
            num_chunks++;
        }
        if (num_chunks <= chunk_index) {
            throw std::out_of_range("chunk index " + std::to_string(chunk_index)
                                    + " should be less than number of chunks " + std::to_string(num_chunks));
        }
        auto start_offset = 0;
        auto end_offset = int(MdrRegionRead::MAX_DATA_COUNT);
        if (expected_mdr_size >= ((chunk_index + 1) * MdrRegionRead::MAX_DATA_COUNT)) {
            start_offset = chunk_index * MdrRegionRead::MAX_DATA_COUNT;
            end_offset = start_offset + MdrRegionRead::MAX_DATA_COUNT;
        }
        else {
            start_offset = expected_mdr_size - last_chunk_length;
            end_offset = start_offset + last_chunk_length;
        }
        chunk_index++;
        return IpmiInterface::ByteBuffer(expected_mdr.cbegin() + start_offset, expected_mdr.cbegin() + end_offset);
    }
};

namespace testing {

ACTION_P(UnpackResponse, byte_buffer) {
    arg1.do_unpack(byte_buffer);
}

ACTION_P(UnpackMdrReadResponse, get_expected_mdr_chunk_cb) {
    auto expected_mdr_chunk = get_expected_mdr_chunk_cb();
    IpmiInterface::ByteBuffer mdr_response_buffer = {
        PurleyMdrRegionTest::CC_OK,
        0x04,
        std::uint8_t(expected_mdr_chunk.size()),
        PurleyMdrRegionTest::DATA_UPDATE_COUNT
    };
    mdr_response_buffer.insert(mdr_response_buffer.end(),
                               expected_mdr_chunk.begin(),
                               expected_mdr_chunk.end());
    arg1.do_unpack(mdr_response_buffer);
}

ACTION_P(UnpackMdrWriteResponse, out_vec) {
    ::testing::StaticAssertTypeEq<IpmiInterface::ByteBuffer*, decltype(out_vec)>();
    IpmiInterface::ByteBuffer mdr_response_buffer = {
        PurleyMdrRegionTest::CC_OK,
        0x04,
        PurleyMdrRegionTest::ISCSI_MDR,
        0x00, // data invalid
        0x01, // strict lock
        uint8_t(purley_fixture::MDR_SMBIOS_REGION.size() & 0xff), // dummy sizes, don't really matter
        uint8_t((purley_fixture::MDR_SMBIOS_REGION.size() >> 8) & 0xff),
        uint8_t(purley_fixture::MDR_SMBIOS_REGION.size() & 0x00ff),
        uint8_t((purley_fixture::MDR_SMBIOS_REGION.size() & 0xff00) >> 8)
    };

    // copy data from the packed message
    auto packed_write = arg0.do_pack();
    auto where_to_start = packed_write.cbegin();
    std::advance(where_to_start, command::sdv::rsd::request::MdrRegionWrite::WHAT_TO_WRITE_OFFSET);
    std::copy(where_to_start, packed_write.cend(), std::back_inserter(*out_vec));

    arg1.do_unpack(mdr_response_buffer);
}

MATCHER(IsLockReq, "match lock request message") {
        auto *lock = dynamic_cast<const command::sdv::rsd::request::MdrRegionLock*>(&arg);
        return lock != nullptr;
}

MATCHER(IsLockResp, "match lock response message") {
        auto *lock = dynamic_cast<const command::sdv::rsd::response::MdrRegionLock*>(&arg);
        return lock != nullptr;
}

MATCHER(IsWriteReq, "match write request message") {
        auto *write = dynamic_cast<const command::sdv::rsd::request::MdrRegionWrite*>(&arg);
        return write != nullptr;
}

MATCHER(IsWriteResp, "match write response message") {
        auto *write = dynamic_cast<const command::sdv::rsd::response::MdrRegionWrite*>(&arg);
        return write != nullptr;
}

MATCHER(IsCompleteReq, "match complete request message") {
        auto *complete = dynamic_cast<const command::sdv::rsd::request::MdrRegionUpdateComplete*>(&arg);
        return complete != nullptr;
}

MATCHER(IsCompleteResp, "match complete response message") {
        auto *complete = dynamic_cast<const command::sdv::rsd::response::MdrRegionUpdateComplete*>(&arg);
        return complete != nullptr;
}

MATCHER(IsStatusReq, "match complete request message") {
        auto *complete = dynamic_cast<const command::sdv::rsd::request::GetMdrDataRegionStatus*>(&arg);
        return complete != nullptr;
}

MATCHER(IsStatusResp, "match complete response message") {
        auto *complete = dynamic_cast<const command::sdv::rsd::response::GetMdrDataRegionStatus*>(&arg);
        return complete != nullptr;
}

TEST_F(PurleyMdrRegionTest, ReadSuccess) {
    MockIpmiController ipmi_controller{};

    int chunk_index{0};
    EXPECT_CALL(ipmi_controller, send(_, _))
            .WillRepeatedly(UnpackMdrReadResponse([&chunk_index,this] {return get_expected_mdr_chunk(chunk_index);}));

    EXPECT_CALL(ipmi_controller, send(IsStatusReq(), IsStatusResp()))
        .WillOnce(UnpackResponse(mdr_region_status_response_buffer));

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    const auto mdr = accessor->get_mdr_region();
    ASSERT_EQ(expected_mdr, mdr);
}

TEST_F(PurleyMdrRegionTest, InvalidMdrDataRegion) {
    MockIpmiController ipmi_controller{};
    using namespace command::sdv;
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{
            CC_OK,
            RACKSCALE,
            MDR_VERSION,
            DataRegionId::SMBIOS_TABLE,
            ipmi::command::sdv::DataValidation::INVALID,
            DATA_UPDATE_COUNT,
            ipmi::command::sdv::LockStatus::UNLOCKED,
            uint8_t(expected_mdr.size() & 0xff),
            uint8_t((expected_mdr.size() >> 8) & 0xff),
            uint8_t(expected_mdr.size() & 0x00ff),
            uint8_t((expected_mdr.size() & 0xff00) >> 8),
            CHECKSUM
        })).RetiresOnSaturation();

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), ipmi::command::sdv::response::MdrDataRegionInvalidError);
}

TEST_F(PurleyMdrRegionTest, LockedMdrDataRegion) {
    MockIpmiController ipmi_controller{};
    using namespace command::sdv;
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{
            CC_OK,
            RACKSCALE,
            MDR_VERSION,
            DataRegionId::SMBIOS_TABLE,
            ipmi::command::sdv::DataValidation::VALID,
            DATA_UPDATE_COUNT,
            ipmi::command::sdv::LockStatus::STRICT_LOCK,
            uint8_t(expected_mdr.size() & 0xff),
            uint8_t((expected_mdr.size() >> 8) & 0xff),
            uint8_t(expected_mdr.size() & 0x00ff),
            uint8_t((expected_mdr.size() & 0xff00) >> 8),
            CHECKSUM
        })).RetiresOnSaturation();

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), response::MdrDataRegionLockedError);
}

TEST_F(PurleyMdrRegionTest, EmptyMdrDataRegion) {
    MockIpmiController ipmi_controller{};
    using namespace command::sdv;
    std::uint8_t used_region_size{0};
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{
            CC_OK,
            RACKSCALE,
            MDR_VERSION,
            DataRegionId::SMBIOS_TABLE,
            ipmi::command::sdv::DataValidation::VALID,
            DATA_UPDATE_COUNT,
            ipmi::command::sdv::LockStatus::UNLOCKED,
            uint8_t(expected_mdr.size() & 0xff),
            uint8_t((expected_mdr.size() >> 8) & 0xff),
            used_region_size, used_region_size,
            CHECKSUM
        })).RetiresOnSaturation();

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), response::MdrDataRegionEmptyError);
}

TEST_F(PurleyMdrRegionTest, MdrRegionReadTimeOutError) {
    MockIpmiController ipmi_controller{};
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(mdr_region_status_response_buffer))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{TimeOutError::ERROR_CODE}))
        .RetiresOnSaturation();

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), TimeOutError);
}

TEST_F(PurleyMdrRegionTest, WriteSuccess) {
    MockIpmiController ipmi_controller{};

    EXPECT_CALL(ipmi_controller, send(IsStatusReq(), IsStatusResp()))
        .WillOnce(UnpackResponse(mdr_region_status_response_buffer));

    EXPECT_CALL(ipmi_controller, send(IsLockReq(), IsLockResp()))
        .WillOnce(UnpackResponse(mdr_region_lock_response_buffer));

    IpmiInterface::ByteBuffer out_data;
    EXPECT_CALL(ipmi_controller, send(IsWriteReq(), IsWriteResp()))
        .Times(5)
        .WillRepeatedly(UnpackMdrWriteResponse(&out_data));

    EXPECT_CALL(ipmi_controller, send(IsCompleteReq(), IsCompleteResp()))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{CC_OK, 0x04}));

    IpmiInterface::ByteBuffer data;
    std::generate_n(std::back_inserter(data), 512, []() -> int{
        static int count = 0;
        return count++;}
    );

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller,
        command::sdv::DataRegionId::ISCSI_BOOT_OPTIONS);
    accessor->write_mdr_region(data);
    ASSERT_EQ(data, out_data);
}

TEST_F(PurleyMdrRegionTest, UpdateCompleteFailure) {
    MockIpmiController ipmi_controller{};

    EXPECT_CALL(ipmi_controller, send(IsStatusReq(), IsStatusResp()))
        .WillOnce(UnpackResponse(mdr_region_status_response_buffer));

    EXPECT_CALL(ipmi_controller, send(IsLockReq(), IsLockResp()))
        .WillOnce(UnpackResponse(mdr_region_lock_response_buffer));

    IpmiInterface::ByteBuffer out_data;
    EXPECT_CALL(ipmi_controller, send(IsWriteReq(), IsWriteResp()))
        .Times(5)
        .WillRepeatedly(UnpackMdrWriteResponse(&out_data));

    EXPECT_CALL(ipmi_controller, send(IsCompleteReq(), IsCompleteResp()))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{CC_IN_USE, 0x04}));

    IpmiInterface::ByteBuffer data;
    std::generate_n(std::back_inserter(data), 512, []() -> int{
        static int count = 0;
        return count++;}
    );

    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_PURLEY, ipmi_controller,
        command::sdv::DataRegionId::ISCSI_BOOT_OPTIONS);
    ASSERT_THROW(accessor->write_mdr_region(data), ipmi::MdrRegionInUse);
}

}
