/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 * */
#include "agent-framework/module-ref/compute_manager.hpp"
#include "discovery/discovery_manager.hpp"
#include "mock_management_controller.hpp"
#include "mock_const.hpp"
#include "loader/compute_loader.hpp"
#include "configuration_full.hpp"
#include "json/deserializer.hpp"
#include "json/value.hpp"
#include "gtest/gtest.h"
#include <memory>

using namespace std;
using namespace ipmi;
using namespace agent::compute::loader;
using agent::compute::discovery::DiscoveryManager;

// TODO: For refactor purpose only. Remove after name unification.
using ComputeComponents = agent_framework::module::ComputeManager;

class DiscoveryManagerTest: public ::testing::Test {
private:
    void load_config_and_build_compute() {
        deserializer << COMPUTE_FULL_CONFIGURATION;
        deserializer >> config;
        bool is_loaded = loader.load(config);

        if (!is_loaded) {
            throw std::runtime_error("Load must be successful before test DOA!");
        }
    }

    string get_manager_uuid() {
        auto& module_manager = ComputeComponents::get_instance()->get_module_manager();
        auto uuids = module_manager.get_keys("");

        if (1 > uuids.size()) {
            throw std::runtime_error("One manager should be available!");
        }
        return uuids.front();
    }
    static bool is_initialized;
protected:

    ComputeLoader loader{};
    json::Value config;
    json::Deserializer deserializer{};
    string manager_uuid{};
public:

    DiscoveryManagerTest() {
        if (is_initialized) {
            return;
        }

        load_config_and_build_compute();
        manager_uuid = get_manager_uuid();

        /* Initialization mock managment controller */
        unique_ptr<ManagementController> mc(new MockManagementController());
        DiscoveryManager discovery_manager{mc};
        discovery_manager.discovery(manager_uuid);

        is_initialized = true;
    }

    void SetUp() {
        manager_uuid = get_manager_uuid();
    }

    virtual ~DiscoveryManagerTest();
};

bool DiscoveryManagerTest::is_initialized{false};

DiscoveryManagerTest::~DiscoveryManagerTest() {}

TEST_F(DiscoveryManagerTest, DiscoverManagerFirmwareVersion) {
    auto& module_manager = ComputeComponents::get_instance()->get_module_manager();
    auto manager = module_manager.get_entry(manager_uuid);
    ASSERT_STREQ(manager.get_firmware_version().value().c_str(), mock_const::FIRMWARE_VERSION);
}

TEST_F(DiscoveryManagerTest, DiscoverBios) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);

    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no system.";
    for (const auto& system_uuid: system_uuid_vector) {
        auto system = system_manager.get_entry(system_uuid);
        EXPECT_STREQ(system.get_bios_version().value().c_str(), mock_const::FULL_VERSION);
    }
}

TEST_F(DiscoveryManagerTest, DiscoverProcessors) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& processor_manager = ComputeComponents::get_instance()->get_processor_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto processor_uuid_vector = processor_manager.get_keys(system_uuid);
        EXPECT_FALSE(processor_uuid_vector.empty()) << "There is no Processor.";
    }
}

TEST_F(DiscoveryManagerTest, DiscoverProcessorFrequency) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& processor_manager = ComputeComponents::get_instance()->get_processor_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto processor_uuid_vector = processor_manager.get_keys(system_uuid);
        ASSERT_FALSE(processor_uuid_vector.empty()) << "There is no Processor.";

        for (const auto& processor_uuid: processor_uuid_vector) {
            auto processor = processor_manager.get_entry(processor_uuid);
            EXPECT_EQ(processor.get_max_speed_mhz(), mock_const::CPU_FREQUENCY)
                << "CPU Frequency is not set.";
        }
    }
}

TEST_F(DiscoveryManagerTest, DiscoverDimmModules) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& dimm_manager = ComputeComponents::get_instance()->get_dimm_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto dimm_uuid_vector = dimm_manager.get_keys(system_uuid);
        EXPECT_FALSE(dimm_uuid_vector.empty()) << "There is no Dimm Module.";
    }
}

TEST_F(DiscoveryManagerTest, DiscoverDimmModulesTypes) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System";
    auto& dimm_manager = ComputeComponents::get_instance()->get_dimm_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto dimm_uuid_vector = dimm_manager.get_keys(system_uuid);
        ASSERT_FALSE(dimm_uuid_vector.empty()) << "There is no Dimm Module.";

        for (const auto& dimm_uuid: dimm_uuid_vector) {
            auto dimm = dimm_manager.get_entry(dimm_uuid);
            EXPECT_EQ(dimm.get_memory_type(), agent_framework::model::enums::MemoryType::DRAM)
                << "Dimm type is not set.";
        }
    }
}

