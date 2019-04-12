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
 * @file network_tree_stabilizer_test.cpp
 * */


#include "tree_stability/network_tree_stabilizer.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/service_uuid.hpp"

#include "gtest/gtest.h"

#include <string>
#include <iostream>



using namespace agent::network;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;

namespace constants {

// Service UUID
constexpr const char SERIVCE_UUID[] = "e784d192-379c-11e6-bc47-0242ac110002";

// Switch identifier
constexpr const char SWITCH_IDENTIFIER[] = "switch_identifier";

// Switch ports identifiers
constexpr const char PORT_1_IDENTIFIER[] = "port_1_identifier";
constexpr const char PORT_2_IDENTIFIER[] = "port_2_identifier";
constexpr const char PORT_3_IDENTIFIER[] = "port_3_identifier";

// Static MACs MAC addresses
constexpr const char STATIC_MAC_1_PORT_1_MAC[] = "ae:af:0d:3e:10:df";
constexpr const char STATIC_MAC_2_PORT_1_MAC[] = "52:67:0c:46:03:04";
constexpr const char STATIC_MAC_1_PORT_2_MAC[] = "96:f5:fd:bf:61:e6";
constexpr const char STATIC_MAC_2_PORT_2_MAC[] = "1e:ea:2a:4d:35:7d";
constexpr const char STATIC_MAC_1_PORT_3_MAC[] = "5e:66:a9:99:21:99";
constexpr const char STATIC_MAC_2_PORT_3_MAC[] = "2a:a3:3c:ea:7e:3b";

// Static MACs VLAN id
constexpr const unsigned STATIC_MAC_VLAN_ID = 1u;

// ACL names
constexpr const char ACL_1_NAME[] = "acl_1_name";
constexpr const char ACL_2_NAME[] = "acl_2_name";

// ACL rules ids
constexpr const unsigned RULE_1_ACL_1_ID = 1;
constexpr const unsigned RULE_2_ACL_1_ID = 2;
constexpr const unsigned RULE_1_ACL_2_ID = 1;
constexpr const unsigned RULE_2_ACL_2_ID = 2;

// Expected values of generated persistent uuids
constexpr const char MANAGER_PERSISTENT_UUID[] = "7e5d1afb-1a40-5d1a-b068-3065d9e2e834";

constexpr const char CHASSIS_PERSISTENT_UUID[] = "78c645b3-304b-5b78-8cab-adbed49316e6";

constexpr const char SWITCH_PERSISTENT_UUID[] = "bf5ba8f0-0c81-5158-965b-7b6a3ca737d0";

constexpr const char PORT_1_PERSISTENT_UUID[] = "1d505a10-1964-5c80-88fa-a85bc58aec8b";
constexpr const char PORT_2_PERSISTENT_UUID[] = "5bfe59af-c06b-5ebe-8d67-63c666d3e74b";
constexpr const char PORT_3_PERSISTENT_UUID[] = "2078e1dc-3aa8-57e1-865c-5c09edb51016";

constexpr const char STATIC_MAC_1_PORT_1_MAC_PERSISTENT_UUID[] = "df5b1dc6-78d2-5f8e-864b-d90cc410ab2d";
constexpr const char STATIC_MAC_2_PORT_1_MAC_PERSISTENT_UUID[] = "bb9aad87-5e9f-5719-8779-d9c0b10f360d";
constexpr const char STATIC_MAC_1_PORT_2_MAC_PERSISTENT_UUID[] = "ed40965b-a7ad-5f74-be8f-beb054409b5e";
constexpr const char STATIC_MAC_2_PORT_2_MAC_PERSISTENT_UUID[] = "86896c31-b76d-51e0-9066-978b8c8cfae2";
constexpr const char STATIC_MAC_1_PORT_3_MAC_PERSISTENT_UUID[] = "ec62bb40-7c28-59e3-9840-08fdd06ba995";
constexpr const char STATIC_MAC_2_PORT_3_MAC_PERSISTENT_UUID[] = "17487475-d5a0-5487-9556-74fee927cdb7";

constexpr const char VLAN_ON_PORT_1_PERSISTENT_UUID[] = "2056c268-1723-5087-af2e-91a51bbb29b4";
constexpr const char VLAN_ON_PORT_2_PERSISTENT_UUID[] = "a4b1cc10-7c90-50f3-80b8-a9754200f623";
constexpr const char VLAN_ON_PORT_3_PERSISTENT_UUID[] = "5016486e-1f10-5657-abcd-d74971a265e0";

constexpr const char ACL_1_PERSISTENT_UUID[] = "41eb68eb-8251-519b-a46a-2390f07cd564";
constexpr const char ACL_2_PERSISTENT_UUID[] = "e0c4f8ae-41a8-589e-b6a9-82d31e7fc1de";

constexpr const char RULE_1_ACL_1_PERSISTENT_UUID[] = "5c86b7eb-bce5-5707-923d-2cdd83777243";
constexpr const char RULE_2_ACL_1_PERSISTENT_UUID[] = "a9dae42f-6fde-59a7-a548-f103ccd7c379";
constexpr const char RULE_1_ACL_2_PERSISTENT_UUID[] = "d53ff764-8abc-5c43-9e27-7da9d643d7b4";
constexpr const char RULE_2_ACL_2_PERSISTENT_UUID[] = "b37390ea-acc7-5c77-b9bc-3a8d6c99f73e";

}

