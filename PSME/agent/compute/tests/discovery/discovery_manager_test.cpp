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
#include "discovery/discovery_manager.hpp"
#include "mock/mock_management_controller.hpp"

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

using smbios::parser::SmbiosParser;
using agent::compute::discovery::DiscoveryManager;
using namespace agent_framework::model;
using namespace agent_framework::module;

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

class MockMdrRegionAccessorFactory : public MdrRegionAccessorFactory {
public:
    MdrRegionAccessor::Ptr create(std::uint32_t platform_id,
            IpmiController& ipmi_controller,
            const command::sdv::DataRegionId region_id) const override {
        return MdrRegionAccessor::Ptr(create_proxy(platform_id, ipmi_controller, region_id));
    }

    MOCK_CONST_METHOD3(create_proxy, MdrRegionAccessor*(std::uint32_t, IpmiController&, const command::sdv::DataRegionId));
};

} // sdv
} // ipmi

namespace agent {
namespace compute {

class BmcMock : public Bmc {
public:
    BmcMock(const Bmc::ConnectionData& conn) : Bmc(conn, /*std::chrono::milliseconds{100} */Bmc::Duration{}) {}

    MOCK_METHOD1(on_become_online, bool(const Transition&));

    ipmi::IpmiController& ipmi() override {
        return m_ipmi;
    }

    OptionalField<ipmi::command::generic::BmcInterface> get_interface() const override {
        return ipmi::command::generic::BmcInterface::RSD_2_4;
    }

private:
    NiceMock<ipmi::MockIpmiController> m_ipmi{};
};

namespace discovery {

class MockDiscoverer : public GenericDiscoverer {
public:
    MockDiscoverer(ipmi::IpmiController& ipmi_controller,
                   ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory)
        : GenericDiscoverer(ipmi_controller, mdr_accessor_factory) {}

    MOCK_CONST_METHOD0(get_platform_id, std::uint32_t());
    MOCK_METHOD0(read_mdr, void());
    MOCK_METHOD1(discover_manager,
            bool(agent_framework::model::Manager& manager));
    MOCK_METHOD1(discover_chassis,
            bool(agent_framework::model::Chassis& chassis));
    MOCK_METHOD1(discover_thermal_zone,
            bool(agent_framework::model::ThermalZone& zone));
    MOCK_METHOD1(discover_power_zone,
            bool(agent_framework::model::PowerZone& zone));
    MOCK_METHOD1(discover_system,
            bool(agent_framework::model::System& system));
    MOCK_METHOD2(discover_processors,
            bool(std::vector<agent_framework::model::Processor>& processors, const std::string& parent_uuid));
    MOCK_METHOD2(discover_memory,
            bool(std::vector<agent_framework::model::Memory>& memories, const std::string& parent_uuid));
    MOCK_METHOD1(discover_storage_subsystem,
            bool(agent_framework::model::StorageSubsystem& storage));
    MOCK_METHOD2(discover_drives,
            bool(std::vector<agent_framework::model::Drive>& drives, const std::string& parent_uuid));
    MOCK_METHOD2(discover_network_interfaces,
            bool(std::vector<agent_framework::model::NetworkInterface>& network_interfaces, const std::string& parent_uuid));
    MOCK_METHOD1(discover_network_device,
            bool(agent_framework::model::NetworkDevice& network_device));
    MOCK_METHOD1(discover_network_device_function,
            bool(agent_framework::model::NetworkDeviceFunction& network_device_function));
    MOCK_METHOD1(discover_bios,
            bool(agent_framework::model::System& system));
    MOCK_METHOD1(discover_cable_id,
            bool(agent_framework::model::System& system));
    MOCK_METHOD2(discover_trusted_modules,
            bool(std::vector<agent_framework::model::TrustedModule>& trusted_modules, const std::string& parent_uuid));
    MOCK_METHOD1(discover_user_mode,
            bool(agent_framework::model::System& system));
};

class MockDiscovererFactory : public DiscovererFactory {
public:

    GenericDiscoverer::Ptr create(ipmi::command::generic::BmcInterface bmc_interface,
                                  ipmi::IpmiController& ipmi_controller,
                                  ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory) const override {
        return GenericDiscoverer::Ptr(create_proxy(bmc_interface, ipmi_controller, mdr_accessor_factory));
    }

    MOCK_CONST_METHOD3(create_proxy, GenericDiscoverer*(ipmi::command::generic::BmcInterface bmc_interface,
                                                        ipmi::IpmiController& ipmi_controller,
                                                        ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory));
};



class DiscoveryManagerTest : public ::testing::Test {
public:
    DiscoveryManagerTest() {}

    void SetUp() override {
        get_manager<Drive>().clear_entries();
    }

    void TearDown() override {
        get_manager<Drive>().clear_entries();
    }

    virtual ~DiscoveryManagerTest() {};
};


TEST_F(DiscoveryManagerTest, DiscoverDrivesTwice) {
    Bmc::ConnectionData conn{};
    conn.set_ip_address("0.0.0.1");
    agent::compute::BmcMock bmc{conn};
    bmc.start();
    auto mdr_region_factory = std::make_shared<ipmi::sdv::MockMdrRegionAccessorFactory>();
    ipmi::MockIpmiController ipmi_ctrl{};

    std::vector<agent_framework::model::Drive> drives{};
    drives.emplace_back("dummy_parent1");
    auto drives2 = drives;
    drives.emplace_back("dummy_parent2");

    auto discoverer_factory = std::make_shared<MockDiscovererFactory>();
    ON_CALL(*discoverer_factory, create_proxy(_, _, _)).WillByDefault(Invoke([drives, drives2](ipmi::command::generic::BmcInterface,
            ipmi::IpmiController& ctrl, ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory) {
        auto* discoverer = new MockDiscoverer(ctrl, mdr_accessor_factory);
        EXPECT_CALL(*discoverer, discover_drives(_, _))
            .WillOnce(DoAll(SetArgReferee<0>(drives), Return(true)))
            .WillOnce(DoAll(SetArgReferee<0>(drives2), Return(true)));
        return discoverer;
    }));

    ASSERT_TRUE(get_manager<Drive>().get_keys().empty());

    DiscoveryManager discovery_manager{bmc, mdr_region_factory, discoverer_factory};
    discovery_manager.discover();

    const auto uuids = get_manager<Drive>().get_keys();
    ASSERT_EQ(2, uuids.size());
    const auto drive1 = get_manager<Drive>().get_entry(uuids[0]);
    EXPECT_EQ("0.0.0.1:0", drive1.get_agent_id());
    EXPECT_EQ("dummy_parent1", drive1.get_parent_uuid());
    const auto drive2 = get_manager<Drive>().get_entry(uuids[1]);
    EXPECT_EQ("0.0.0.1:0", drive2.get_agent_id());
    EXPECT_EQ("dummy_parent2", drive2.get_parent_uuid());

    discovery_manager.discover();
    ASSERT_EQ(1, get_manager<Drive>().get_keys().size());
    const auto drive = get_manager<Drive>().get_entry(uuids[0]);
    EXPECT_EQ("0.0.0.1:0", drive.get_agent_id());
    EXPECT_EQ("dummy_parent1", drive.get_parent_uuid());

    bmc.stop();
}

}
}
}
