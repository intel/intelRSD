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

#include "agent-framework/command/storage/delete_logical_drive.hpp"
#include "agent-framework/command/storage/json/delete_logical_drive.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class DeleteLogicalDrive : public storage::DeleteLogicalDrive {
private:
    std::string m_drive{};
public:
    DeleteLogicalDrive(std::string drive) { m_drive = drive; }

    using storage::DeleteLogicalDrive::execute;

    void execute(const Request& request, Response& response) {
        auto drive = request.get_drive();

        if (drive != m_drive) {
            throw exception::NotFound();
        }

        response.set_oem(agent_framework::model::attribute::Oem());
    }

    virtual ~DeleteLogicalDrive();
};

DeleteLogicalDrive::~DeleteLogicalDrive() { }

class DeleteLogicalDriveTest : public ::testing::Test {
protected:
    static constexpr char DRIVE[] = "drive";
    static constexpr char OEM[] = "oem";

    virtual ~DeleteLogicalDriveTest();
};

constexpr char DeleteLogicalDriveTest::DRIVE[];
constexpr char DeleteLogicalDriveTest::OEM[];

DeleteLogicalDriveTest::~DeleteLogicalDriveTest() { }

TEST_F(DeleteLogicalDriveTest, PositiveExecute) {
    storage::json::DeleteLogicalDrive command_json;
    DeleteLogicalDrive* command = new DeleteLogicalDrive("TestDrive");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[DRIVE] = "TestDrive";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[OEM].isObject());
}

TEST_F(DeleteLogicalDriveTest, NegativeDriveNotFound) {
    storage::json::DeleteLogicalDrive command_json;
    DeleteLogicalDrive* command = new DeleteLogicalDrive("TestDrive");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[DRIVE] = "OtherTestDrive";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
