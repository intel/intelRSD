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

#include "agent-framework/command/storage/get_logical_drive_info.hpp"
#include "agent-framework/command/storage/json/get_logical_drive_info.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::command;

static constexpr enums::LogicalDriveMode TEST_MODE
    {enums::LogicalDriveMode::JBOD};
static constexpr enums::LogicalDriveType TEST_TYPE
    {enums::LogicalDriveType::LVM};

class GetLogicalDriveInfo : public storage::GetLogicalDriveInfo {
private:
    std::string m_drive{};
public:
    GetLogicalDriveInfo(std::string drive) { m_drive = drive; }

    using storage::GetLogicalDriveInfo::execute;

    void execute(const Request& request, Response& response) {
        auto drive = request.get_drive();

        if (drive != m_drive) {
            throw exceptions::InvalidUuid("Wrong UUID!");
        }

        LogicalDrive logical_drive{};
        logical_drive.set_mode(enums::LogicalDriveMode::JBOD);
        logical_drive.set_type(enums::LogicalDriveType::LVM);
        logical_drive.set_capacity_gb(2048);
        logical_drive.set_image("Test Image");
        logical_drive.set_bootable(true);
        logical_drive.set_protected(true);
        logical_drive.set_master("Test Master");
        response.set_drive(logical_drive);
    }

    virtual ~GetLogicalDriveInfo();
};

GetLogicalDriveInfo::~GetLogicalDriveInfo() { }

TEST(GetLogicalDriveInfoTest, PositiveExecute) {
    storage::json::GetLogicalDriveInfo command_json;
    GetLogicalDriveInfo* command = new GetLogicalDriveInfo("TestDrive");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::DRIVE] = "TestDrive";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::LogicalDrive::IMAGE].isString());
    ASSERT_TRUE(result[literals::LogicalDrive::TYPE].isString());
    ASSERT_TRUE(result[literals::LogicalDrive::MODE].isString());
    ASSERT_TRUE(result[literals::LogicalDrive::MASTER].isString());
    ASSERT_TRUE(result[literals::LogicalDrive::PROTECTED].isBool());
    ASSERT_TRUE(result[literals::LogicalDrive::CAPACITY].isNumeric());
    ASSERT_TRUE(result[literals::LogicalDrive::BOOTABLE].isBool());
    ASSERT_EQ(result[literals::LogicalDrive::IMAGE].asString(), "Test Image");
    ASSERT_EQ(result[literals::LogicalDrive::MODE].asString(), TEST_MODE.to_string());
    ASSERT_EQ(result[literals::LogicalDrive::MASTER].asString(), "Test Master");
    ASSERT_EQ(result[literals::LogicalDrive::BOOTABLE].asBool(), true);
    ASSERT_EQ(result[literals::LogicalDrive::PROTECTED].asBool(), true);
    ASSERT_EQ(result[literals::LogicalDrive::TYPE].asString(), TEST_TYPE.to_string());
    ASSERT_EQ(result[literals::LogicalDrive::CAPACITY].asInt(), 2048);
}

TEST(GetLogicalDriveInfoTest, NegativeDriveNotFound) {
    storage::json::GetLogicalDriveInfo command_json;
    GetLogicalDriveInfo* command = new GetLogicalDriveInfo("TestDrive");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::LogicalDrive::DRIVE] = "OtherTestDrive";

    EXPECT_THROW(command_json.method(params, result), jsonrpc::JsonRpcException);
}
