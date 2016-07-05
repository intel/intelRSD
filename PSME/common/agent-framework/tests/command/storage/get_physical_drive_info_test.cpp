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

#include "agent-framework/command/storage/get_physical_drive_info.hpp"
#include "agent-framework/command/storage/json/get_physical_drive_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr enums::PhysicalDriveInterface TEST_INTERFACE
    {enums::PhysicalDriveInterface::SATA};
static constexpr enums::PhysicalDriveType TEST_DRIVE
    {enums::PhysicalDriveType::SSD};

class GetPhysicalDriveInfo : public storage::GetPhysicalDriveInfo {
private:
    std::string m_drive{};
public:
    GetPhysicalDriveInfo(std::string drive) { m_drive = drive; }

    using storage::GetPhysicalDriveInfo::execute;

    void execute(const Request& request, Response& response) {
        auto drive = request.get_drive();

        if (drive != m_drive) {
            throw exception::NotFound();
        }

        PhysicalDrive physical_drive{};
        physical_drive.set_interface(enums::PhysicalDriveInterface::SATA);
        physical_drive.set_type(enums::PhysicalDriveType::SSD);
        physical_drive.set_capacity_gb(2048);
        physical_drive.set_rpm(7200);
        physical_drive.set_fru_info(agent_framework::model::attribute::FruInfo(
                                  "TestSerialNumber",
                                  "TestManufacturer",
                                  "TestModelNumber",
                                  "TestPartNumber"));
        response.set_drive(physical_drive);
    }

    virtual ~GetPhysicalDriveInfo();
};

GetPhysicalDriveInfo::~GetPhysicalDriveInfo() { }

TEST(GetPhysicalDriveInfoTest, PositiveExecute) {
    storage::json::GetPhysicalDriveInfo command_json;
    GetPhysicalDriveInfo* command = new GetPhysicalDriveInfo("TestDrive");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PhysicalDrive::DRIVE] = "TestDrive";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::PhysicalDrive::INTERFACE].isString());
    ASSERT_TRUE(result[literals::PhysicalDrive::TYPE].isString());
    ASSERT_TRUE(result[literals::PhysicalDrive::CAPACITY].isNumeric());
    ASSERT_TRUE(result[literals::PhysicalDrive::RPM].isNumeric());
    ASSERT_TRUE(result[literals::PhysicalDrive::FRU_INFO].isObject());
    ASSERT_TRUE(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::SERIAL].isString());
    ASSERT_TRUE(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::MANUFACTURER].isString());
    ASSERT_TRUE(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::MODEL].isString());
    ASSERT_TRUE(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::PART].isString());
    ASSERT_EQ(result[literals::PhysicalDrive::INTERFACE].asString(), TEST_INTERFACE.to_string());
    ASSERT_EQ(result[literals::PhysicalDrive::TYPE].asString(), TEST_DRIVE.to_string());
    ASSERT_EQ(result[literals::PhysicalDrive::CAPACITY].asInt(), 2048);
    ASSERT_EQ(result[literals::PhysicalDrive::RPM].asInt(), 7200);
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::SERIAL], "TestSerialNumber");
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::MANUFACTURER], "TestManufacturer");
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::MODEL], "TestModelNumber");
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::PART], "TestPartNumber");
}

TEST(GetPhysicalDriveInfoTest, NegativeDriveNotFound) {
    storage::json::GetPhysicalDriveInfo command_json;
    GetPhysicalDriveInfo* command = new GetPhysicalDriveInfo("TestDrive");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PhysicalDrive::DRIVE] = "OtherTestDrive";

    EXPECT_THROW(command_json.method(params, result), NotFound);
}
