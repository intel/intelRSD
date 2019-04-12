/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
#include "loader/compute_loader.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/logger_loader.hpp"

#include "mock/mock_const.hpp"
#include "mock/mock_management_controller.hpp"
#include "mock/configuration_full.hpp"

#include <gtest/gtest.h>



using namespace ipmi;
using namespace agent::compute::loader;
using namespace agent::compute::discovery;
using namespace agent_framework::module;

namespace {

std::string to_string_with_padding(int number) {
    std::stringstream stream{};
    stream << std::setw(2) << std::setfill('0') << number;
    return stream.str();
}

}

class DiscoveryManagerTest : public ::testing::Test {
private:
    void load_config_and_build_compute() {
        ComputeLoader loader{};
        json::Json config = json::Json();
        config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);
        bool is_loaded = loader.load(config);

        LoggerLoader loader(configuration);
        loader.load(LoggerFactory::instance());

        if (!is_loaded) {
            throw std::runtime_error("Load must be successful before test DOA!");
        }
    }


    std::string get_manager_uuid() {
        auto& module_manager = CommonComponents::get_instance()->get_module_manager();
        auto uuids = module_manager.get_keys("");

        if (1 > uuids.size()) {
            throw std::runtime_error("One manager should be available!");
        }
        return uuids.front();
    }


    static bool is_initialized;

protected:
    std::string manager_uuid{};

public:
    DiscoveryManagerTest() {
        if (is_initialized) {
            return;
        }

        load_config_and_build_compute();
        manager_uuid = get_manager_uuid();

        /* Initialization mock management controller */
        std::unique_ptr<ManagementController> mc(new MockManagementController());
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


MockManagementController::~MockManagementController() {}


TEST_F(DiscoveryManagerTest, DiscoverManagerFirmwareVersion) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto manager = module_manager.get_entry(manager_uuid);

    const auto& firmware_version = std::to_string(int(mock::FIRMWARE_VERSION_MAJOR)) +
                                   "." + ::to_string_with_padding(int(mock::FIRMWARE_VERSION_MINOR));

    ASSERT_STREQ(manager.get_firmware_version().value().c_str(), firmware_version.c_str());
}


TEST_F(DiscoveryManagerTest, DiscoverBios) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);

    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no system.";
    for (const auto& system_uuid : system_uuid_vector) {
        auto system = system_manager.get_entry(system_uuid);
        EXPECT_STREQ(system.get_bios_version().value().c_str(), mock::FULL_VERSION);
    }
}


TEST_F(DiscoveryManagerTest, DiscoverProcessors) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& processor_manager = get_manager<agent_framework::model::Processor>();

    for (const auto& system_uuid : system_uuid_vector) {
        auto processor_uuid_vector = processor_manager.get_keys(system_uuid);
        EXPECT_FALSE(processor_uuid_vector.empty()) << "There is no Processor.";
    }
}


TEST_F(DiscoveryManagerTest, DiscoverProcessorFrequency) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& processor_manager = get_manager<agent_framework::model::Processor>();

    for (const auto& system_uuid: system_uuid_vector) {
        // search only for CPUs, skip FPGA
        auto processor_uuid_vector = processor_manager.get_keys(system_uuid,
                           [](const agent_framework::model::Processor& processor) {
                    return processor.get_processor_type() == agent_framework::model::enums::ProcessorType::CPU;
        });

        ASSERT_FALSE(processor_uuid_vector.empty()) << "There is no Processor.";

        for (const auto& processor_uuid: processor_uuid_vector) {
            auto processor = processor_manager.get_entry(processor_uuid);
            EXPECT_EQ(processor.get_max_speed_mhz(), mock::CPU_FREQUENCY)
                            << "CPU Frequency is not set.";
        }
    }
}


TEST_F(DiscoveryManagerTest, DiscoverCableId) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";

    for (const auto& system_uuid: system_uuid_vector) {
        auto system = CommonComponents::get_instance()->get_system_manager().get_entry(system_uuid);
        auto cable_ids = system.get_cable_ids();
        EXPECT_EQ(cable_ids.size(), 4) << "Cable id is not set";
        EXPECT_EQ(cable_ids[0], mock::PCIE_PORT_CABLE_ID) << "Invalid Cable id value";
    }
}


TEST_F(DiscoveryManagerTest, DiscoverMemoryModules) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();

    for (const auto& system_uuid : system_uuid_vector) {
        auto memory_uuid_vector = memory_manager.get_keys(system_uuid);
        EXPECT_FALSE(memory_uuid_vector.empty()) << "There is no Memory Module.";
    }
}


TEST_F(DiscoveryManagerTest, DiscoverMemoryDeviceType) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System";
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();

    for (const auto& system_uuid : system_uuid_vector) {
        auto memory_uuid_vector = memory_manager.get_keys(system_uuid);
        ASSERT_FALSE(memory_uuid_vector.empty()) << "There is no Memory Module.";

        for (const auto& memory_uuid : memory_uuid_vector) {
            auto memory = memory_manager.get_entry(memory_uuid);
            EXPECT_EQ(memory.get_device_type(), agent_framework::model::enums::DeviceType::DDR3)
                            << "Memory type is not set.";
        }
    }
}


