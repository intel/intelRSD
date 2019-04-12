/*!
 * @brief Unit tests for generation of UUIDv5
 *
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
 *
 * @file compute_tree_stabilizer_test.cpp
 * */


#include "tree_stability/rmm_tree_stabilizer.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "gtest/gtest.h"

#include <string>



using namespace agent::rmm;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;

namespace constants {

// Service UUID
constexpr char SERIVCE_UUID[] = "e784d192-379c-11e6-bc47-0242ac110002";

constexpr char RACK_NIC_MAC[] = "aa:bb:cc:dd:ee:ff";
constexpr std::uint32_t RACK_NIC_VLAN_ID = 2;

constexpr char DRAWER_1_ZONE_1_SERIAL_NUMBER[] = "1111-1111";
constexpr char DRAWER_2_ZONE_1_SERIAL_NUMBER[] = "2222-1111";
constexpr char DRAWER_1_ZONE_2_SERIAL_NUMBER[] = "1111-2222";
constexpr char DRAWER_2_ZONE_2_SERIAL_NUMBER[] = "2222-2222";

constexpr char ZONE_1_NIC_MAC[] = "aa:aa:aa:aa:aa:aa";
constexpr char ZONE_2_NIC_MAC[] = "bb:bb:bb:bb:bb:bb";

constexpr char PSU_1_PZONE_1_SERIAL_NUMBER[] = "psu_1_pzone_1";
constexpr char PSU_2_PZONE_1_SERIAL_NUMBER[] = "psu_2_pzone_1";
constexpr char PSU_1_PZONE_2_SERIAL_NUMBER[] = "psu_1_pzone_2";
constexpr char PSU_2_PZONE_2_SERIAL_NUMBER[] = "psu_2_pzone_2";

constexpr std::uint8_t FAN_1_TZONE_1_SLOT_ID = 1u;
constexpr std::uint8_t FAN_2_TZONE_1_SLOT_ID = 2u;
constexpr std::uint8_t FAN_1_TZONE_2_SLOT_ID = 3u;
constexpr std::uint8_t FAN_2_TZONE_2_SLOT_ID = 4u;

constexpr char RACK_MANAGER_PERSISTENT_UUID[] = "49fab30b-acdc-53dd-9154-4536fe09cf04";
constexpr char ZONE_1_MANAGER_PERSISTENT_UUID[] = "87036ec1-536d-5b2f-8ce7-7543179d28f2";
constexpr char ZONE_2_MANAGER_PERSISTENT_UUID[] = "69fd43ff-ded6-5290-98cd-96a87f361c37";
constexpr char RACK_PERSISTENT_UUID[] = "ae5a6592-c43a-5573-b570-4f065c9ce474";
constexpr char ZONE_1_PERSISTENT_UUID[] = "eebaa003-ae56-5953-83e8-985e53ce445c";
constexpr char ZONE_2_PERSISTENT_UUID[] = "45359153-771c-5139-8aeb-5afb45f2a1e9";
constexpr char DRAWER_1_ZONE_1_PERSISTENT_UUID[] = "db129d76-eef8-58dd-82f3-76fc980544bd";
constexpr char DRAWER_2_ZONE_1_PERSISTENT_UUID[] = "7f8b514f-d8ed-5e24-99d2-4a80e37f9b86";
constexpr char DRAWER_1_ZONE_2_PERSISTENT_UUID[] = "0cde350e-6078-5be6-b6f0-1cd5dae2bcdf";
constexpr char DRAWER_2_ZONE_2_PERSISTENT_UUID[] = "affed717-00a5-5a78-a4d1-3b9e8d224ed6";
constexpr char RACK_NETWORK_INTERFACE_PERSISTENT_UUID[] = "b4a30212-a003-5993-9f9b-d0c18c4bde92";
constexpr char ZONE_1_NETWORK_INTERFACE_PERSISTENT_UUID[] = "02d3d045-9a1e-54fb-9c2f-55c097cfe0ae";
constexpr char ZONE_2_NETWORK_INTERFACE_PERSISTENT_UUID[] = "c38f16ce-a039-55cc-b806-18737a6d4060";
constexpr char RACK_VLAN_PERSISTENT_UUID[] = "84f16143-8690-5c5c-8944-abbc8ee22125";
constexpr char POWER_ZONE_1_PERSISTENT_UUID[] = "e7909347-8291-501e-8f80-6acad0b37cf9";
constexpr char POWER_ZONE_2_PERSISTENT_UUID[] = "d49a2ca5-5428-5da0-ab89-27cef001e86c";
constexpr char PSU_1_PZONE_1_PERSISTENT_UUID[] = "b6e5de40-7909-51b6-8a4b-c3f389cf3538";
constexpr char PSU_2_PZONE_1_PERSISTENT_UUID[] = "4fe9a6d3-608f-5ba8-b1a7-d5bf2bce6342";
constexpr char PSU_1_PZONE_2_PERSISTENT_UUID[] = "6fe1e60f-0c87-5584-9cc7-96ef270935e7";
constexpr char PSU_2_PZONE_2_PERSISTENT_UUID[] = "c7e9dbd0-f61c-5b6a-ba81-8b74424c8575";
constexpr char THERMAL_ZONE_1_PERSISTENT_UUID[] = "b3fff377-ee98-5da3-bc38-cc0c8e7077e5";
constexpr char THERMAL_ZONE_2_PERSISTENT_UUID[] = "37f2fc6e-c32b-50a7-a681-8d9c20817797";
constexpr char FAN_1_TZONE_1_PERSISTENT_UUID[] = "61cb8879-a033-53ff-a279-59ed297e5670";
constexpr char FAN_2_TZONE_1_PERSISTENT_UUID[] = "5092f209-6e7a-5499-8f82-079c1492c3db";
constexpr char FAN_1_TZONE_2_PERSISTENT_UUID[] = "f425ec05-7d9d-5ff5-8902-5d54627f05e7";
constexpr char FAN_2_TZONE_2_PERSISTENT_UUID[] = "fd9a1a7d-f0fc-5ebd-8553-ca2505e73460";

}

