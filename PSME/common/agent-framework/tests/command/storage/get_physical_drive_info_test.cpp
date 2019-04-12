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

static constexpr enums::PhysicalDriveInterface TEST_INTERFACE
    {enums::PhysicalDriveInterface::SATA};
static constexpr enums::PhysicalDriveType TEST_TYPE
    {enums::PhysicalDriveType::SSD};
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";
static constexpr char TEST_DRIVE[] = "TestDrive";
static constexpr int TEST_CAPACITY = 2048;
static constexpr int TEST_RPM = 7200;


class MyGetPhysicalDriveInfo {
private:
    std::string m_drive{};
public:
    MyGetPhysicalDriveInfo(std::string drive) { m_drive = drive; }

    void execute(const GetPhysicalDriveInfo::Request& request,
                 GetPhysicalDriveInfo::Response& response) {
        auto drive = request.get_uuid();

        if (drive != m_drive) {
            throw std::runtime_error("Not found");
        }

        PhysicalDrive physical_drive{};
        physical_drive.set_interface(enums::PhysicalDriveInterface::SATA);
        physical_drive.set_type(enums::PhysicalDriveType::SSD);
        physical_drive.set_capacity_gb(TEST_CAPACITY);
        physical_drive.set_rpm(TEST_RPM);
        physical_drive.set_fru_info(agent_framework::model::attribute::FruInfo(
                                  TEST_SERIAL,
                                  TEST_MANUFACTURER,
                                  TEST_MODEL,
                                  TEST_PART));
        response = physical_drive;
    }
};

TEST(GetPhysicalDriveInfoTest, PositiveExecute) {
    MyGetPhysicalDriveInfo command{TEST_DRIVE};
    GetPhysicalDriveInfo::Request request{""};
    GetPhysicalDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::PhysicalDrive::DRIVE] = TEST_DRIVE;

    EXPECT_NO_THROW(request = GetPhysicalDriveInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

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
    ASSERT_EQ(result[literals::PhysicalDrive::TYPE].asString(), TEST_TYPE.to_string());
    ASSERT_EQ(result[literals::PhysicalDrive::CAPACITY].asInt(), TEST_CAPACITY);
    ASSERT_EQ(result[literals::PhysicalDrive::RPM].asInt(), TEST_RPM);
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::SERIAL], TEST_SERIAL);
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::MANUFACTURER], TEST_MANUFACTURER);
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::MODEL], TEST_MODEL);
    ASSERT_EQ(result[literals::PhysicalDrive::FRU_INFO][literals::FruInfo::PART], TEST_PART);
}

TEST(GetPhysicalDriveInfoTest, NegativeDriveNotFound) {
    MyGetPhysicalDriveInfo command{TEST_DRIVE};
    GetPhysicalDriveInfo::Request request{""};
    GetPhysicalDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::PhysicalDrive::DRIVE] = "OtherTestDrive";

    EXPECT_NO_THROW(request = GetPhysicalDriveInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