TEST_F(DiscoveryManagerTest, DiscoverDimmModulesVoltage) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& dimm_manager = ComputeComponents::get_instance()->get_dimm_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto dimm_uuid_vector = dimm_manager.get_keys(system_uuid);
        ASSERT_FALSE(dimm_uuid_vector.empty()) << "There is no Dimm Module.";

        for (const auto& dimm_uuid: dimm_uuid_vector) {
            auto dimm = dimm_manager.get_entry(dimm_uuid);
            EXPECT_EQ(dimm.get_max_voltage_volt(), mock_const::DIMM_VOLTAGE)
                << "Dimm maximum volate is not set.";
        }
    }
}

TEST_F(DiscoveryManagerTest, DiscoverDimmModulesSpeedMhz) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& dimm_manager = ComputeComponents::get_instance()->get_dimm_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto dimm_uuid_vector = dimm_manager.get_keys(system_uuid);
        ASSERT_FALSE(dimm_uuid_vector.empty()) << "There is no Dimm Module.";

        for (const auto& dimm_uuid: dimm_uuid_vector) {
            auto dimm = dimm_manager.get_entry(dimm_uuid);
            EXPECT_EQ(dimm.get_operating_speed_mhz(), mock_const::DIMM_SPEED_MHZ)
                << "Dimm configured speed is not set";
        }
    }
}

TEST_F(DiscoveryManagerTest, DiscoverDimmModulesSizeGB) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& dimm_manager = ComputeComponents::get_instance()->get_dimm_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto dimm_uuid_vector = dimm_manager.get_keys(system_uuid);
        ASSERT_FALSE(dimm_uuid_vector.empty()) << "There is no Dimm Module.";

        for (const auto& dimm_uuid: dimm_uuid_vector) {
            auto dimm = dimm_manager.get_entry(dimm_uuid);
            EXPECT_EQ(dimm.get_capacity_mb(), mock_const::DIMM_SIZE_MB)
                << "Dimm size is not set.";
        }
    }
}

TEST_F(DiscoveryManagerTest, DiscoverNetworkInterfaces) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& network_interface_manager = ComputeComponents::get_instance()->
        get_network_interface_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto network_interface_vector = network_interface_manager.get_keys(system_uuid);
        EXPECT_FALSE(network_interface_vector.empty())
            << "There is no Network Interface.";
    }
}

TEST_F(DiscoveryManagerTest, DiscoverNetworkInterfacesHaswellMacAddress) {
    auto& system_manager = ComputeComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& network_interface_manager = ComputeComponents::get_instance()->
        get_network_interface_manager();

    for (const auto& system_uuid: system_uuid_vector) {
        auto network_interface_vector = network_interface_manager.get_keys(system_uuid);
        ASSERT_FALSE(network_interface_vector.empty()) << "There is no Network Interface.";

        for (const auto& network_interface_uuid: network_interface_vector) {
            auto network_interface = network_interface_manager.get_entry(network_interface_uuid);
            const auto& mac_address = network_interface.get_mac_address();
            if (mac_address && !mac_address.value().empty()) {
                EXPECT_STREQ(mac_address.value().c_str(),
                             mock_const::SYSTEM_MAC_ADDRESS_HASWELL)
                    << "Network interface mac address is not set.";
            }
        }
    }
}

TEST_F(DiscoveryManagerTest, DiscoverManufacturer) {
    auto& chassis_manager = ComputeComponents::get_instance()->get_chassis_manager();
    auto chassis_uuid_vector = chassis_manager.get_keys(manager_uuid);
    ASSERT_FALSE(chassis_uuid_vector.empty()) << "There is no Chassis.";

    for (const auto& chassis_uuid: chassis_uuid_vector) {
        auto chassis = chassis_manager.get_entry(chassis_uuid);
        auto fru_info = chassis.get_fru_info();
        EXPECT_STREQ(fru_info.get_manufacturer().value().c_str(), mock_const::MANUFACTURER_ID_INTEL)
            << "Chassis manufacturer id is not set.";
    }
}

TEST_F(DiscoveryManagerTest, DiscoverProduct) {
    auto& chassis_manager = ComputeComponents::get_instance()->get_chassis_manager();
    auto chassis_uuid_vector = chassis_manager.get_keys(manager_uuid);
    ASSERT_FALSE(chassis_uuid_vector.empty()) << "There is no Chassis.";

    for (const auto& chassis_uuid: chassis_uuid_vector) {
        auto chassis = chassis_manager.get_entry(chassis_uuid);
        auto fru_info = chassis.get_fru_info();
        EXPECT_STREQ(fru_info.get_model_number().value().c_str(), mock_const::PRODUCT_ID_INTEL_ATOM)
            << "Chassis product id is not set.";
    }
}
