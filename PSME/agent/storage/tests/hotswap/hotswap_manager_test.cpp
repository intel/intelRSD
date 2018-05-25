/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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

/* Tests includes */
#include "mock_hotswap_discoverer.hpp"

/* Agent */
#include "watcher/hotswap_manager.hpp"
#include "discovery/builders/drive_builder.hpp"

/* Agent framework */
#include "agent-framework/service_uuid.hpp"
#include "agent-framework/module/storage_components.hpp"

/* 3rd party */
#include "gtest/gtest.h"

using namespace agent::storage::watcher;
using namespace agent::storage::discovery;
using namespace agent_framework::generic;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

static const constexpr char CHASSIS_UUID[] = "2e0e5ac4-be39-11e7-abc4-cec278b6b50a";
static const constexpr char SERVICE_UUID[] = "3e0e5ac4-be39-11e7-abc4-cec278b6b50b";

static const constexpr char DRIVE_PATH_1[] = "C:/";
static const constexpr char DRIVE_PATH_2[] = "D:/";
static const constexpr char DRIVE_PATH_3[] = "F:/";

static const constexpr char SERIAL_1[] = "1234-4321";
static const constexpr char SERIAL_2[] = "4321-1234";
static const constexpr char SERIAL_3[] = "1243-3421";

static const constexpr char MODEL_1[] = "M1234";
static const constexpr char MODEL_2[] = "XPro800";
static const constexpr char MODEL_3[] = "HDDv12";

}

class StorageHotswapTest : public ::testing::Test {
public:
    virtual ~StorageHotswapTest();


    virtual void SetUp();


    virtual void TearDown();

    Drive make_drive(const Uuid& parent, const std::string& system_path,
                     const std::string& serial_number, const std::string& model) {
        Drive drive = DriveBuilder::build_default(parent);
        drive.add_identifier({system_path, enums::IdentifierType::SystemPath});
        drive.set_fru_info(attribute::FruInfo{
            serial_number, {}, model, {}
        });
        return drive;
    }

    Drive m_drive_1{};
    Drive m_drive_2{};
    Drive m_drive_3{};
};


StorageHotswapTest::~StorageHotswapTest() { }


void StorageHotswapTest::SetUp() {
    ServiceUuid::get_instance()->set_service_uuid(::SERVICE_UUID);

    // Create parent manager for storage services
    Manager manager{};

    // Create storage service
    StorageService service{manager.get_uuid()};

    // Create chassis
    Chassis chassis{manager.get_uuid()};
    chassis.set_uuid(::CHASSIS_UUID);

    m_drive_1 = make_drive(::CHASSIS_UUID, ::DRIVE_PATH_1, ::SERIAL_1, ::MODEL_1);
    m_drive_2 = make_drive(::CHASSIS_UUID, ::DRIVE_PATH_2, ::SERIAL_2, ::MODEL_2);
    m_drive_3 = make_drive(::CHASSIS_UUID, ::DRIVE_PATH_3, ::SERIAL_3, ::MODEL_3);

    // Add all created resources to the model
    get_manager<Manager>().add_entry(manager);
    get_manager<StorageService>().add_entry(service);
    get_manager<Chassis>().add_entry(chassis);
}


void StorageHotswapTest::TearDown() {
    get_manager<Manager>().clear_entries();
    get_manager<StorageService>().clear_entries();
    get_manager<Chassis>().clear_entries();
    get_manager<Drive>().clear_entries();
}


TEST_F(StorageHotswapTest, EraseOneDrive) {
    // Prepare drives to test
    get_manager<Drive>().add_entry(m_drive_1);
    get_manager<Drive>().add_entry(m_drive_2);
    get_manager<Drive>().add_entry(m_drive_3);

    // Drive #2 was erased
    auto discoverer = tests::MockHotswapDiscoverer{
        m_drive_1,
        m_drive_3
    };

    // --------------------------------------------
    HotswapManager manager{std::make_shared<tests::MockHotswapDiscoverer>(discoverer)};
    manager.hotswap_discover_hard_drives();

    auto size = get_manager<Drive>().get_entries().size();
    ASSERT_EQ(size, 2); // After hotswap only 2 drives were in container
}

TEST_F(StorageHotswapTest, InsertOneDrive) {
    // Prepare drives to test
    get_manager<Drive>().add_entry(m_drive_1);
    get_manager<Drive>().add_entry(m_drive_3);

    // Drive #2 was inserted
    auto discoverer = tests::MockHotswapDiscoverer{
        m_drive_1,
        m_drive_2,
        m_drive_3
    };

    // --------------------------------------------
    HotswapManager manager{std::make_shared<tests::MockHotswapDiscoverer>(discoverer)};
    manager.hotswap_discover_hard_drives();

    auto size = get_manager<Drive>().get_entries().size();
    ASSERT_EQ(size, 3);
}

TEST_F(StorageHotswapTest, EraseAllDrives) {
    // Prepare drives to test
    get_manager<Drive>().add_entry(m_drive_1);
    get_manager<Drive>().add_entry(m_drive_2);
    get_manager<Drive>().add_entry(m_drive_3);

    // All 3 drives were erased (empty discoverer)
    auto discoverer = tests::MockHotswapDiscoverer{};

    // --------------------------------------------
    HotswapManager manager{std::make_shared<tests::MockHotswapDiscoverer>(discoverer)};
    manager.hotswap_discover_hard_drives();

    auto size = get_manager<Drive>().get_entries().size();
    ASSERT_EQ(size, 0);
}

TEST_F(StorageHotswapTest, InsertAllDrives) {
    // Make sure that there is no drives
    get_manager<Drive>().clear_entries();

    // All, 3 drives were inserted
    auto discoverer = tests::MockHotswapDiscoverer{
        m_drive_1,
        m_drive_2,
        m_drive_3
    };

    // --------------------------------------------
    HotswapManager manager{std::make_shared<tests::MockHotswapDiscoverer>(discoverer)};
    manager.hotswap_discover_hard_drives();

    auto size = get_manager<Drive>().get_entries().size();
    ASSERT_EQ(size, 3);
}