TEST_F(DiscoveryManagerTest, DiscoverMemoryModulesVoltage) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();

    for (const auto& system_uuid : system_uuid_vector) {
        auto memory_uuid_vector = memory_manager.get_keys(system_uuid);
        ASSERT_FALSE(memory_uuid_vector.empty()) << "There is no Memory Module.";

        for (const auto& memory_uuid : memory_uuid_vector) {
            auto memory = memory_manager.get_entry(memory_uuid);
            EXPECT_EQ(memory.get_voltage_volt(), mock::MEMORY_VOLTAGE)
                            << "Memory maximum voltage is not set.";
        }
    }
}


TEST_F(DiscoveryManagerTest, DiscoverMemoryModulesSpeedMhz) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();

    for (const auto& system_uuid : system_uuid_vector) {
        auto memory_uuid_vector = memory_manager.get_keys(system_uuid);
        ASSERT_FALSE(memory_uuid_vector.empty()) << "There is no Memory Module.";

        for (const auto& memory_uuid : memory_uuid_vector) {
            auto memory = memory_manager.get_entry(memory_uuid);
            EXPECT_EQ(memory.get_operating_speed_mhz(), mock::MEMORY_SPEED_MHZ)
                            << "Memory configured speed is not set";
        }
    }
}


TEST_F(DiscoveryManagerTest, DiscoverMemoryModulesSizeGB) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& memory_manager = ComputeComponents::get_instance()->get_memory_manager();

    for (const auto& system_uuid : system_uuid_vector) {
        auto memory_uuid_vector = memory_manager.get_keys(system_uuid);
        ASSERT_FALSE(memory_uuid_vector.empty()) << "There is no Memory Module.";

        for (const auto& memory_uuid : memory_uuid_vector) {
            auto memory = memory_manager.get_entry(memory_uuid);
            EXPECT_EQ(memory.get_capacity_mib(), mock::MEMORY_SIZE_MIB)
                            << "Memory size is not set.";
        }
    }
}


TEST_F(DiscoveryManagerTest, DiscoverNetworkInterfaces) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& network_interface_manager = get_manager<agent_framework::model::NetworkInterface>();

    for (const auto& system_uuid : system_uuid_vector) {
        auto network_interface_vector = network_interface_manager.get_keys(system_uuid);
        EXPECT_FALSE(network_interface_vector.empty())
                        << "There is no Network Interface.";
    }
}


TEST_F(DiscoveryManagerTest, DiscoverNetworkInterfacesHaswellMacAddress) {
    auto& system_manager = CommonComponents::get_instance()->get_system_manager();
    auto system_uuid_vector = system_manager.get_keys(manager_uuid);
    ASSERT_FALSE(system_uuid_vector.empty()) << "There is no System.";
    auto& network_interface_manager = get_manager<agent_framework::model::NetworkInterface>();

    for (const auto& system_uuid : system_uuid_vector) {
        auto network_interface_vector = network_interface_manager.get_keys(system_uuid);
        ASSERT_FALSE(network_interface_vector.empty()) << "There is no Network Interface.";

        for (const auto& network_interface_uuid: network_interface_vector) {
            auto network_interface = network_interface_manager.get_entry(network_interface_uuid);
            const auto& mac_address = network_interface.get_mac_address();
            if (mac_address && !mac_address.value().empty()) {
                EXPECT_STREQ(mac_address.value().c_str(), mock::SYSTEM_MAC_ADDRESS_HASWELL)
                                << "Network interface mac address is not set.";
            }
        }
    }
}


TEST_F(DiscoveryManagerTest, DiscoverManufacturer) {
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    auto chassis_uuid_vector = chassis_manager.get_keys(manager_uuid);
    ASSERT_FALSE(chassis_uuid_vector.empty()) << "There is no Chassis.";

    for (const auto& chassis_uuid : chassis_uuid_vector) {
        auto chassis = chassis_manager.get_entry(chassis_uuid);
        auto fru_info = chassis.get_fru_info();
        EXPECT_STREQ(fru_info.get_manufacturer().value().c_str(), mock::MANUFACTURER_ID_INTEL)
                        << "Chassis manufacturer id is not set.";
    }
}


TEST_F(DiscoveryManagerTest, DiscoverProduct) {
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();
    auto chassis_uuid_vector = chassis_manager.get_keys(manager_uuid);
    ASSERT_FALSE(chassis_uuid_vector.empty()) << "There is no Chassis.";

    for (const auto& chassis_uuid : chassis_uuid_vector) {
        auto chassis = chassis_manager.get_entry(chassis_uuid);
        auto fru_info = chassis.get_fru_info();
        EXPECT_STREQ(fru_info.get_model_number().value().c_str(), mock::PRODUCT_ID_INTEL_ATOM)
                        << "Chassis product id is not set.";
    }
}
