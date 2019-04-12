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

#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/command-ref/compute_commands.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command_ref;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;

static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";
static constexpr enums::TransportProtocol TEST_INTERFACE{enums::TransportProtocol::SATA};
static constexpr enums::DriveType TEST_TYPE{enums::DriveType::HDD};
static constexpr int TEST_CAPACITY = 2048;

class MyGetDriveInfo {
private:
    std::string m_drive{};
public:
    MyGetDriveInfo(
        const std::string& drive) {
        m_drive = drive;
        }

    void execute(const GetDriveInfo::Request& request,
                 GetDriveInfo::Response& response) {
        const auto& drive_uuid = request.get_uuid();

        if (drive_uuid != m_drive) {
            throw std::runtime_error("Not found");
        }

        agent_framework::model::Drive drive{};

        drive.set_interface(TEST_INTERFACE);
        drive.set_type(TEST_TYPE);
        drive.set_capacity_gb(TEST_CAPACITY);
        drive.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));
        drive.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));
        drive.set_oem(agent_framework::model::attribute::Oem());

        response = drive;
    }

    virtual ~MyGetDriveInfo();
};

MyGetDriveInfo::~MyGetDriveInfo() { }

TEST(GetDriveInfoTest, PositiveExecute) {
    MyGetDriveInfo command{"TestDriveId"};
    GetDriveInfo::Request request{""};
    GetDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Drive::DRIVE] = "TestDriveId";

    EXPECT_NO_THROW(request = GetDriveInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

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

TEST(GetDriveInfoTest, NegativeModuleNotFound) {
    MyGetDriveInfo command{"TestDriveId"};
    GetDriveInfo::Request request{""};
    GetDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Drive::DRIVE] = "OtherTestDriveId";

    EXPECT_NO_THROW(request = GetDriveInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}