class NetworkTreeStabilizerTest : public ::testing::Test {
public:
    virtual ~NetworkTreeStabilizerTest();


    virtual void SetUp();


    virtual void TearDown();
};


NetworkTreeStabilizerTest::~NetworkTreeStabilizerTest() { }


void NetworkTreeStabilizerTest::SetUp() {
    // This is required to force the linker not to link with network agent hw api

    ServiceUuid::get_instance()->set_service_uuid(constants::SERIVCE_UUID);

    // Create parent manager for ethernet switch and chassis
    Manager manager{};

    // Create one chassis
    Chassis chassis{manager.get_uuid()};

    // Create one ethernet switch
    EthernetSwitch eth_switch{manager.get_uuid()};

    // Create 3 switch ports
    EthernetSwitchPort port_1{eth_switch.get_uuid()};
    EthernetSwitchPort port_2{eth_switch.get_uuid()};
    EthernetSwitchPort port_3{eth_switch.get_uuid()};

    // Create static macs (two per switch port)
    StaticMac static_mac_1_port_1{port_1.get_uuid()};
    StaticMac static_mac_2_port_1{port_1.get_uuid()};
    StaticMac static_mac_1_port_2{port_2.get_uuid()};
    StaticMac static_mac_2_port_2{port_2.get_uuid()};
    StaticMac static_mac_1_port_3{port_3.get_uuid()};
    StaticMac static_mac_2_port_3{port_3.get_uuid()};

    // Create port vlans (one per switch port)
    EthernetSwitchPortVlan vlan_on_port_1{port_1.get_uuid()};
    EthernetSwitchPortVlan vlan_on_port_2{port_2.get_uuid()};
    EthernetSwitchPortVlan vlan_on_port_3{port_3.get_uuid()};

    // Create tow ACLs
    Acl acl_1{eth_switch.get_uuid()};
    Acl acl_2{eth_switch.get_uuid()};

    // Create ACL rules (two per ACL)
    AclRule rule_1_acl_1{acl_1.get_uuid()};
    AclRule rule_2_acl_1{acl_1.get_uuid()};
    AclRule rule_1_acl_2{acl_2.get_uuid()};
    AclRule rule_2_acl_2{acl_2.get_uuid()};

    // Set relations between componenents
    eth_switch.set_chassis(chassis.get_uuid());

    // Set unique properties
    eth_switch.set_switch_identifier(constants::SWITCH_IDENTIFIER);

    port_1.set_port_identifier(constants::PORT_1_IDENTIFIER);
    port_2.set_port_identifier(constants::PORT_2_IDENTIFIER);
    port_3.set_port_identifier(constants::PORT_3_IDENTIFIER);

    static_mac_1_port_1.set_address(constants::STATIC_MAC_1_PORT_1_MAC);
    static_mac_2_port_1.set_address(constants::STATIC_MAC_2_PORT_1_MAC);
    static_mac_1_port_2.set_address(constants::STATIC_MAC_1_PORT_2_MAC);
    static_mac_2_port_2.set_address(constants::STATIC_MAC_2_PORT_2_MAC);
    static_mac_1_port_3.set_address(constants::STATIC_MAC_1_PORT_3_MAC);
    static_mac_2_port_3.set_address(constants::STATIC_MAC_2_PORT_3_MAC);

    static_mac_1_port_1.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    static_mac_2_port_1.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    static_mac_1_port_2.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    static_mac_2_port_2.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    static_mac_1_port_3.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    static_mac_2_port_3.set_vlan_id(constants::STATIC_MAC_VLAN_ID);

    vlan_on_port_1.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    vlan_on_port_2.set_vlan_id(constants::STATIC_MAC_VLAN_ID);
    vlan_on_port_3.set_vlan_id(constants::STATIC_MAC_VLAN_ID);

    acl_1.set_name(constants::ACL_1_NAME);
    acl_2.set_name(constants::ACL_2_NAME);

    rule_1_acl_1.set_rule_id(constants::RULE_1_ACL_1_ID);
    rule_2_acl_1.set_rule_id(constants::RULE_2_ACL_1_ID);
    rule_1_acl_2.set_rule_id(constants::RULE_1_ACL_2_ID);
    rule_2_acl_2.set_rule_id(constants::RULE_2_ACL_2_ID);

    // Add resources to the model
    get_manager<Manager>().add_entry(manager);

    get_manager<Chassis>().add_entry(chassis);

    get_manager<EthernetSwitch>().add_entry(eth_switch);

    get_manager<EthernetSwitchPort>().add_entry(port_1);
    get_manager<EthernetSwitchPort>().add_entry(port_2);
    get_manager<EthernetSwitchPort>().add_entry(port_3);

    get_manager<StaticMac>().add_entry(static_mac_1_port_1);
    get_manager<StaticMac>().add_entry(static_mac_2_port_1);
    get_manager<StaticMac>().add_entry(static_mac_1_port_2);
    get_manager<StaticMac>().add_entry(static_mac_2_port_2);
    get_manager<StaticMac>().add_entry(static_mac_1_port_3);
    get_manager<StaticMac>().add_entry(static_mac_2_port_3);

    get_manager<EthernetSwitchPortVlan>().add_entry(vlan_on_port_1);
    get_manager<EthernetSwitchPortVlan>().add_entry(vlan_on_port_2);
    get_manager<EthernetSwitchPortVlan>().add_entry(vlan_on_port_3);

    get_manager<Acl>().add_entry(acl_1);
    get_manager<Acl>().add_entry(acl_2);

    get_manager<AclRule>().add_entry(rule_1_acl_1);
    get_manager<AclRule>().add_entry(rule_2_acl_1);
    get_manager<AclRule>().add_entry(rule_1_acl_2);
    get_manager<AclRule>().add_entry(rule_2_acl_2);

    // Stabilize mock resource tree
    NetworkTreeStabilizer().stabilize(manager.get_uuid());
}


