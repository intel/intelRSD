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

#include "agent-framework/command/compute/get_drive_info.hpp"
#include "agent-framework/command/compute/json/get_drive_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;

static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";
static constexpr enums::DriveInterface TEST_INTERFACE{enums::DriveInterface::PCIe};
static constexpr enums::DriveType TEST_TYPE{enums::DriveType::NVMe};
static constexpr int TEST_CAPACITY = 2048;

class GetDriveInfo : public compute::GetDriveInfo {
private:
    std::string m_drive{};
public:
    GetDriveInfo(
        const std::string& drive) {
        m_drive = drive;
        }

    using compute::GetDriveInfo::execute;

    void execute(const Request& request, Response& response) {
        const auto& drive_uuid = request.get_drive();

        if (drive_uuid != m_drive) {
            throw exception::NotFound();
        }

        agent_framework::model::Drive drive{};

        drive.set_interface(TEST_INTERFACE);
        drive.set_type(TEST_TYPE);
        drive.set_capacity_gb(TEST_CAPACITY);
        drive.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));
        drive.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));
        drive.set_oem(agent_framework::model::attribute::Oem());

	response.set_drive(drive);
    }

    virtual ~GetDriveInfo();
};

GetDriveInfo::~GetDriveInfo() { }

class GetDriveInfoTest : public ::testing::Test {
public:
    virtual ~GetDriveInfoTest();
};

GetDriveInfoTest::~GetDriveInfoTest() { }

TEST_F(GetDriveInfoTest, PositiveExecute) {
    compute::json::GetDriveInfo command_json;
    GetDriveInfo* command = new GetDriveInfo("TestDriveId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Drive::DRIVE] = "TestDriveId";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::Drive::INTERFACE].isString());
    ASSERT_TRUE(result[literals::Drive::TYPE].isString());
    ASSERT_TRUE(result[literals::Drive::CAPACITY].isDouble());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_TRUE(result[Status::STATUS][Status::STATE].isString());
    ASSERT_TRUE(result[Status::STATUS][Status::HEALTH].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO].isObject());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::SERIAL].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::MANUFACTURER].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::MODEL].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::PART].isString());
    ASSERT_TRUE(result[Oem::OEM].isObject());
    ASSERT_EQ(result[literals::Drive::INTERFACE].asString(), TEST_INTERFACE.to_string());
    ASSERT_EQ(result[literals::Drive::TYPE].asString(), TEST_TYPE.to_string());
    ASSERT_EQ(result[literals::Drive::CAPACITY].asDouble(), TEST_CAPACITY);
    ASSERT_EQ(result[Status::STATUS][Status::STATE], "InTest");
    ASSERT_EQ(result[Status::STATUS][Status::HEALTH], "Warning");
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::SERIAL], TEST_SERIAL);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::MANUFACTURER], TEST_MANUFACTURER);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::MODEL], TEST_MODEL);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::PART], TEST_PART);
}

TEST_F(GetDriveInfoTest, NegativeModuleNotFound) {
    compute::json::GetDriveInfo command_json;
    GetDriveInfo* command = new GetDriveInfo("TestDriveId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Drive::DRIVE] = "OtherTestDriveId";

    EXPECT_THROW(command_json.method(params, result), exception::NotFound);
}
