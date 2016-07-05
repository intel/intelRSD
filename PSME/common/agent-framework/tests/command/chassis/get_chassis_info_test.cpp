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

#include "agent-framework/command/chassis/get_chassis_info.hpp"
#include "agent-framework/command/chassis/json/get_chassis_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;
using namespace agent_framework::model;

static constexpr int TEST_SIZE = 2048;
static constexpr int TEST_OFFSET = 1;
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";

class GetChassisInfo : public chassis::GetChassisInfo {
private:
    std::string m_chassis{};
public:
    GetChassisInfo(const std::string& chassis) { m_chassis = chassis; }

    using chassis::GetChassisInfo::execute;

    void execute(const Request& request, Response& response) {
        const auto& chassis_uuid = request.get_chassis();

        if (chassis_uuid != m_chassis) {
            throw exception::NotFound();
        }

	Chassis chassis{};
        chassis.set_status(attribute::Status(enums::State::InTest, enums::Health::Warning));
        chassis.set_type(enums::ChassisType::Rack);
        chassis.set_size(TEST_SIZE);
        chassis.set_location_offset(TEST_OFFSET);
        chassis.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));
        chassis.set_oem(attribute::Oem());
        response.set_chassis(std::move(chassis));
    }

    virtual ~GetChassisInfo();
};

GetChassisInfo::~GetChassisInfo() { }

TEST(GetChassisInfoTest, PositiveExecute) {
    chassis::json::GetChassisInfo command_json;
    GetChassisInfo* command = new GetChassisInfo("TestChassis");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["chassis"] = "TestChassis";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result["type"].isString());
    ASSERT_TRUE(result["size"].isUInt());
    ASSERT_TRUE(result["locationOffset"].isUInt());
    ASSERT_TRUE(result["status"].isObject());
    ASSERT_TRUE(result["status"]["state"].isString());
    ASSERT_TRUE(result["status"]["health"].isString());
    ASSERT_TRUE(result["oem"].isObject());
    ASSERT_TRUE(result["fruInfo"].isObject());
    ASSERT_TRUE(result["fruInfo"]["serialNumber"].isString());
    ASSERT_TRUE(result["fruInfo"]["manufacturer"].isString());
    ASSERT_TRUE(result["fruInfo"]["modelNumber"].isString());
    ASSERT_TRUE(result["fruInfo"]["partNumber"].isString());
    ASSERT_EQ(result["type"].asString(), "Rack");
    ASSERT_EQ(result["size"].asUInt(), TEST_SIZE);
    ASSERT_EQ(result["locationOffset"].asUInt(), TEST_OFFSET);
    ASSERT_EQ(result["status"]["state"], "InTest");
    ASSERT_EQ(result["status"]["health"], "Warning");
    ASSERT_EQ(result["fruInfo"]["serialNumber"].asString(), TEST_SERIAL);
    ASSERT_EQ(result["fruInfo"]["manufacturer"].asString(), TEST_MANUFACTURER);
    ASSERT_EQ(result["fruInfo"]["modelNumber"].asString(), TEST_MODEL);
    ASSERT_EQ(result["fruInfo"]["partNumber"].asString(), TEST_PART);
}

TEST(GetChassisInfoTest, NegativeModuleNotFound) {
    chassis::json::GetChassisInfo command_json;
    GetChassisInfo* command = new GetChassisInfo("TestChassis");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["chassis"] = "OtherTestChassis";

    EXPECT_THROW(command_json.method(params, result), exception::NotFound);
}