void NetworkTreeStabilizerTest::TearDown() {
    get_manager<Manager>().clear_entries();
    get_manager<EthernetSwitchPortVlan>().clear_entries();
    NetworkComponents::get_instance()->get_vlan_manager().clear_entries();
    get_manager<EthernetSwitch>().clear_entries();
    get_manager<EthernetSwitchPort>().clear_entries();
    NetworkComponents::get_instance()->get_remote_switch_manager().clear_entries();
    get_manager<Chassis>().clear_entries();
    NetworkComponents::get_instance()->get_port_members_manager().clear_entries();
    get_manager<Acl>().clear_entries();
    get_manager<AclRule>().clear_entries();
    NetworkComponents::get_instance()->get_acl_port_manager().clear_entries();
    get_manager<StaticMac>().clear_entries();
}


TEST_F(NetworkTreeStabilizerTest, NoNewOrDeletedResourcesTest) {
    // Assert that all resources are still present in the resource tree
    ASSERT_EQ(1, get_manager<Manager>().get_keys().size());
    ASSERT_EQ(1, get_manager<Chassis>().get_keys().size());
    ASSERT_EQ(1, get_manager<EthernetSwitch>().get_keys().size());
    ASSERT_EQ(3, get_manager<EthernetSwitchPort>().get_keys().size());
    ASSERT_EQ(3, get_manager<EthernetSwitchPortVlan>().get_keys().size());
    ASSERT_EQ(6, get_manager<StaticMac>().get_keys().size());
    ASSERT_EQ(2, get_manager<Acl>().get_keys().size());
    ASSERT_EQ(4, get_manager<AclRule>().get_keys().size());
}


