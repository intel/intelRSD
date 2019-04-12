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


#include "tree_stability/pnc_tree_stabilizer.hpp"
#include "tree_stability/pnc_stabilizer.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "gtest/gtest.h"

#include <string>
#include <iostream>


using namespace agent::pnc;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;

namespace constants {

// Service UUID
constexpr char SERIVCE_UUID[] = "e784d192-379c-11e6-bc47-0242ac110002";

// PCIe switch serial number
constexpr char PCIE_SWITCH_SERIAL_NUMBER[] = "pcie_switch_serial";

// Drive serial numbers
constexpr char DRIVE_1_SERIAL_NUMBER[] = "drive_1_serial";
constexpr char DRIVE_2_SERIAL_NUMBER[] = "drive_2_serial";

// Devices serial numbers
constexpr char DEVICE_1_SERIAL_NUMBER[] = "device_1_serial";
constexpr char DEVICE_2_SERIAL_NUMBER[] = "device_2_serial";

// Function id
constexpr char FUNCTION_ID[] = "0";

// Port id
constexpr char PORT_1_ID[] = "port_1_id";
constexpr char PORT_2_ID[] = "port_2_id";

// Zone id
constexpr unsigned int ZONE_1_ID = 1;
constexpr unsigned int ZONE_2_ID = 2;

// Expected values of generated persistent UUIDs
constexpr char MANAGER_PERSISTENT_UUID[] = "4fe52b8b-0bc8-53ca-9dce-34f1607683db";
constexpr char FABRIC_PERSISTENT_UUID[] = "930654e7-4741-54e9-86db-70c65f827d09";
constexpr char SWITCH_PERSISTENT_UUID[] = "539968b8-bad3-5fb8-bd97-7babf7f901b3";
constexpr char PORT_1_PERSISTENT_UUID[] = "ed44bdaa-fd53-51e5-ab21-399202b5c719";
constexpr char PORT_2_PERSISTENT_UUID[] = "13515074-87cd-5360-998f-4529ec0a311a";
constexpr char ZONE_1_PERSISTENT_UUID[] = "11eff206-a0a4-5317-88dc-2ceb58e0b06c";
constexpr char ZONE_2_PERSISTENT_UUID[] = "ae6aec3f-d469-5a1f-91b9-703bf1358967";
constexpr char ENDPOINT_1_PERSISTENT_UUID[] = "f7b641d4-fb4a-5dfb-98ac-654f9d5ece84";
constexpr char ENDPOINT_2_PERSISTENT_UUID[] = "e6899df1-f7bb-56e4-b67a-713eff178b20";
constexpr char CHASSIS_PERSISTENT_UUID[] = "70ef5e73-a18c-526e-96e3-88fb00550ff5";
constexpr char DRIVE_1_PERSISTENT_UUID[] = "37f7b3e9-9819-529b-9ad0-a71035e497e3";
constexpr char DRIVE_2_PERSISTENT_UUID[] = "6d0f6d97-2a2a-58a5-ae6f-d9a086d99976";
constexpr char SYSTEM_PERSISTENT_UUID[] = "05c1d53c-f920-584f-99f4-c76f9fcf7b41";
constexpr char STORAGE_SUBSYSTEM_PERSISTENT_UUID[] = "924f59d6-c136-5e14-aca1-c758dc606f70";
constexpr char PCIE_DEVICE_1_PERSISTENT_UUID[] = "f77a507d-cff1-5827-a64f-ee5b5b94c6ce";
constexpr char PCIE_DEVICE_2_PERSISTENT_UUID[] = "e786c9b1-6f3a-534b-b863-28dcf6d7f42c";
constexpr char PCIE_FUNCTION_1_PERSISTENT_UUID[] = "bf37d3ad-bf5f-5667-afc7-1f0d137042c6";
constexpr char PCIE_FUNCTION_2_PERSISTENT_UUID[] = "d50f7171-f172-5a46-9d2f-57f1d13a7328";

}

class PncTreeStabilityTest : public ::testing::Test {
public:
    virtual ~PncTreeStabilityTest();


    virtual void SetUp();


    virtual void TearDown();
};


PncTreeStabilityTest::~PncTreeStabilityTest() { }


