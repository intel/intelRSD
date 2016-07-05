/*!
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
 *
 * @file logical_drive_test.cpp
 *
 * @brief Unit test for Logical Drive class
 * */

#include <uuid++.hh>

#include "agent-framework/module/logical_drive.hpp"
#include "gtest/gtest.h"

using namespace agent_framework::generic;
static constexpr const unsigned int COUNT = 10;


/* Positive tests */

/* get_logical_drives tests */

TEST(LogicalDriveTest, PositiveGetEmptyLogicalDrives) {
    const auto& logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);
    const auto& drives = logical_drive->get_logical_drives();
    ASSERT_EQ(drives.size(), 0);
}

TEST(LogicalDriveTest, PositiveGetFilledLogicalDrives) {
    auto logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        const auto& new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        ASSERT_NO_THROW(logical_drive->add_logical_drive(new_drive));

        const auto& drives = logical_drive->get_logical_drives();
        ASSERT_EQ(drives.size(), i + 1);
    }
}

TEST(LogicalDriveTest, PositiveGetFilledLogicalDrivesTree) {
    auto root_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(root_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        const auto& new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        ASSERT_NO_THROW(root_drive->add_logical_drive(new_drive));
    }

    const auto& children_drives = root_drive->get_logical_drives();
    ASSERT_EQ(children_drives.size(), COUNT);
    for(unsigned int i = 0; i < COUNT; ++i) {
        auto child_drive = children_drives.at(i);

        for(unsigned int j = 0; j < COUNT; ++j) {
            auto new_child = LogicalDrive::make_logical_drive();
            ASSERT_NE(new_child, nullptr);
            ASSERT_NO_THROW(child_drive->add_logical_drive(new_child));
        }
    }

    const auto& drives = root_drive->get_logical_drives();
    ASSERT_EQ(drives.size(), COUNT);

    for(auto child_drive : drives) {
        ASSERT_EQ(child_drive->get_logical_drives().size(), COUNT);
    }
}

/* find_logical_drives tests */

TEST(LogicalDriveTest, PositiveFindEmptyLogicalDrive) {
    const auto& logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);
    std::string drive_id = logical_drive->get_uuid();

    auto empty_drive = LogicalDrive::LogicalDriveWeakPtr{};
    ASSERT_EQ(logical_drive->find_logical_drive(drive_id).lock(),
              empty_drive.lock());
}

TEST(LogicalDriveTest, PositiveFindFilledLogicalDrive) {
    auto logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        const auto& new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        ASSERT_NO_THROW(logical_drive->add_logical_drive(new_drive));
    }

    const auto& drives = logical_drive->get_logical_drives();
    ASSERT_EQ(drives.size(), COUNT);
    unsigned int index = static_cast<unsigned int>(rand()) % COUNT;
    const auto& random_drive = drives.at(index);
    std::string sub_drive_id = random_drive->get_uuid();

    /* Test */
    auto search_drive = logical_drive->find_logical_drive(sub_drive_id);
    ASSERT_EQ(search_drive.lock(), random_drive);
}

TEST(LogicalDriveTest, PositiveFindSubLogicalDrive) {
    auto root_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(root_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        auto new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        for(unsigned int j = 0; j < COUNT; ++j) {
            const auto& new_sub_drive = LogicalDrive::make_logical_drive();
            ASSERT_NE(new_sub_drive, nullptr);
            ASSERT_NO_THROW(new_drive->add_logical_drive(new_sub_drive));
        }
        ASSERT_NO_THROW(root_drive->add_logical_drive(new_drive));
    }

    /* Top level of drives */
    const auto& drives = root_drive->get_logical_drives();
    ASSERT_EQ(drives.size(), COUNT);

    unsigned int index = static_cast<unsigned int>(rand()) % COUNT;
    const auto& random_drive = drives.at(index);

    /* Second level of drives */
    const auto& sub_drives = random_drive->get_logical_drives();
    ASSERT_EQ(sub_drives.size(), COUNT);

    index = static_cast<unsigned int>(rand()) % COUNT;
    const auto& random_sub_drive = sub_drives.at(index);

    std::string sub_drive_id = random_sub_drive->get_uuid();

    /* Test */
    auto search_drive = root_drive->find_logical_drive(sub_drive_id);
    ASSERT_EQ(search_drive.lock(), random_sub_drive);
}

/* delete_logical_drive tests */

TEST(LogicalDriveTest, PositiveDeleteEmptyLogicalDrive) {
    auto logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);
    std::string drive_id = logical_drive->get_uuid();

    ASSERT_FALSE(logical_drive->delete_logical_drive(drive_id));
}