class RmmTreeStabilityTest : public ::testing::Test {
public:
    virtual ~RmmTreeStabilityTest();


    virtual void SetUp();


    virtual void TearDown();
};


RmmTreeStabilityTest::~RmmTreeStabilityTest() {}


void RmmTreeStabilityTest::SetUp() {
    // set service uuid
    ServiceUuid::get_instance()->set_service_uuid(constants::SERIVCE_UUID);


    // create all resources
    Manager rack_manager{};
    Manager zone_1_manager{};
    Manager zone_2_manager{};

    Chassis rack{rack_manager.get_uuid()};
    Chassis zone_1{zone_1_manager.get_uuid()};
    Chassis zone_2{zone_2_manager.get_uuid()};
    Chassis drawer_1_zone_1{rack_manager.get_uuid()};
    Chassis drawer_2_zone_1{rack_manager.get_uuid()};
    Chassis drawer_1_zone_2{rack_manager.get_uuid()};
    Chassis drawer_2_zone_2{rack_manager.get_uuid()};

    NetworkInterface rack_nic{rack_manager.get_uuid()};
    NetworkInterface zone_1_nic{zone_1_manager.get_uuid()};
    NetworkInterface zone_2_nic{zone_2_manager.get_uuid()};

    EthernetSwitchPortVlan rack_vlan{rack_nic.get_uuid()};

    PowerZone pzone_1{zone_1.get_uuid()};
    PowerZone pzone_2{zone_2.get_uuid()};

    ThermalZone tzone_1{zone_1.get_uuid()};
    ThermalZone tzone_2{zone_2.get_uuid()};

    Psu psu_1_pzone_1{pzone_1.get_uuid()};
    Psu psu_2_pzone_1{pzone_1.get_uuid()};
    Psu psu_1_pzone_2{pzone_2.get_uuid()};
    Psu psu_2_pzone_2{pzone_2.get_uuid()};

    Fan fan_1_tzone_1{tzone_1.get_uuid()};
    Fan fan_2_tzone_1{tzone_1.get_uuid()};
    Fan fan_1_tzone_2{tzone_2.get_uuid()};
    Fan fan_2_tzone_2{tzone_2.get_uuid()};

    // set unique properties
    rack.set_type(enums::ChassisType::Rack);

    zone_1.set_type(enums::ChassisType::Zone);
    zone_2.set_type(enums::ChassisType::Zone);

    drawer_1_zone_1.set_fru_info({constants::DRAWER_1_ZONE_1_SERIAL_NUMBER, "", "", ""});
    drawer_2_zone_1.set_fru_info({constants::DRAWER_2_ZONE_1_SERIAL_NUMBER, "", "", ""});
    drawer_1_zone_2.set_fru_info({constants::DRAWER_1_ZONE_2_SERIAL_NUMBER, "", "", ""});
    drawer_2_zone_2.set_fru_info({constants::DRAWER_2_ZONE_2_SERIAL_NUMBER, "", "", ""});

    drawer_1_zone_1.set_type(enums::ChassisType::Drawer);
    drawer_2_zone_1.set_type(enums::ChassisType::Drawer);
    drawer_1_zone_2.set_type(enums::ChassisType::Drawer);
    drawer_2_zone_2.set_type(enums::ChassisType::Drawer);

    rack_nic.set_mac_address(constants::RACK_NIC_MAC);

    rack_vlan.set_vlan_id(constants::RACK_NIC_VLAN_ID);

    zone_1_nic.set_mac_address(constants::ZONE_1_NIC_MAC);
    zone_2_nic.set_mac_address(constants::ZONE_2_NIC_MAC);

    psu_1_pzone_1.set_fru_info({constants::PSU_1_PZONE_1_SERIAL_NUMBER, "", "", ""});
    psu_2_pzone_1.set_fru_info({constants::PSU_2_PZONE_1_SERIAL_NUMBER, "", "", ""});
    psu_1_pzone_2.set_fru_info({constants::PSU_1_PZONE_2_SERIAL_NUMBER, "", "", ""});
    psu_2_pzone_2.set_fru_info({constants::PSU_2_PZONE_2_SERIAL_NUMBER, "", "", ""});

    fan_1_tzone_1.set_slot_id(constants::FAN_1_TZONE_1_SLOT_ID);
    fan_2_tzone_1.set_slot_id(constants::FAN_2_TZONE_1_SLOT_ID);
    fan_1_tzone_2.set_slot_id(constants::FAN_1_TZONE_2_SLOT_ID);
    fan_2_tzone_2.set_slot_id(constants::FAN_2_TZONE_2_SLOT_ID);

    // Take care of all relations in the model
    drawer_1_zone_1.set_power_zone(pzone_1.get_uuid());
    drawer_2_zone_1.set_power_zone(pzone_1.get_uuid());
    drawer_1_zone_2.set_power_zone(pzone_2.get_uuid());
    drawer_2_zone_2.set_power_zone(pzone_2.get_uuid());

    drawer_1_zone_1.set_thermal_zone(tzone_1.get_uuid());
    drawer_2_zone_1.set_thermal_zone(tzone_1.get_uuid());
    drawer_1_zone_2.set_thermal_zone(tzone_2.get_uuid());
    drawer_2_zone_2.set_thermal_zone(tzone_2.get_uuid());

    drawer_1_zone_1.set_parent_chassis(zone_1.get_uuid());
    drawer_2_zone_1.set_parent_chassis(zone_1.get_uuid());
    drawer_1_zone_2.set_parent_chassis(zone_2.get_uuid());
    drawer_2_zone_2.set_parent_chassis(zone_2.get_uuid());

    zone_1.set_parent_chassis(rack.get_uuid());
    zone_2.set_parent_chassis(rack.get_uuid());

    rack_manager.set_location(rack.get_uuid());

    zone_1_manager.set_location(zone_1.get_uuid());
    zone_2_manager.set_location(zone_2.get_uuid());

    // Add all objects to the model
    get_manager<Manager>().add_entry(rack_manager);
    get_manager<Manager>().add_entry(zone_1_manager);
    get_manager<Manager>().add_entry(zone_2_manager);
    get_manager<Chassis>().add_entry(rack);
    get_manager<Chassis>().add_entry(zone_1);
    get_manager<Chassis>().add_entry(zone_2);
    get_manager<Chassis>().add_entry(drawer_1_zone_1);
    get_manager<Chassis>().add_entry(drawer_2_zone_1);
    get_manager<Chassis>().add_entry(drawer_1_zone_2);
    get_manager<Chassis>().add_entry(drawer_2_zone_2);
    get_manager<NetworkInterface>().add_entry(rack_nic);
    get_manager<NetworkInterface>().add_entry(zone_1_nic);
    get_manager<NetworkInterface>().add_entry(zone_2_nic);
    get_manager<EthernetSwitchPortVlan>().add_entry(rack_vlan);
    get_manager<PowerZone>().add_entry(pzone_1);
    get_manager<PowerZone>().add_entry(pzone_2);
    get_manager<ThermalZone>().add_entry(tzone_1);
    get_manager<ThermalZone>().add_entry(tzone_2);
    get_manager<Psu>().add_entry(psu_1_pzone_1);
    get_manager<Psu>().add_entry(psu_2_pzone_1);
    get_manager<Psu>().add_entry(psu_1_pzone_2);
    get_manager<Psu>().add_entry(psu_2_pzone_2);
    get_manager<Fan>().add_entry(fan_1_tzone_1);
    get_manager<Fan>().add_entry(fan_2_tzone_1);
    get_manager<Fan>().add_entry(fan_1_tzone_2);
    get_manager<Fan>().add_entry(fan_2_tzone_2);

    RmmTreeStabilizer().stabilize(rack_manager.get_uuid());
    RmmTreeStabilizer().stabilize(zone_1_manager.get_uuid());
    RmmTreeStabilizer().stabilize(zone_2_manager.get_uuid());
}


