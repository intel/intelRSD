/*!
 * @brief Unit tests for generation of UUIDv5
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "tree_stability/storage_tree_stabilizer.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/service_uuid.hpp"
#include "agent-framework/module/enum/storage.hpp"

#include "gtest/gtest.h"

#include <string>



using namespace agent::storage;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;

namespace constants {

// Service UUID
constexpr const char SERIVCE_UUID[] = "e784d192-379c-11e6-bc47-0242ac110002";

// Logical drives device paths
constexpr const char LVG_DEVICE_PATH[] = "lvg_device_path";
constexpr const char PV_1_DEVICE_PATH[] = "pv_1_device_path";
constexpr const char PV_2_DEVICE_PATH[] = "pv_2_device_path";
constexpr const char LV_DEVICE_PATH[] = "lv_device_path";

// Physical drives serial numbers
constexpr const char PHYSICAL_DRIVE_1_SERIAL_NUMBER[] = "serial1";
constexpr const char PHYSICAL_DRIVE_2_SERIAL_NUMBER[] = "serial2";

// iSCSI target IQN
constexpr const char ISCSI_TARGET_1_IQN[] = "target_1_IQN";
constexpr const char ISCSI_TARGET_2_IQN[] = "target_2_IQN";

// iSCSI target LUN
const unsigned ISCSI_TARGET_1_LUN = 1;
const unsigned ISCSI_TARGET_2_LUN = 2;

// iSCSI target ID
const unsigned ISCSI_TARGET_1_ID = 1;
const unsigned ISCSI_TARGET_2_ID = 2;

// Expected values of generated persistnet UUIDs
constexpr const char MANAGER_PERSISTENT_UUID[] = "bd124990-083f-5dc5-ab57-d75870917b5f";
constexpr const char STORAGE_SERVICES_PERSISTENT_UUID[] = "49291515-1049-5734-8c7a-7537898ece1b";
const std::vector <std::string> PHYSICAL_DRIVES_UUIDS{"a878d5b2-e6f1-5cda-b764-79e27fa4ce70",
                                                      "6e49d771-9988-5fa9-9b07-c0d0145c34cc"};
const std::vector <std::string> LOGICAL_DRIVES_UUIDS{"88586a93-e31c-58ea-a4c6-c50147968d5c",
                                                     "ba0af620-b70a-547c-890d-67445930caa5",
                                                     "79ba3d28-527b-5d98-b7a7-cd0284202782"};
const std::vector <std::string> ISCSI_TARGETS_UUIDS{"fe3b77b5-0ebd-59cc-a59f-95910a190d9c",
                                                               "4499385f-cc3a-5803-b32c-89bfdb596275"};

}

class StorageTreeStabilizerTest : public ::testing::Test {
public:
    virtual ~StorageTreeStabilizerTest();


    virtual void SetUp();


    virtual void TearDown();
};


StorageTreeStabilizerTest::~StorageTreeStabilizerTest() { }


void StorageTreeStabilizerTest::SetUp() {
    ServiceUuid::get_instance()->set_service_uuid(constants::SERIVCE_UUID);

    // Create parent manager for storage services
    Manager manager{};

    // Create storage service
    StorageServices services{manager.get_uuid()};

    // Create a LVG logical volume
    LogicalDrive volume_group{services.get_uuid()};
    volume_group.set_mode(enums::LogicalDriveMode::LVG);
    volume_group.set_device_path(constants::LVG_DEVICE_PATH);

    // Create 2 logical drives of type "PV"
    LogicalDrive pv_logical_drive_1{volume_group.get_uuid()};
    pv_logical_drive_1.set_device_path(constants::PV_1_DEVICE_PATH);
    pv_logical_drive_1.set_mode(enums::LogicalDriveMode::PV);

    LogicalDrive pv_logical_drive_2{volume_group.get_uuid()};
    pv_logical_drive_2.set_device_path(constants::PV_2_DEVICE_PATH);
    pv_logical_drive_2.set_mode(enums::LogicalDriveMode::PV);

    // Create 2 physical drives corresponding to PV logical drives
    PhysicalDrive physical_drive_1{pv_logical_drive_1.get_uuid()};
    physical_drive_1.set_fru_info({constants::PHYSICAL_DRIVE_1_SERIAL_NUMBER, "", "", ""});

    PhysicalDrive physical_drive_2{pv_logical_drive_2.get_uuid()};
    physical_drive_2.set_fru_info({constants::PHYSICAL_DRIVE_2_SERIAL_NUMBER, "", "", ""});

    // Create 1 logical drive of type "LV"
    LogicalDrive lv_logical_drive{volume_group.get_uuid()};
    lv_logical_drive.set_device_path(constants::LV_DEVICE_PATH);
    lv_logical_drive.set_mode(enums::LogicalDriveMode::LV);

    // Create 2 iSCSI targets
    IscsiTarget target_1{services.get_uuid()};
    target_1.set_target_iqn(constants::ISCSI_TARGET_1_IQN);
    target_1.add_target_lun({constants::ISCSI_TARGET_1_LUN, pv_logical_drive_1.get_uuid()});
    target_1.set_target_id(constants::ISCSI_TARGET_1_ID);

    IscsiTarget target_2{services.get_uuid()};
    target_2.set_target_iqn(constants::ISCSI_TARGET_2_IQN);
    target_2.add_target_lun({constants::ISCSI_TARGET_2_LUN, lv_logical_drive.get_uuid()});
    target_2.set_target_id(constants::ISCSI_TARGET_2_ID);

    // Add all created resources to the model
    CommonComponents::get_instance()->get_module_manager().add_entry(manager);

    StorageComponents::get_instance()->get_storage_services_manager().add_entry(services);

    StorageComponents::get_instance()->get_physical_drive_manager().add_entry(physical_drive_1);
    StorageComponents::get_instance()->get_physical_drive_manager().add_entry(physical_drive_2);

    StorageComponents::get_instance()->get_logical_drive_manager().add_entry(pv_logical_drive_1);
    StorageComponents::get_instance()->get_logical_drive_manager().add_entry(pv_logical_drive_2);
    StorageComponents::get_instance()->get_logical_drive_manager().add_entry(lv_logical_drive);

    StorageComponents::get_instance()->get_iscsi_target_manager().add_entry(target_1);
    StorageComponents::get_instance()->get_iscsi_target_manager().add_entry(target_2);
    StorageComponents::get_instance()->get_iscsi_target_manager().add_target_id(constants::ISCSI_TARGET_1_ID,
                                                                                target_1.get_uuid());
    StorageComponents::get_instance()->get_iscsi_target_manager().add_target_id(constants::ISCSI_TARGET_2_ID,
                                                                                target_2.get_uuid());

    StorageTreeStabilizer().stabilize(manager.get_uuid());
}


void StorageTreeStabilizerTest::TearDown() {
    CommonComponents::get_instance()->get_module_manager().clear_entries();
    StorageComponents::get_instance()->get_storage_services_manager().clear_entries();
    StorageComponents::get_instance()->get_logical_drive_manager().clear_entries();
    StorageComponents::get_instance()->get_physical_drive_manager().clear_entries();
    StorageComponents::get_instance()->get_iscsi_target_manager().clear_entries();
}


TEST_F(StorageTreeStabilizerTest, NoNewOrDeletedResourcesTest) {
    // Assert that all resources are still present in the resource tree
    ASSERT_EQ(1, CommonComponents::get_instance()->get_module_manager().get_keys().size());
    ASSERT_EQ(1, StorageComponents::get_instance()->get_storage_services_manager().get_keys().size());
    ASSERT_EQ(2, StorageComponents::get_instance()->get_physical_drive_manager().get_keys().size());
    ASSERT_EQ(3, StorageComponents::get_instance()->get_logical_drive_manager().get_keys().size());
    ASSERT_EQ(2, StorageComponents::get_instance()->get_iscsi_target_manager().get_keys().size());
}


TEST_F(StorageTreeStabilizerTest, PersistentUUIDsGeneratedTest) {
    // Assert that all resources now have persistent UUIDs
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_module_manager().get_entry(constants::MANAGER_PERSISTENT_UUID));
    ASSERT_NO_THROW(StorageComponents::get_instance()->
                    get_storage_services_manager().get_entry(constants::STORAGE_SERVICES_PERSISTENT_UUID));

    for (const auto& PHYSICAL_DRIVE_UUID : constants::PHYSICAL_DRIVES_UUIDS) {
        ASSERT_NO_THROW(StorageComponents::get_instance()->
                        get_physical_drive_manager().get_entry(PHYSICAL_DRIVE_UUID));
    }

    for (const auto& LOGICAL_DRIVE_UUID : constants::LOGICAL_DRIVES_UUIDS) {
        ASSERT_NO_THROW(StorageComponents::get_instance()->
                        get_logical_drive_manager().get_entry(LOGICAL_DRIVE_UUID));
    }

    for (const auto& ISCSI_TARGET_UUID : constants::ISCSI_TARGETS_UUIDS) {
        ASSERT_NO_THROW(StorageComponents::get_instance()->
            get_iscsi_target_manager().get_entry(ISCSI_TARGET_UUID));
    }


}
