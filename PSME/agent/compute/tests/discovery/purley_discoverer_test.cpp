/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */


#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "mock/mock_management_controller.hpp"
#include "mock/smbios_mock.hpp"
#include "discovery/discoverers/platform_specific/purley_discoverer.hpp"

#include <gtest/gtest.h>
#include "gmock/gmock.h"


using ::testing::_;
using ::testing::Invoke;
using ::testing::Throw;
using ::testing::AnyNumber;
using ::testing::NiceMock;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::Return;

using agent::compute::discovery::PurleyDiscoverer;
using namespace agent_framework::model;

namespace ipmi {

class MockIpmiController : public ipmi::IpmiController {
public:
    MockIpmiController() : IpmiController(nullptr) {
        EXPECT_CALL(*this, send(_, _, _)).Times(0);
    }

    MOCK_METHOD2(send, void(const Request&, Response&));
    MOCK_METHOD3(send, void(const Request&, const BridgeInfo&, Response&));
};

namespace sdv {

class MockSmbiosMdrRegionAccessor : public MdrRegionAccessor {
public:
    MockSmbiosMdrRegionAccessor(ipmi::IpmiController& ipmi_controller, const command::sdv::DataRegionId region_id)
        : MdrRegionAccessor(ipmi_controller, region_id) {}

    MOCK_METHOD0(get_mdr_region, IpmiInterface::ByteBuffer());
    MOCK_METHOD0(get_mdr_region_checksum, std::uint8_t());
    MOCK_METHOD0(get_mdr_region_size_to_read, std::uint16_t());
    MOCK_METHOD1(write_mdr_region, void(const IpmiInterface::ByteBuffer&));
    MOCK_METHOD0(get_mdr_region_update_count, std::uint8_t());
};

class MockMdrRegionAccessorFactory : public MdrRegionAccessorFactory {
public:
    MdrRegionAccessor::Ptr create(std::uint32_t platform_id,
            IpmiController& ipmi_controller,
            const command::sdv::DataRegionId region_id) const override {
        return MdrRegionAccessor::Ptr(create_proxy(platform_id, ipmi_controller, region_id));
    }

    MOCK_CONST_METHOD3(create_proxy, MdrRegionAccessor*(std::uint32_t, IpmiController&, const command::sdv::DataRegionId));
};

}

}

namespace agent {
namespace compute {
namespace discovery {

class PurleyDiscovererTest : public ::testing::Test {
public:
    PurleyDiscovererTest() {}

    virtual ~PurleyDiscovererTest() {};

    void SetUp() override {
        m_mdr_accessor_factory = std::make_shared<ipmi::sdv::MockMdrRegionAccessorFactory>();
        ON_CALL(*m_mdr_accessor_factory, create_proxy(_, _, _)).WillByDefault(Invoke([](std::uint32_t,
                ipmi::IpmiController& ctrl, const ipmi::command::sdv::DataRegionId region_id2) {
            auto* a = new ipmi::sdv::MockSmbiosMdrRegionAccessor(ctrl, region_id2);
            ON_CALL(*a, get_mdr_region()).WillByDefault(Invoke([]() {
                return ipmi::IpmiInterface::ByteBuffer(mocked_smbios::smbios,
                        mocked_smbios::smbios + sizeof (mocked_smbios::smbios) / sizeof (mocked_smbios::smbios[0]));
            }));
            return a;
        }));
    }

    const std::string m_dummy_parent = "dummy_parent";
    NiceMock<ipmi::MockIpmiController> m_ipmi_ctrl{};
    std::shared_ptr<ipmi::sdv::MockMdrRegionAccessorFactory> m_mdr_accessor_factory{};
};


TEST_F(PurleyDiscovererTest, DiscoverDrives) {
    std::vector<Drive> drives{};
    PurleyDiscoverer purley_discoverer(m_ipmi_ctrl, m_mdr_accessor_factory);
    purley_discoverer.read_mdr();
    EXPECT_TRUE(purley_discoverer.discover_drives(drives, m_dummy_parent));
    EXPECT_EQ(1, drives.size());
    auto drive = drives[0];
    EXPECT_EQ(m_dummy_parent, drive.get_parent_uuid());
    EXPECT_EQ(255, drive.get_capacity_gb());
    EXPECT_EQ(enums::TransportProtocol::SATA, drive.get_interface());
    EXPECT_EQ(enums::DriveType::HDD, drive.get_type());
    EXPECT_EQ("ABC789", drive.get_firmware_version().value());
    EXPECT_EQ("321Model", drive.get_fru_info().get_model_number().value());
    EXPECT_EQ("INTEL123", drive.get_fru_info().get_serial_number().value());
}


TEST_F(PurleyDiscovererTest, DiscoverTPM) {
    std::vector<TrustedModule> tpms{};
    PurleyDiscoverer purley_discoverer(m_ipmi_ctrl, m_mdr_accessor_factory);
    purley_discoverer.read_mdr();
    purley_discoverer.discover_trusted_modules(tpms, m_dummy_parent);
    ASSERT_EQ(2, tpms.size());
    const auto tpm_1 = tpms[0];
    const auto tpm_2 = tpms[1];
    EXPECT_EQ(1, tpm_1.get_configuration_index());
    EXPECT_STREQ("TPM2_0", tpm_1.get_interface_type().value().to_string());
    EXPECT_EQ(2, tpm_2.get_configuration_index());
    EXPECT_STREQ("TPM1_2", tpm_2.get_interface_type().value().to_string());
}

}
}
}