void RmmTreeStabilityTest::TearDown() {
    get_manager<Manager>().clear_entries();
    get_manager<Chassis>().clear_entries();
    get_manager<NetworkInterface>().clear_entries();
    get_manager<Fan>().clear_entries();
    get_manager<EthernetSwitchPortVlan>().clear_entries();
    get_manager<PowerZone>().clear_entries();
    get_manager<Psu>().clear_entries();
    get_manager<ThermalZone>().clear_entries();
}


TEST_F(RmmTreeStabilityTest, NoNewOrDeletedResourcesTest) {
    // Assert that all resources are still present in the resource tree
    ASSERT_EQ(3, get_manager<Manager>().get_keys().size());
    ASSERT_EQ(7, get_manager<Chassis>().get_keys().size());
    ASSERT_EQ(3, get_manager<NetworkInterface>().get_keys().size());
    ASSERT_EQ(4, get_manager<Fan>().get_keys().size());
    ASSERT_EQ(1, get_manager<EthernetSwitchPortVlan>().get_keys().size());
    ASSERT_EQ(2, get_manager<PowerZone>().get_keys().size());
    ASSERT_EQ(4, get_manager<Psu>().get_keys().size());
    ASSERT_EQ(2, get_manager<ThermalZone>().get_keys().size());
}


