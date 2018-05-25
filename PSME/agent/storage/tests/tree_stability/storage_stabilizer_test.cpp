/*!
 * @brief Implementation of Storage tree stability tests.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file storage_stabilizer_test.cpp
 */

#include "gtest/gtest.h"

#include "tree_stability/storage_stabilizer.hpp"
#include "agent-framework/module/storage_components.hpp"

using namespace agent_framework::model;
using namespace agent::storage;

TEST(StorageStabilizer, ManagerTest) {
    Manager manager{}, same_manager{};

    Uuid uuid1 = StorageStabilizer().stabilize(manager);
    Uuid uuid2 = StorageStabilizer().stabilize(same_manager);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, ChassisTestPositive) {
    Manager manager{};
    Chassis chassis{manager.get_uuid()}, same_chassis{manager.get_uuid()};

    Uuid uuid1 = StorageStabilizer().stabilize(chassis, manager);
    Uuid uuid2 = StorageStabilizer().stabilize(same_chassis, manager);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, ChassisTestNegative) {
    Manager manager{}, another_manager{};
    Chassis chassis{manager.get_uuid()}, another_chassis{another_manager.get_uuid()};

    Uuid uuid1 = StorageStabilizer().stabilize(chassis, manager);
    Uuid uuid2 = StorageStabilizer().stabilize(another_chassis, another_manager);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, StorageServiceTestPositive) {
    Manager manager{};
    StorageService service{manager.get_uuid()}, same_service{manager.get_uuid()};

    Uuid uuid1 = StorageStabilizer().stabilize(service, manager);
    Uuid uuid2 = StorageStabilizer().stabilize(same_service, manager);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, FabricTestPositive) {
    Manager manager{};
    Fabric fabric{manager.get_uuid()}, same_fabric{manager.get_uuid()};

    Uuid uuid1 = StorageStabilizer().stabilize(fabric, manager);
    Uuid uuid2 = StorageStabilizer().stabilize(same_fabric, manager);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, FabricTestNegative) {
    Manager manager{}, another_manager{};
    Fabric fabric{manager.get_uuid()}, another_fabric{another_manager.get_uuid()};

    Uuid uuid1 = StorageStabilizer().stabilize(fabric, manager);
    Uuid uuid2 = StorageStabilizer().stabilize(another_fabric, another_manager);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, StorageServiceTestNegative) {
    Manager manager{}, another_manager{};
    StorageService service{manager.get_uuid()}, another_service{another_manager.get_uuid()};

    Uuid uuid1 = StorageStabilizer().stabilize(service, manager);
    Uuid uuid2 = StorageStabilizer().stabilize(another_service, another_manager);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, DriveTestPositive) {
    Drive drive{}, same_drive{};

    drive.set_fru_info(attribute::FruInfo{"Serial1"});
    same_drive.set_fru_info(attribute::FruInfo{"Serial1"});

    Uuid uuid1 = StorageStabilizer().stabilize(drive);
    Uuid uuid2 = StorageStabilizer().stabilize(same_drive);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, DriveTestNegative) {
    Drive drive{}, another_drive{};

    drive.set_fru_info(attribute::FruInfo{"Serial1"});
    another_drive.set_fru_info(attribute::FruInfo{"Serial2"});

    Uuid uuid1 = StorageStabilizer().stabilize(drive);
    Uuid uuid2 = StorageStabilizer().stabilize(another_drive);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, DrivesOnlySerialNumberShouldBeUsed) {
    Drive drive{}, same_drive{};

    drive.set_fru_info(attribute::FruInfo{"serial", "abc", "def", "ghi"});
    same_drive.set_fru_info(attribute::FruInfo{"serial", "jkl", "mno", "prs"});

    Uuid uuid1 = StorageStabilizer().stabilize(drive);
    Uuid uuid2 = StorageStabilizer().stabilize(same_drive);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, StoragePoolTestPositive) {
    StoragePool pool{}, same_pool{};

    pool.add_identifier({"C:/", enums::IdentifierType::SystemPath});
    same_pool.add_identifier({"C:/", enums::IdentifierType::SystemPath});

    Uuid uuid1 = StorageStabilizer().stabilize(pool);
    Uuid uuid2 = StorageStabilizer().stabilize(same_pool);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, StoragePoolTestNegative) {
    StoragePool pool{}, another_pool{};

    pool.add_identifier({"C:/", enums::IdentifierType::SystemPath});
    another_pool.add_identifier({"D:/", enums::IdentifierType::SystemPath});

    Uuid uuid1 = StorageStabilizer().stabilize(pool);
    Uuid uuid2 = StorageStabilizer().stabilize(another_pool);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, VolumeTestPositive) {
    Volume volume{}, same_volume{};

    volume.add_identifier({"C:/", enums::IdentifierType::SystemPath});
    same_volume.add_identifier({"C:/", enums::IdentifierType::SystemPath});

    Uuid uuid1 = StorageStabilizer().stabilize(volume);
    Uuid uuid2 = StorageStabilizer().stabilize(same_volume);
    ASSERT_EQ(uuid1, uuid2);
}

TEST(StorageStabilizer, VolumeTestNegative) {
    Volume volume{}, another_volume{};

    volume.add_identifier({"C:/", enums::IdentifierType::SystemPath});
    another_volume.add_identifier({"D:/", enums::IdentifierType::SystemPath});

    Uuid uuid1 = StorageStabilizer().stabilize(volume);
    Uuid uuid2 = StorageStabilizer().stabilize(another_volume);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, VolumeWithoutIdentifier) {
    Volume volume{}, another_volume{};

    // Stabilizer should use temporary UUIDs
    Uuid uuid1 = StorageStabilizer().stabilize(volume);
    Uuid uuid2 = StorageStabilizer().stabilize(another_volume);
    ASSERT_NE(uuid1, uuid2);
}

TEST(StorageStabilizer, VolumeWithAnotherIdentifierType) {
    Volume volume{}, same_volume{};

    volume.add_identifier({"ngn:123:test", enums::IdentifierType::NQN});
    same_volume.add_identifier({"ngn:123:test", enums::IdentifierType::NQN});

    // Stabilizer should use temporary UUIDs
    Uuid uuid1 = StorageStabilizer().stabilize(volume);
    Uuid uuid2 = StorageStabilizer().stabilize(same_volume);
    ASSERT_NE(uuid1, uuid2);
}