void PncTreeStabilityTest::SetUp() {
    ServiceUuid::get_instance()->set_service_uuid(constants::SERIVCE_UUID);

    // Create Manager (resource tree root)
    Manager manager{};

    // Create Fabrics
    Fabric fabric{manager.get_uuid()};

    // Create PcieSwitch
    Switch pcie_switch{fabric.get_uuid()};

    // Create Ports
    Port port_1{pcie_switch.get_uuid()};
    Port port_2{pcie_switch.get_uuid()};

    // Create Zones
    Zone zone_1{fabric.get_uuid()};
    Zone zone_2{fabric.get_uuid()};

    // Create Endpoints
    Endpoint endpoint_1{fabric.get_uuid()};
    Endpoint endpoint_2{fabric.get_uuid()};

    // Create Chassis
    Chassis chassis{manager.get_uuid()};

    // Create Drives
    Drive drive_1{chassis.get_uuid()};
    Drive drive_2{chassis.get_uuid()};

    // Create System
    System system{manager.get_uuid()};

    // Create StorageSubsystem
    StorageSubsystem subsystem{system.get_uuid()};

    // Create PcieDevices
    PcieDevice pcie_device_1{manager.get_uuid()};
    PcieDevice pcie_device_2{manager.get_uuid()};

    // Create PcieFunctions
    PcieFunction pcie_function_1{pcie_device_1.get_uuid()};
    PcieFunction pcie_function_2{pcie_device_2.get_uuid()};

    // Set unique properties
    pcie_switch.set_fru_info({constants::PCIE_SWITCH_SERIAL_NUMBER, "", "", ""});

    port_1.set_port_id(constants::PORT_1_ID);
    port_2.set_port_id(constants::PORT_2_ID);

    zone_1.set_zone_id(constants::ZONE_1_ID);
    zone_2.set_zone_id(constants::ZONE_2_ID);

    attribute::ConnectedEntity entity_1;
    entity_1.set_entity(drive_1.get_uuid());
    entity_1.set_entity_role(enums::EntityRole::Initiator);
    endpoint_1.add_connected_entity(entity_1);

    attribute::ConnectedEntity entity_2;
    entity_2.set_entity(drive_2.get_uuid());
    entity_2.set_entity_role(enums::EntityRole::Target);
    endpoint_2.add_connected_entity(entity_2);

    drive_1.set_fru_info({constants::DRIVE_1_SERIAL_NUMBER, "", "", ""});
    drive_2.set_fru_info({constants::DRIVE_2_SERIAL_NUMBER, "", "", ""});

    pcie_device_1.set_fru_info({constants::DEVICE_1_SERIAL_NUMBER, "", "", ""});
    pcie_device_2.set_fru_info({constants::DEVICE_2_SERIAL_NUMBER, "", "", ""});

    pcie_function_1.set_function_id(constants::FUNCTION_ID);
    pcie_function_2.set_function_id(constants::FUNCTION_ID);
    pcie_function_1.set_functional_device(drive_1.get_uuid());
    pcie_function_2.set_functional_device(drive_2.get_uuid());

    // Take care of all relations in the model
    pcie_switch.set_chassis(chassis.get_uuid());

    zone_1.set_switch_uuid(pcie_switch.get_uuid());
    get_m2m_manager<Zone, Endpoint>().add_entry(zone_1.get_uuid(), endpoint_1.get_uuid());

    zone_2.set_switch_uuid(pcie_switch.get_uuid());
    get_m2m_manager<Zone, Endpoint>().add_entry(zone_2.get_uuid(), endpoint_2.get_uuid());

    get_m2m_manager<Endpoint, Port>().add_entry(endpoint_1.get_uuid(), port_1.get_uuid());
    get_m2m_manager<Endpoint, Port>().add_entry(endpoint_2.get_uuid(), port_2.get_uuid());

    get_m2m_manager<Drive, PcieFunction>().add_entry(drive_1.get_uuid(), pcie_function_1.get_uuid());
    get_m2m_manager<Drive, PcieFunction>().add_entry(drive_2.get_uuid(), pcie_function_2.get_uuid());

    system.set_chassis(chassis.get_uuid());

    pcie_device_1.set_chassis(chassis.get_uuid());
    pcie_device_2.set_chassis(chassis.get_uuid());

    pcie_function_1.set_dsp_port_uuid(port_1.get_uuid());
    get_m2m_manager<Drive, PcieFunction>().add_entry(drive_1.get_uuid(), pcie_function_1.get_uuid());

    pcie_function_2.set_dsp_port_uuid(port_2.get_uuid());
    get_m2m_manager<Drive, PcieFunction>().add_entry(drive_2.get_uuid(), pcie_function_2.get_uuid());

    get_m2m_manager<StorageSubsystem, Drive>().add_entry(subsystem.get_uuid(), drive_1.get_uuid());
    get_m2m_manager<StorageSubsystem, Drive>().add_entry(subsystem.get_uuid(), drive_2.get_uuid());

    // Add all objects to the model
    get_manager<Manager>().add_entry(manager);
    get_manager<Fabric>().add_entry(fabric);
    get_manager<Switch>().add_entry(pcie_switch);
    get_manager<Port>().add_entry(port_1);
    get_manager<Port>().add_entry(port_2);
    get_manager<Zone>().add_entry(zone_1);
    get_manager<Zone>().add_entry(zone_2);
    get_manager<Endpoint>().add_entry(endpoint_1);
    get_manager<Endpoint>().add_entry(endpoint_2);
    get_manager<Chassis>().add_entry(chassis);
    get_manager<Drive>().add_entry(drive_1);
    get_manager<Drive>().add_entry(drive_2);
    get_manager<System>().add_entry(system);
    get_manager<StorageSubsystem>().add_entry(subsystem);
    get_manager<PcieDevice>().add_entry(pcie_device_1);
    get_manager<PcieDevice>().add_entry(pcie_device_2);
    get_manager<PcieFunction>().add_entry(pcie_function_1);
    get_manager<PcieFunction>().add_entry(pcie_function_2);

    PncTreeStabilizer().stabilize(manager.get_uuid());
}