TEST_F(RmmTreeStabilityTest, PersistentUUIDsGeneratedTest) {
    // Assert that all resources now have persistent UUIDs
    ASSERT_TRUE(get_manager<Manager>().entry_exists(constants::RACK_MANAGER_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Manager>().entry_exists(constants::ZONE_1_MANAGER_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Manager>().entry_exists(constants::ZONE_2_MANAGER_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::RACK_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::ZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::ZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::DRAWER_1_ZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::DRAWER_2_ZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::DRAWER_1_ZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::DRAWER_2_ZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<NetworkInterface>().entry_exists(constants::RACK_NETWORK_INTERFACE_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<NetworkInterface>().entry_exists(constants::ZONE_1_NETWORK_INTERFACE_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<NetworkInterface>().entry_exists(constants::ZONE_2_NETWORK_INTERFACE_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<EthernetSwitchPortVlan>().entry_exists(constants::RACK_VLAN_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<PowerZone>().entry_exists(constants::POWER_ZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<PowerZone>().entry_exists(constants::POWER_ZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Psu>().entry_exists(constants::PSU_1_PZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Psu>().entry_exists(constants::PSU_2_PZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Psu>().entry_exists(constants::PSU_1_PZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Psu>().entry_exists(constants::PSU_2_PZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<ThermalZone>().entry_exists(constants::THERMAL_ZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<ThermalZone>().entry_exists(constants::THERMAL_ZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Fan>().entry_exists(constants::FAN_1_TZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Fan>().entry_exists(constants::FAN_2_TZONE_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Fan>().entry_exists(constants::FAN_1_TZONE_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Fan>().entry_exists(constants::FAN_2_TZONE_2_PERSISTENT_UUID));
}


TEST_F(RmmTreeStabilityTest, ParentUUIDChangedTest) {
    // Assert that all resources have correct parent UUID set
    ASSERT_EQ(5, get_manager<Chassis>().get_keys(constants::RACK_MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<Chassis>().get_keys(constants::ZONE_1_MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<Chassis>().get_keys(constants::ZONE_2_MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<NetworkInterface>().get_keys(constants::RACK_MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<NetworkInterface>().get_keys(constants::ZONE_1_MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<NetworkInterface>().get_keys(constants::ZONE_2_MANAGER_PERSISTENT_UUID).size());
    ASSERT_EQ(1,
              get_manager<EthernetSwitchPortVlan>().get_keys(constants::RACK_NETWORK_INTERFACE_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<PowerZone>().get_keys(constants::ZONE_1_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<PowerZone>().get_keys(constants::ZONE_2_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<ThermalZone>().get_keys(constants::ZONE_1_PERSISTENT_UUID).size());
    ASSERT_EQ(1, get_manager<ThermalZone>().get_keys(constants::ZONE_2_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<Psu>().get_keys(constants::POWER_ZONE_1_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<Psu>().get_keys(constants::POWER_ZONE_2_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<Fan>().get_keys(constants::THERMAL_ZONE_1_PERSISTENT_UUID).size());
    ASSERT_EQ(2, get_manager<Fan>().get_keys(constants::THERMAL_ZONE_2_PERSISTENT_UUID).size());
}


TEST_F(RmmTreeStabilityTest, RelationsUpdatedTest) {
    // Assert that all relations have been succesfully updated
    auto rack_manager = get_manager<Manager>().get_entry(constants::RACK_MANAGER_PERSISTENT_UUID);
    auto zone_1_manager = get_manager<Manager>().get_entry(constants::ZONE_1_MANAGER_PERSISTENT_UUID);
    auto zone_2_manager = get_manager<Manager>().get_entry(constants::ZONE_2_MANAGER_PERSISTENT_UUID);

    auto zone_1 = get_manager<Chassis>().get_entry(constants::ZONE_1_PERSISTENT_UUID);
    auto zone_2 = get_manager<Chassis>().get_entry(constants::ZONE_2_PERSISTENT_UUID);
    auto drawer_1_zone_1 = get_manager<Chassis>().get_entry(constants::DRAWER_1_ZONE_1_PERSISTENT_UUID);
    auto drawer_2_zone_1 = get_manager<Chassis>().get_entry(constants::DRAWER_2_ZONE_1_PERSISTENT_UUID);
    auto drawer_1_zone_2 = get_manager<Chassis>().get_entry(constants::DRAWER_1_ZONE_2_PERSISTENT_UUID);
    auto drawer_2_zone_2 = get_manager<Chassis>().get_entry(constants::DRAWER_2_ZONE_2_PERSISTENT_UUID);

    ASSERT_EQ(constants::ZONE_1_PERSISTENT_UUID, drawer_1_zone_1.get_parent_chassis());
    ASSERT_EQ(constants::ZONE_1_PERSISTENT_UUID, drawer_2_zone_1.get_parent_chassis());
    ASSERT_EQ(constants::ZONE_2_PERSISTENT_UUID, drawer_1_zone_2.get_parent_chassis());
    ASSERT_EQ(constants::ZONE_2_PERSISTENT_UUID, drawer_2_zone_2.get_parent_chassis());

    ASSERT_EQ(constants::POWER_ZONE_1_PERSISTENT_UUID, drawer_1_zone_1.get_power_zone());
    ASSERT_EQ(constants::POWER_ZONE_1_PERSISTENT_UUID, drawer_2_zone_1.get_power_zone());
    ASSERT_EQ(constants::POWER_ZONE_2_PERSISTENT_UUID, drawer_1_zone_2.get_power_zone());
    ASSERT_EQ(constants::POWER_ZONE_2_PERSISTENT_UUID, drawer_2_zone_2.get_power_zone());

    ASSERT_EQ(constants::THERMAL_ZONE_1_PERSISTENT_UUID, drawer_1_zone_1.get_thermal_zone());
    ASSERT_EQ(constants::THERMAL_ZONE_1_PERSISTENT_UUID, drawer_2_zone_1.get_thermal_zone());
    ASSERT_EQ(constants::THERMAL_ZONE_2_PERSISTENT_UUID, drawer_1_zone_2.get_thermal_zone());
    ASSERT_EQ(constants::THERMAL_ZONE_2_PERSISTENT_UUID, drawer_2_zone_2.get_thermal_zone());

    ASSERT_EQ(constants::RACK_PERSISTENT_UUID, zone_1.get_parent_chassis());
    ASSERT_EQ(constants::RACK_PERSISTENT_UUID, zone_2.get_parent_chassis());

    ASSERT_EQ(constants::RACK_PERSISTENT_UUID, rack_manager.get_location());
    ASSERT_EQ(constants::ZONE_1_PERSISTENT_UUID, zone_1_manager.get_location());
    ASSERT_EQ(constants::ZONE_2_PERSISTENT_UUID, zone_2_manager.get_location());
}
