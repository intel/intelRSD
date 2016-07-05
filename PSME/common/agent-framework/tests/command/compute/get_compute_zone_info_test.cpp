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

#include "agent-framework/command/compute/get_compute_zone_info.hpp"
#include "agent-framework/command/compute/json/get_compute_zone_info.hpp"
#include "agent-framework/module/collection.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class GetComputeZoneInfo : public compute::GetComputeZoneInfo {
private:
    std::string m_zone{};
public:
    GetComputeZoneInfo(const std::string& zone) { m_zone = zone; }

    using compute::GetComputeZoneInfo::execute;

    void execute(const Request& request, Response& response) {
        const auto& zone = request.get_zone();

        if (zone != m_zone) {
            throw exception::NotFound();
        }

        response.set_status(agent_framework::generic::Status("TestState", "TestHealth"));
        response.set_collection(agent_framework::generic::Collection());
        response.set_oem(agent_framework::generic::OEMData());
    }

    virtual ~GetComputeZoneInfo();
};

GetComputeZoneInfo::~GetComputeZoneInfo() { }

class GetComputeZoneInfoTest : public ::testing::Test {
public:
    virtual ~GetComputeZoneInfoTest();
};

GetComputeZoneInfoTest::~GetComputeZoneInfoTest() { }

TEST_F(GetComputeZoneInfoTest, PositiveExecute) {
    compute::json::GetComputeZoneInfo command_json;
    GetComputeZoneInfo* command = new GetComputeZoneInfo("TestZone");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["zone"] = "TestZone";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result["status"].isObject());
    ASSERT_TRUE(result["status"]["state"].isString());
    ASSERT_TRUE(result["status"]["health"].isString());
    ASSERT_TRUE(result["oem"].isObject());
    ASSERT_TRUE(result["collections"].isArray());
    ASSERT_EQ(result["status"]["state"], "TestState");
    ASSERT_EQ(result["status"]["health"], "TestHealth");
}

TEST_F(GetComputeZoneInfoTest, NegativeModuleNotFound) {
    compute::json::GetComputeZoneInfo command_json;
    GetComputeZoneInfo* command = new GetComputeZoneInfo("TestZone");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["zone"] = "OtherTestZone";

    EXPECT_THROW(command_json.method(params, result), jsonrpc::JsonRpcException);
}