void PncTreeStabilityTest::TearDown() {
    get_manager<Manager>().clear_entries();
    get_manager<Fabric>().clear_entries();
    get_manager<Switch>().clear_entries();
    get_manager<Port>().clear_entries();
    get_manager<Zone>().clear_entries();
    get_manager<Endpoint>().clear_entries();
    get_manager<Chassis>().clear_entries();
    get_manager<Drive>().clear_entries();
    get_manager<System>().clear_entries();
    get_manager<StorageSubsystem>().clear_entries();
    get_manager<PcieDevice>().clear_entries();
    get_manager<PcieFunction>().clear_entries();
    get_m2m_manager<StorageSubsystem, Drive>().clear_entries();
    get_m2m_manager<Zone, Endpoint>().clear_entries();
    get_m2m_manager<Endpoint, Port>().clear_entries();
    get_m2m_manager<Drive, PcieFunction>().clear_entries();
}


TEST_F(PncTreeStabilityTest, NoNewOrDeletedResourcesTest) {
    // Assert that all resources are still present in the resource tree

    ASSERT_EQ(1, get_manager<Manager>().get_keys().size());
    ASSERT_EQ(1, get_manager<Fabric>().get_keys().size());
    ASSERT_EQ(1, get_manager<Switch>().get_keys().size());
    ASSERT_EQ(2, get_manager<Port>().get_keys().size());
    ASSERT_EQ(2, get_manager<Zone>().get_keys().size());
    ASSERT_EQ(2, get_manager<Endpoint>().get_keys().size());
    ASSERT_EQ(1, get_manager<Chassis>().get_keys().size());
    ASSERT_EQ(2, get_manager<Drive>().get_keys().size());
    ASSERT_EQ(1, get_manager<System>().get_keys().size());
    ASSERT_EQ(1, get_manager<StorageSubsystem>().get_keys().size());
    ASSERT_EQ(2, get_manager<PcieDevice>().get_keys().size());
    ASSERT_EQ(2, get_manager<PcieFunction>().get_keys().size());
}


