/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/command/storage/add_logical_drive.hpp"
#include "agent-framework/command/storage/json/add_logical_drive.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class AddLogicalDrive : public storage::AddLogicalDrive {
private:
    enums::LogicalDriveType m_type{ enums::LogicalDriveType::LVM };
    enums::LogicalDriveMode m_mode{ enums::LogicalDriveMode::JBOD };
    std::string m_master{};
    bool m_snapshot{};
    bool m_protected{};
    std::vector<std::string> m_drives{};
public:
    AddLogicalDrive(
        agent_framework::model::enums::LogicalDriveType type,
        agent_framework::model::enums::LogicalDriveMode mode,
        std::string master,
        bool snapshot,
        bool prot) {
        m_type = type;
        m_mode = mode;
        m_master = master;
        m_snapshot = snapshot;
        m_protected = prot;
        }

    using storage::AddLogicalDrive::execute;

    void execute(const Request& request, Response& response) {
        auto type = request.get_type();
        auto mode = request.get_mode();
        auto master = request.get_master();
        auto snapshot = request.is_snapshot();
        auto prot = request.is_protected();

        if (type != m_type
            || mode != m_mode
            || master != m_master
            || snapshot != m_snapshot
            || prot != m_protected) {
            throw exception::NotFound();
        }

        response.set_oem(agent_framework::model::attribute::Oem());
        response.set_drive_uuid("Test Drive UUID");
    }

    virtual ~AddLogicalDrive();
};

AddLogicalDrive::~AddLogicalDrive() { }

TEST(AddLogicalDriveTest, PositiveExecute) {
    storage::json::AddLogicalDrive command_json;
    AddLogicalDrive* command = new AddLogicalDrive(enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::LogicalDrive::OEM].isObject());
    ASSERT_TRUE(result[literals::LogicalDrive::DRIVE].isString());
    ASSERT_EQ(result[literals::LogicalDrive::DRIVE].asString(), "Test Drive UUID");
}

TEST(AddLogicalDriveTest, NegativeTypeNotFound) {
    storage::json::AddLogicalDrive command_json;
    AddLogicalDrive* command = new AddLogicalDrive(enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::TYPE] = "CEPH";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddLogicalDriveTest, NegativeModeNotFound) {
    storage::json::AddLogicalDrive command_json;
    AddLogicalDrive* command = new AddLogicalDrive(enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "LV";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddLogicalDriveTest, NegativeMasterNotFound) {
    storage::json::AddLogicalDrive command_json;
    AddLogicalDrive* command = new AddLogicalDrive(enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "OtherTestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::PROTECTED] = true;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddLogicalDriveTest, NegativeSnapshotNotFound) {
    storage::json::AddLogicalDrive command_json;
    AddLogicalDrive* command = new AddLogicalDrive(enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = false;
    params[literals::LogicalDrive::PROTECTED] = true;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddLogicalDriveTest, NegativeProtectedNotFound) {
    storage::json::AddLogicalDrive command_json;
    AddLogicalDrive* command = new AddLogicalDrive(enums::LogicalDriveType::LVM,
                    enums::LogicalDriveMode::JBOD, "TestMaster", true, true);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::TYPE] = "LVM";
    params[literals::LogicalDrive::MODE] = "JBOD";
    params[literals::LogicalDrive::MASTER] = "TestMaster";
    params[literals::LogicalDrive::SNAPSHOT] = true;
    params[literals::LogicalDrive::PROTECTED] = false;

    EXPECT_ANY_THROW(command_json.method(params, result));
}
