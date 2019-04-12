/*!
 * @brief Tests for reading MDR region on Grantley platform.
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
 * @file read_mdr_region_test.cpp
 */

#include "ipmi/utils/sdv/grantley_mdr_region_accessor.hpp"
#include "ipmi/utils/sdv/purley_mdr_region_accessor.hpp"
#include "ipmi/command/generic/enums.hpp"
#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include "ipmi/command/sdv/mdr_region_read.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "test_fixture.hpp"

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

class GrantleyMdrTest : public ::testing::Test {
public:
    static constexpr const uint8_t CC_OK = 0x0;
    static constexpr const uint8_t MDR_VERSION = 0x4;
    static constexpr const uint8_t DATA_UPDATE_COUNT = 0x1;
    static constexpr const uint8_t CHECKSUM = 0x86;

protected:
    const std::vector<uint8_t>& expected_mdr = fixture::MDR_SMBIOS_REGION;

    IpmiInterface::ByteBuffer mdr_region_status_response_buffer = {
        CC_OK,
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

    IpmiInterface::ByteBuffer get_expected_mdr_chunk(int& chunk_index) {
        using command::sdv::request::MdrRegionRead;

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
            GrantleyMdrTest::CC_OK,
            std::uint8_t(expected_mdr_chunk.size()),
            GrantleyMdrTest::DATA_UPDATE_COUNT
    };
    mdr_response_buffer.insert(mdr_response_buffer.end(),
                               expected_mdr_chunk.begin(),
                               expected_mdr_chunk.end());
    arg1.do_unpack(mdr_response_buffer);
}

TEST_F(GrantleyMdrTest, ReadSuccess) {
    MockIpmiController ipmi_controller{};

    int chunk_index{0};
    EXPECT_CALL(ipmi_controller, send(_, _))
            .WillOnce(UnpackResponse(mdr_region_status_response_buffer))
            .WillRepeatedly(UnpackMdrReadResponse([&chunk_index,this] {return get_expected_mdr_chunk(chunk_index);}));
    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    const auto mdr = accessor->get_mdr_region();
    ASSERT_EQ(expected_mdr, mdr);
}

TEST_F(GrantleyMdrTest, InvalidMdrDataRegion) {
    MockIpmiController ipmi_controller{};
    using namespace command::sdv;
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{
            CC_OK,
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
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), ipmi::command::sdv::response::MdrDataRegionInvalidError);
}

TEST_F(GrantleyMdrTest, LockedMdrDataRegion) {
    MockIpmiController ipmi_controller{};
    using namespace command::sdv;
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{
            CC_OK,
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
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), response::MdrDataRegionLockedError);
}

TEST_F(GrantleyMdrTest, EmptyMdrDataRegion) {
    MockIpmiController ipmi_controller{};
    using namespace command::sdv;
    std::uint8_t used_region_size{0};
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
        .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{
            CC_OK,
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
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), response::MdrDataRegionEmptyError);
}

TEST_F(GrantleyMdrTest, MdrRegionReadTimeOutError) {
    MockIpmiController ipmi_controller{};
    EXPECT_CALL(ipmi_controller, send(_, _)).WillRepeatedly(Throw(std::logic_error("break")));
    EXPECT_CALL(ipmi_controller, send(_, _))
            .WillOnce(UnpackResponse(mdr_region_status_response_buffer))
            .WillOnce(UnpackResponse(IpmiInterface::ByteBuffer{TimeOutError::ERROR_CODE}))
            .RetiresOnSaturation();
    ipmi::sdv::MdrRegionAccessorFactory factory{};
    auto accessor = factory.create(ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R, ipmi_controller, command::sdv::DataRegionId::SMBIOS_TABLE);
    ASSERT_THROW(accessor->get_mdr_region(), TimeOutError);
}

}