TEST_F(PncTreeStabilityTest, PersistentUUIDsGeneratedTest) {
    // Assert that all resources now have persistent UUIDs
    ASSERT_TRUE(get_manager<Manager>().entry_exists(constants::MANAGER_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Fabric>().entry_exists(constants::FABRIC_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Switch>().entry_exists(constants::SWITCH_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Port>().entry_exists(constants::PORT_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Port>().entry_exists(constants::PORT_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Zone>().entry_exists(constants::ZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Zone>().entry_exists(constants::ZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Endpoint>().entry_exists(constants::ENDPOINT_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Endpoint>().entry_exists(constants::ENDPOINT_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::CHASSIS_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Drive>().entry_exists(constants::DRIVE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Drive>().entry_exists(constants::DRIVE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<System>().entry_exists(constants::SYSTEM_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<StorageSubsystem>().entry_exists(constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<PcieDevice>().entry_exists(constants::PCIE_DEVICE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<PcieDevice>().entry_exists(constants::PCIE_DEVICE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<PcieFunction>().entry_exists(constants::PCIE_FUNCTION_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<PcieFunction>().entry_exists(constants::PCIE_FUNCTION_2_PERSISTENT_UUID));
}


TEST_F(PncTreeStabilityTest, ParentUUIDChangedTest) {
    // Assert that the PCIe switch parent is correct
    ASSERT_EQ(1, get_manager<Fabric>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<Chassis>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<System>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<PcieDevice>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());

    ASSERT_EQ(1, get_manager<Switch>().get_keys(constants::FABRIC_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<Zone>().get_keys(constants::FABRIC_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<Endpoint>().get_keys(constants::FABRIC_PERSISTENT_UUID).size());

    ASSERT_EQ(2, get_manager<Port>().get_keys(constants::SWITCH_PERSISTENT_UUID).size());

    ASSERT_EQ(2, get_manager<Drive>().get_keys(constants::CHASSIS_PERSISTENT_UUID).size());

    ASSERT_EQ(1, get_manager<StorageSubsystem>().get_keys(constants::SYSTEM_PERSISTENT_UUID).size());

    ASSERT_EQ(1, get_manager<PcieFunction>().get_keys(constants::PCIE_DEVICE_1_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<PcieFunction>().get_keys(constants::PCIE_DEVICE_2_PERSISTENT_UUID).size());
}

TEST_F(PncTreeStabilityTest, RelationsUpdatedTest) {
    // Assert that all relations have been succesfully updated
    for (const auto& zone_uuid : get_manager<Zone>().get_keys()) {
        ASSERT_EQ(get_manager<Zone>().get_entry(zone_uuid).get_switch_uuid(), constants::SWITCH_PERSISTENT_UUID);
    }

    ASSERT_TRUE((get_m2m_manager<Zone,Endpoint>().entry_exists(constants::ZONE_1_PERSISTENT_UUID, constants::ENDPOINT_1_PERSISTENT_UUID)));
    ASSERT_TRUE((get_m2m_manager<Zone,Endpoint>().entry_exists(constants::ZONE_2_PERSISTENT_UUID, constants::ENDPOINT_2_PERSISTENT_UUID)));

    ASSERT_TRUE((get_m2m_manager<Endpoint, Port>().entry_exists(constants::ENDPOINT_1_PERSISTENT_UUID, constants::PORT_1_PERSISTENT_UUID)));
    ASSERT_TRUE((get_m2m_manager<Endpoint, Port>().entry_exists(constants::ENDPOINT_2_PERSISTENT_UUID, constants::PORT_2_PERSISTENT_UUID)));

    ASSERT_EQ(get_manager<PcieFunction>().get_entry(constants::PCIE_FUNCTION_1_PERSISTENT_UUID).get_dsp_port_uuid(), constants::PORT_1_PERSISTENT_UUID);
    ASSERT_EQ(get_manager<PcieFunction>().get_entry(constants::PCIE_FUNCTION_2_PERSISTENT_UUID).get_dsp_port_uuid(), constants::PORT_2_PERSISTENT_UUID);

    ASSERT_EQ(get_manager<Switch>().get_entry(constants::SWITCH_PERSISTENT_UUID).get_chassis(), constants::CHASSIS_PERSISTENT_UUID);
    ASSERT_EQ(get_manager<System>().get_entry(constants::SYSTEM_PERSISTENT_UUID).get_chassis(), constants::CHASSIS_PERSISTENT_UUID);

    for (const auto& device_uuid : get_manager<PcieDevice>().get_keys()) {
        ASSERT_EQ(get_manager<PcieDevice>().get_entry(device_uuid).get_chassis(), constants::CHASSIS_PERSISTENT_UUID);
    }

    ASSERT_TRUE((get_m2m_manager<Drive, PcieFunction>().entry_exists(constants::DRIVE_1_PERSISTENT_UUID, constants::PCIE_FUNCTION_1_PERSISTENT_UUID)));
    ASSERT_TRUE((get_m2m_manager<Drive, PcieFunction>().entry_exists(constants::DRIVE_2_PERSISTENT_UUID, constants::PCIE_FUNCTION_2_PERSISTENT_UUID)));

    ASSERT_TRUE((get_m2m_manager<StorageSubsystem, Drive>().entry_exists(constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID, constants::DRIVE_1_PERSISTENT_UUID)));
    ASSERT_TRUE((get_m2m_manager<StorageSubsystem, Drive>().entry_exists(constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID, constants::DRIVE_2_PERSISTENT_UUID)));
}

TEST_F(PncTreeStabilityTest, DryStability) {
    auto manager = get_manager<Manager>().get_entry(constants::MANAGER_PERSISTENT_UUID);
    auto fabric = get_manager<Fabric>().get_entry(constants::FABRIC_PERSISTENT_UUID);
    auto pcie_switch = get_manager<Switch>().get_entry(constants::SWITCH_PERSISTENT_UUID);
    auto port_1 = get_manager<Port>().get_entry(constants::PORT_1_PERSISTENT_UUID);
    auto port_2 = get_manager<Port>().get_entry(constants::PORT_2_PERSISTENT_UUID);
    auto zone_1 = get_manager<Zone>().get_entry(constants::ZONE_1_PERSISTENT_UUID);
    auto zone_2 = get_manager<Zone>().get_entry(constants::ZONE_2_PERSISTENT_UUID);
    auto endpoint_1 = get_manager<Endpoint>().get_entry(constants::ENDPOINT_1_PERSISTENT_UUID);
    auto endpoint_2 = get_manager<Endpoint>().get_entry(constants::ENDPOINT_2_PERSISTENT_UUID);
    auto chassis = get_manager<Chassis>().get_entry(constants::CHASSIS_PERSISTENT_UUID);
    auto drive_1 = get_manager<Drive>().get_entry(constants::DRIVE_1_PERSISTENT_UUID);
    auto drive_2 = get_manager<Drive>().get_entry(constants::DRIVE_2_PERSISTENT_UUID);
    auto system = get_manager<System>().get_entry(constants::SYSTEM_PERSISTENT_UUID);
    auto storage_subsystem = get_manager<StorageSubsystem>().get_entry(constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID);
    auto pcie_device_1 = get_manager<PcieDevice>().get_entry(constants::PCIE_DEVICE_1_PERSISTENT_UUID);
    auto pcie_device_2 = get_manager<PcieDevice>().get_entry(constants::PCIE_DEVICE_2_PERSISTENT_UUID);
    auto function_1 = get_manager<PcieFunction>().get_entry(constants::PCIE_FUNCTION_1_PERSISTENT_UUID);
    auto function_2 = get_manager<PcieFunction>().get_entry(constants::PCIE_FUNCTION_2_PERSISTENT_UUID);

    manager.make_random_uuid();
    fabric.make_random_uuid();
    pcie_switch.make_random_uuid();
    port_1.make_random_uuid();
    port_2.make_random_uuid();
    zone_1.make_random_uuid();
    zone_2.make_random_uuid();
    endpoint_1.make_random_uuid();
    endpoint_2.make_random_uuid();
    chassis.make_random_uuid();
    drive_1.make_random_uuid();
    drive_2.make_random_uuid();
    system.make_random_uuid();
    storage_subsystem.make_random_uuid();
    pcie_device_1.make_random_uuid();
    pcie_device_2.make_random_uuid();
    function_1.make_random_uuid();
    function_2.make_random_uuid();

    PncStabilizer stabilizer;
    ASSERT_EQ(stabilizer.dry_stabilize(manager, pcie_switch), constants::MANAGER_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(fabric), constants::FABRIC_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(pcie_switch), constants::SWITCH_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(port_1, pcie_switch), constants::PORT_1_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(port_2, pcie_switch), constants::PORT_2_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(zone_1, pcie_switch), constants::ZONE_1_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(zone_2, pcie_switch), constants::ZONE_2_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(endpoint_1, std::vector<Port>{port_1}), constants::ENDPOINT_1_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(endpoint_2, std::vector<Port>{port_2}), constants::ENDPOINT_2_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(chassis, pcie_switch), constants::CHASSIS_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(drive_1), constants::DRIVE_1_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(drive_2), constants::DRIVE_2_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(system), constants::SYSTEM_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(storage_subsystem), constants::STORAGE_SUBSYSTEM_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(pcie_device_1), constants::PCIE_DEVICE_1_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(pcie_device_2), constants::PCIE_DEVICE_2_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(function_1, pcie_device_1), constants::PCIE_FUNCTION_1_PERSISTENT_UUID);
    ASSERT_EQ(stabilizer.dry_stabilize(function_2, pcie_device_2), constants::PCIE_FUNCTION_2_PERSISTENT_UUID);

}
