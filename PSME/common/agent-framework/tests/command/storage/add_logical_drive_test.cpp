/*!
 * @section LICENSE
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
 * @section DESCRIPTION
 * */

#include "agent-framework/command-ref/storage_commands.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command_ref;
using namespace agent_framework::model;

class MyAddLogicalDrive {
private:
    enums::LogicalDriveType m_type{ enums::LogicalDriveType::LVM };
    enums::LogicalDriveMode m_mode{ enums::LogicalDriveMode::JBOD };
    std::string m_master{};
    bool m_snapshot{};
    bool m_bootable{};
    bool m_protected{};
    std::vector<std::string> m_drives{};
public:
    MyAddLogicalDrive(
        agent_framework::model::enums::LogicalDriveType type,
        agent_framework::model::enums::LogicalDriveMode mode,
        std::string master,
        bool snapshot,
        bool bootable,
        bool prot) {
        m_type = type;
        m_mode = mode;
        m_master = master;
        m_snapshot = snapshot;
        m_bootable = bootable;
        m_protected = prot;
        }

    void execute(const AddLogicalDrive::Request& request,
                 AddLogicalDrive::Response& response) {
        auto type = request.get_type();
        auto mode = request.get_mode();
        auto master = request.get_master();
        auto snapshot = request.is_snapshot();
        auto bootable = request.is_bootable();
        auto prot = request.is_protected();

        if (type != m_type.to_string()
            || mode != m_mode.to_string()
            || master != m_master
            || snapshot != m_snapshot
            || bootable != m_bootable
            || prot != m_protected) {
            throw std::runtime_error("Not found");
        }
        response.set_drive("Test Drive UUID");
    }
};

TEST(AddLogicalDriveTest, PositiveExecute) {
    MyAddLogicalDrive command{enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true, true};
    AddLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::BOOTABLE] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    auto request = AddLogicalDrive::Request::from_json(params);
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::LogicalDrive::OEM].isObject());
    ASSERT_TRUE(result[literals::LogicalDrive::DRIVE].isString());
    ASSERT_EQ(result[literals::LogicalDrive::DRIVE].asString(), "Test Drive UUID");
}

TEST(AddLogicalDriveTest, NegativeTypeNotFound) {
    MyAddLogicalDrive command{enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true, true};
    AddLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::LogicalDrive::TYPE] = "CEPH";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::BOOTABLE] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    auto request = AddLogicalDrive::Request::from_json(params);
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST(AddLogicalDriveTest, NegativeModeNotFound) {
    MyAddLogicalDrive command{enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true, true};
    AddLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "LV";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::BOOTABLE] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    auto request = AddLogicalDrive::Request::from_json(params);
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST(AddLogicalDriveTest, NegativeMasterNotFound) {
    MyAddLogicalDrive command{enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true, true};
    AddLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "OtherTestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::BOOTABLE] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    auto request = AddLogicalDrive::Request::from_json(params);
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST(AddLogicalDriveTest, NegativeSnapshotNotFound) {
    MyAddLogicalDrive command{enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true, true};
    AddLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = false;
    params[literals::LogicalDrive::BOOTABLE] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    auto request = AddLogicalDrive::Request::from_json(params);
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST(AddLogicalDriveTest, NegativeProtectedNotFound) {
    MyAddLogicalDrive command{enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true, true};
    AddLogicalDrive::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::BOOTABLE] = true;
    params[literals::LogicalDrive::PROTECTED] = false;

    auto request = AddLogicalDrive::Request::from_json(params);
    EXPECT_ANY_THROW(command.execute(request, response));
}
