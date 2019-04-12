/*!
 * @brief Unit tests for generation of UUIDv5
 *
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
 *
 * @file compute_tree_stabilizer_test.cpp
 * */


#include "tree_stability/compute_tree_stabilizer.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "gtest/gtest.h"

#include <string>



using namespace agent::compute;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;

namespace constants {

// Service UUID
constexpr const char SERIVCE_UUID[] = "e784d192-379c-11e6-bc47-0242ac110002";

// System BMC GUID
constexpr const char SYSTEM_BMC_GUID[] = "system_bmc_guid";

// CPU slots names
constexpr const char CPU_1_SOCKET[] = "cpu_1_socket";
constexpr const char CPU_2_SOCKET[] = "cpu_2_socket";

// Device locators for DIMMs
constexpr const char DEVICE_LOCATOR_1[] = "device_locator_1";
constexpr const char DEVICE_LOCATOR_2[] = "device_locator_2";

// System NIC MAC address
constexpr const char SYSTEM_NIC_MAC[] = "aa:bb:cc:dd:ee:ff";

// Expected values of generated persistent uuids
constexpr const char MANAGER_PERSISTENT_UUID[] = "baeaa71f-f782-5f77-bc6b-a9f6a6399edc";
constexpr const char SYSTEM_PERSISTENT_UUID[] = "5a49c79e-454b-5b0d-a6a4-15d7e96a5d71";
constexpr const char CHASSIS_PERSISTENT_UUID[] = "c55cae82-cb89-5cc9-9ece-f0ad145a81d1";
constexpr const char PROCESSOR_1_PERSISTENT_UUID[] = "59ad7dfe-1190-5c8a-8f48-9d6c7686bc76";
constexpr const char PROCESSOR_2_PERSISTENT_UUID[] = "f1965e43-1464-5b74-8907-3a99fbd714ab";
constexpr const char MEMORY_1_PERSISTENT_UUID[] = "72912650-0ce8-5280-964c-ef282abebedf";
constexpr const char MEMORY_2_PERSISTENT_UUID[] = "32672fcd-f84c-52e5-aa28-72b89dbcf7c1";
constexpr const char NIC_PERSISTENT_UUID[] = "09b6391d-55f9-55b0-8475-74a08b818ca4";
constexpr const char STORAGE_SUBSYSTEM_PERSISTENT_UUID[] = "be0d248c-f6d4-55ff-9938-1cb2fb1191b3";
constexpr const char NETWORK_DEVICE_PERSISTENT_UUID[] = "cb54c83b-f5b9-5324-be7d-b74649539241";
constexpr const char NETWORK_DEVICE_FUNCTION_PERSISTENT_UUID[] = "714bc415-6c48-50da-8cb6-3aee99dcd3e3";

}

class ComputeTreeStabilizerTest : public ::testing::Test {
public:
    virtual ~ComputeTreeStabilizerTest();


    virtual void SetUp();


    virtual void TearDown();
};


ComputeTreeStabilizerTest::~ComputeTreeStabilizerTest() { }


void ComputeTreeStabilizerTest::SetUp() {
    ServiceUuid::get_instance()->set_service_uuid(constants::SERIVCE_UUID);

    // Create parent manager for system and chassis
    Manager manager{};
    const std::string module_uuid = manager.get_uuid();
    manager.set_guid(constants::SYSTEM_BMC_GUID);

    // Create chassis as child of manager
    Chassis chassis{manager.get_uuid()};
    manager.set_location(chassis.get_uuid());

    // Create system as child of manager and set its unique property
    System system{manager.get_uuid()};
    system.set_guid(constants::SYSTEM_BMC_GUID);
    system.set_chassis(chassis.get_uuid());


    // Create two processors as children of system and set their unique properties
    Processor processor1{system.get_uuid()};
    Processor processor2{system.get_uuid()};

    processor1.set_socket(constants::CPU_1_SOCKET);
    processor2.set_socket(constants::CPU_2_SOCKET);

    // Create two memories as children of system and set their unique properties
    Memory memory1{system.get_uuid()};
    Memory memory2{system.get_uuid()};

    memory1.set_device_locator(constants::DEVICE_LOCATOR_1);
    memory2.set_device_locator(constants::DEVICE_LOCATOR_2);

    // Create network interface as child of system and set its unique property
    NetworkInterface nic{system.get_uuid()};
    nic.set_mac_address(constants::SYSTEM_NIC_MAC);

    // Create storage subsystem as a child of the system
    StorageSubsystem storage{system.get_uuid()};

    //Create network device as a child of the system
    NetworkDevice network_device{system.get_uuid()};

    //Create network device function as a child of the system
    NetworkDeviceFunction function{network_device.get_uuid()};

    get_manager<agent_framework::model::Manager>().add_entry(manager);
    get_manager<agent_framework::model::Chassis>().add_entry(chassis);
    get_manager<agent_framework::model::System>().add_entry(system);
    get_manager<agent_framework::model::Processor>().add_entry(processor1);
    get_manager<agent_framework::model::Processor>().add_entry(processor2);
    get_manager<agent_framework::model::Memory>().add_entry(memory1);
    get_manager<agent_framework::model::Memory>().add_entry(memory2);
    get_manager<agent_framework::model::NetworkInterface>().add_entry(nic);
    get_manager<agent_framework::model::StorageSubsystem>().add_entry(storage);
    get_manager<agent_framework::model::NetworkDevice>().add_entry(network_device);
    get_manager<agent_framework::model::NetworkDeviceFunction>().add_entry(function);

    // Stabilize mock resource tree
    ComputeTreeStabilizer().stabilize(module_uuid);
}


