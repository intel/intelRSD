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
using namespace agent_framework::model::literals;
using namespace agent_framework::model;

static constexpr int TEST_OFFSET = 1;
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";

class MyGetChassisInfo {
private:
    std::string m_chassis{};
public:
    MyGetChassisInfo(const std::string& chassis) { m_chassis = chassis; }

    void execute(const GetChassisInfo::Request& request,
                 GetChassisInfo::Response& response) {
        const auto& chassis_uuid = request.get_uuid();

        if (chassis_uuid != m_chassis) {
            throw std::runtime_error("Not found");
        }

        agent_framework::model::Chassis chassis{};
        chassis.set_status(attribute::Status(enums::State::InTest, enums::Health::Warning));
        chassis.set_type(enums::ChassisType::Rack);
        chassis.set_location_offset(TEST_OFFSET);
        chassis.set_fru_info(
            agent_framework::model::attribute::FruInfo(
                TEST_SERIAL,
                TEST_MANUFACTURER,
                TEST_MODEL,
                TEST_PART)
        );
        chassis.set_oem(attribute::Oem());
        response = chassis;
    }

    virtual ~MyGetChassisInfo();
};

MyGetChassisInfo::~MyGetChassisInfo() { }

TEST(GetChassisInfoTest, PositiveExecute) {
    MyGetChassisInfo command{"TestChassis"};
    GetChassisInfo::Request request{""};
    GetChassisInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params["chassis"] = "TestChassis";

    EXPECT_NO_THROW(request = GetChassisInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result["type"].isString());
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
    ASSERT_EQ(result["locationOffset"].asUInt(), TEST_OFFSET);
    ASSERT_EQ(result["status"]["state"], "InTest");
    ASSERT_EQ(result["status"]["health"], "Warning");
    ASSERT_EQ(result["fruInfo"]["serialNumber"].asString(), TEST_SERIAL);
    ASSERT_EQ(result["fruInfo"]["manufacturer"].asString(), TEST_MANUFACTURER);
    ASSERT_EQ(result["fruInfo"]["modelNumber"].asString(), TEST_MODEL);
    ASSERT_EQ(result["fruInfo"]["partNumber"].asString(), TEST_PART);
}

TEST(GetChassisInfoTest, NegativeModuleNotFound) {
    MyGetChassisInfo command{"TestChassis"};
    GetChassisInfo::Request request{""};
    GetChassisInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params["chassis"] = "OtherTestChassis";

    EXPECT_NO_THROW(request = GetChassisInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}