TEST(LogicalDriveTest, PositiveDeleteFilledLogicalDrive) {
    auto logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        const auto& new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        ASSERT_NO_THROW(logical_drive->add_logical_drive(new_drive));
    }

    const auto& drives = logical_drive->get_logical_drives();
    ASSERT_EQ(drives.size(), COUNT);

    unsigned int index = static_cast<unsigned int>(rand()) % COUNT;
    const auto& random_drive = drives.at(index);
    std::string sub_drive_id = random_drive->get_uuid();

    /* Test */
    ASSERT_TRUE(logical_drive->delete_logical_drive(sub_drive_id));

    /* Check */
    auto empty_drive = LogicalDrive::LogicalDriveWeakPtr{};
    auto search_drive = logical_drive->find_logical_drive(sub_drive_id);
    ASSERT_EQ(search_drive.lock(), empty_drive.lock());
}

/* Negative tests */

/* find_logical_drives tests */

TEST(LogicalDriveTest, NegativeFindEmptyLogicalDrive) {
    const auto& logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);

    uuid test_id{};
    test_id.make(UUID_MAKE_V1);

    auto empty_drive = LogicalDrive::LogicalDriveWeakPtr{};
    ASSERT_EQ(logical_drive->find_logical_drive(test_id.string()).lock(),
              empty_drive.lock());
}

TEST(LogicalDriveTest, NegativeFindSubLogicalDrive) {
    auto root_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(root_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        auto new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        for(unsigned int j = 0; j < COUNT; ++j) {
            const auto& new_sub_drive = LogicalDrive::make_logical_drive();
            ASSERT_NE(new_sub_drive, nullptr);
            ASSERT_NO_THROW(new_drive->add_logical_drive(new_sub_drive));
        }
        ASSERT_NO_THROW(root_drive->add_logical_drive(new_drive));
    }

    uuid unknown_uuid{};
    unknown_uuid.make(UUID_MAKE_V1);
    auto empty_drive = LogicalDrive::LogicalDriveWeakPtr{};

    /* Test */
    auto search_drive = root_drive->find_logical_drive(unknown_uuid.string());
    ASSERT_EQ(search_drive.lock(), empty_drive.lock());
}

/* delete_logical_drive tests */

TEST(LogicalDriveTest, NegativeDeleteEmptyLogicalDrive) {
    auto logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);

    uuid unknown_id{};
    unknown_id.make(UUID_MAKE_V1);

    ASSERT_FALSE(logical_drive->delete_logical_drive(unknown_id.string()));
}

TEST(LogicalDriveTest, NegativeDeleteFilledLogicalDrive) {
    auto logical_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(logical_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        const auto& new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        ASSERT_NO_THROW(logical_drive->add_logical_drive(new_drive));
    }

    uuid unknown_id{};
    unknown_id.make(UUID_MAKE_V1);

    /* Test */
    ASSERT_FALSE(logical_drive->delete_logical_drive(unknown_id.string()));
}

TEST(LogicalDriveTest, NegativeDeleteSubLogicalDrive) {
    auto root_drive = LogicalDrive::make_logical_drive();
    ASSERT_NE(root_drive, nullptr);

    for(unsigned int i = 0; i < COUNT; ++i) {
        auto new_drive = LogicalDrive::make_logical_drive();
        ASSERT_NE(new_drive, nullptr);
        for(unsigned int j = 0; j < COUNT; ++j) {
            const auto& new_sub_drive = LogicalDrive::make_logical_drive();
            ASSERT_NE(new_sub_drive, nullptr);
            ASSERT_NO_THROW(new_drive->add_logical_drive(new_sub_drive));
        }
        ASSERT_NO_THROW(root_drive->add_logical_drive(new_drive));
    }

    /* Top level of drives */
    const auto& drives = root_drive->get_logical_drives();
    ASSERT_EQ(drives.size(), COUNT);

    unsigned int index = static_cast<unsigned int>(rand()) % COUNT;
    const auto& random_drive = drives.at(index);

    /* Second level of drives */
    const auto& sub_drives = random_drive->get_logical_drives();
    ASSERT_EQ(sub_drives.size(), COUNT);

    index = static_cast<unsigned int>(rand()) % COUNT;
    const auto& random_sub_drive = sub_drives.at(index);

    std::string sub_drive_id = random_sub_drive->get_uuid();

    /* Test */
    /* Note: No recursion */
    ASSERT_FALSE(root_drive->delete_logical_drive(sub_drive_id));

    /* Check */
    auto empty_drive = LogicalDrive::LogicalDriveWeakPtr{};
    auto search_drive = root_drive->find_logical_drive(sub_drive_id);
    ASSERT_NE(search_drive.lock(), empty_drive.lock());
}