void ComputeTreeStabilizerTest::TearDown() {
    get_manager<agent_framework::model::Manager>().clear_entries();
    get_manager<agent_framework::model::Chassis>().clear_entries();
    get_manager<agent_framework::model::System>().clear_entries();
    get_manager<agent_framework::model::Processor>().clear_entries();
    get_manager<agent_framework::model::Memory>().clear_entries();
    get_manager<agent_framework::model::NetworkInterface>().clear_entries();
    get_manager<agent_framework::model::StorageSubsystem>().clear_entries();
    get_manager<agent_framework::model::NetworkDevice>().clear_entries();
    get_manager<agent_framework::model::NetworkDeviceFunction>().clear_entries();
}


TEST_F(ComputeTreeStabilizerTest, NoNewOrDeletedResourcesTest) {
    // Assert that all resources are still present in the resource tree
    ASSERT_EQ(1, get_manager<agent_framework::model::Manager>().get_keys().size());
    ASSERT_EQ(1, get_manager<agent_framework::model::System>().get_keys().size());
    ASSERT_EQ(1, get_manager<agent_framework::model::Chassis>().get_keys().size());
    ASSERT_EQ(2, get_manager<agent_framework::model::Processor>().get_keys().size());
    ASSERT_EQ(2, get_manager<agent_framework::model::Memory>().get_keys().size());
    ASSERT_EQ(1, get_manager<agent_framework::model::NetworkInterface>().get_keys().size());
    ASSERT_EQ(1, get_manager<agent_framework::model::StorageSubsystem>().get_keys().size());
    ASSERT_EQ(1, get_manager<agent_framework::model::NetworkDevice>().get_keys().size());
    ASSERT_EQ(1, get_manager<agent_framework::model::NetworkDeviceFunction>().get_keys().size());
}


TEST_F(ComputeTreeStabilizerTest, PersistentUUIDsGeneratedTest) {

    log_debug("default", get_manager<agent_framework::model::NetworkDevice>().get_keys().front());
    log_debug("default", get_manager<agent_framework::model::NetworkDeviceFunction>().get_keys().front());

    // Assert that all resources now have persistent UUIDs
    ASSERT_NO_THROW(get_manager<agent_framework::model::Manager>().get_entry(constants::MANAGER_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::Chassis>().get_entry(constants::CHASSIS_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::System>().get_entry(constants::SYSTEM_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::Processor>().get_entry(constants::PROCESSOR_1_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::Processor>().get_entry(constants::PROCESSOR_2_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::Memory>().get_entry(constants::MEMORY_1_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::Memory>().get_entry(constants::MEMORY_2_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::NetworkInterface>().get_entry(constants::NIC_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::StorageSubsystem>().get_entry(constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::NetworkDevice>().get_entry(constants::NETWORK_DEVICE_PERSISTENT_UUID));
    ASSERT_NO_THROW(get_manager<agent_framework::model::NetworkDeviceFunction>().get_entry(constants::NETWORK_DEVICE_FUNCTION_PERSISTENT_UUID));

}


TEST_F(ComputeTreeStabilizerTest, ParentUUIDChangedTest) {
    // Assert that the system parent is correct
    ASSERT_EQ(1, get_manager<agent_framework::model::System>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());

    // Assert that the chassis parent is correct
    ASSERT_EQ(1, get_manager<agent_framework::model::Chassis>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());

    // Assert that resources under system have correct parent UUID
    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::Processor>()
        .get_entry(constants::PROCESSOR_1_PERSISTENT_UUID).get_parent_uuid());
    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::Processor>()
        .get_entry(constants::PROCESSOR_1_PERSISTENT_UUID).get_parent_uuid());

    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::Memory>()
        .get_entry(constants::MEMORY_1_PERSISTENT_UUID).get_parent_uuid());
    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::Memory>()
        .get_entry(constants::MEMORY_2_PERSISTENT_UUID).get_parent_uuid());

    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::NetworkInterface>()
        .get_entry(constants::NIC_PERSISTENT_UUID).get_parent_uuid());
    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::StorageSubsystem>()
        .get_entry(constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID).get_parent_uuid());

    ASSERT_EQ(std::string(constants::SYSTEM_PERSISTENT_UUID), get_manager<agent_framework::model::NetworkDevice>()
        .get_entry(constants::NETWORK_DEVICE_PERSISTENT_UUID).get_parent_uuid());
    ASSERT_EQ(std::string(constants::NETWORK_DEVICE_PERSISTENT_UUID),
              get_manager<agent_framework::model::NetworkDeviceFunction>()
                  .get_entry(constants::NETWORK_DEVICE_FUNCTION_PERSISTENT_UUID).get_parent_uuid());
}

TEST_F(ComputeTreeStabilizerTest, ManagerLocationTest) {
    // After stabilization, manager should be located in persistent chassis.
    ASSERT_TRUE(get_manager<agent_framework::model::Manager>()
        .get_entry(constants::MANAGER_PERSISTENT_UUID).get_location().has_value());
    ASSERT_EQ(std::string(constants::CHASSIS_PERSISTENT_UUID), get_manager<agent_framework::model::Manager>()
        .get_entry(constants::MANAGER_PERSISTENT_UUID).get_location().value());
}