TEST_F(NetworkTreeStabilizerTest, PersistentUUIDsGeneratedTest) {
    // Assert that all resources now have persistent UUIDs
    ASSERT_TRUE(get_manager<Manager>().entry_exists(constants::MANAGER_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<Chassis>().entry_exists(constants::CHASSIS_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<EthernetSwitch>().entry_exists(constants::SWITCH_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<EthernetSwitchPort>().entry_exists(constants::PORT_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<EthernetSwitchPort>().entry_exists(constants::PORT_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<EthernetSwitchPort>().entry_exists(constants::PORT_3_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<StaticMac>().entry_exists(constants::STATIC_MAC_1_PORT_1_MAC_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<StaticMac>().entry_exists(constants::STATIC_MAC_2_PORT_1_MAC_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<StaticMac>().entry_exists(constants::STATIC_MAC_1_PORT_2_MAC_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<StaticMac>().entry_exists(constants::STATIC_MAC_2_PORT_2_MAC_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<StaticMac>().entry_exists(constants::STATIC_MAC_1_PORT_3_MAC_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<StaticMac>().entry_exists(constants::STATIC_MAC_2_PORT_3_MAC_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<EthernetSwitchPortVlan>().entry_exists(constants::VLAN_ON_PORT_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<EthernetSwitchPortVlan>().entry_exists(constants::VLAN_ON_PORT_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<EthernetSwitchPortVlan>().entry_exists(constants::VLAN_ON_PORT_3_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<Acl>().entry_exists(constants::ACL_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<Acl>().entry_exists(constants::ACL_2_PERSISTENT_UUID));

    ASSERT_TRUE(get_manager<AclRule>().entry_exists(constants::RULE_1_ACL_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<AclRule>().entry_exists(constants::RULE_2_ACL_1_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<AclRule>().entry_exists(constants::RULE_1_ACL_2_PERSISTENT_UUID));
    ASSERT_TRUE(get_manager<AclRule>().entry_exists(constants::RULE_2_ACL_2_PERSISTENT_UUID));
}


TEST_F(NetworkTreeStabilizerTest, ParentUUIDChangedTest) {
    // Assert that the switch parent is correct
    ASSERT_EQ(1, get_manager<EthernetSwitch>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());

    // Assert that the chassis parent is correct
    ASSERT_EQ(1, get_manager<Chassis>().get_keys(constants::MANAGER_PERSISTENT_UUID).size());

    // Assert that ports parent is correct
    ASSERT_EQ(3, get_manager<EthernetSwitchPort>().get_keys(constants::SWITCH_PERSISTENT_UUID).size());

    // Assert that static macs and VLANs parent is correct
    auto ports_uuids = get_manager<EthernetSwitchPort>().get_keys(constants::SWITCH_PERSISTENT_UUID);

    for (const auto& port_uuid : ports_uuids) {
        ASSERT_EQ(2, get_manager<StaticMac>().get_keys(port_uuid).size());
        ASSERT_EQ(1, get_manager<EthernetSwitchPortVlan>().get_keys(port_uuid).size());
    }

    // Assert that ACLs parent is correct
    ASSERT_EQ(2, get_manager<Acl>().get_keys(constants::SWITCH_PERSISTENT_UUID).size());

    // Assert that ACL rules parent is correct
    auto acls_uuids = get_manager<Acl>().get_keys();
    for (const auto& acl_uuid : acls_uuids) {
        ASSERT_EQ(2, get_manager<AclRule>().get_keys(acl_uuid).size());
    }
}

TEST_F(NetworkTreeStabilizerTest, RelationsUpdatedTest) {
    // Assert that manager location points to chassis
    ASSERT_EQ(get_manager<Manager>().get_entry(constants::MANAGER_PERSISTENT_UUID).get_location(),
              constants::CHASSIS_PERSISTENT_UUID);

    // Assert that ethernet switch points to chassis
    ASSERT_EQ(get_manager<EthernetSwitch>().get_entry(constants::SWITCH_PERSISTENT_UUID).get_chassis(),
              constants::CHASSIS_PERSISTENT_UUID);
}
