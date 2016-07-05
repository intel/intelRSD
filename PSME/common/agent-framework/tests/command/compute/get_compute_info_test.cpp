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

#include "agent-framework/command/compute/get_compute_info.hpp"
#include "agent-framework/command/compute/json/get_compute_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;

class GetComputeInfo : public compute::GetComputeInfo {
private:
    std::string m_compute{};
public:
    GetComputeInfo(const std::string& compute) { m_compute = compute; }

    using compute::GetComputeInfo::execute;

    void execute(const Request& request, Response& response) {
        const auto& compute = request.get_compute();

        if (compute != m_compute) {
            throw exception::NotFound();
        }

        response.set_status(agent_framework::generic::Status("TestState", "TestHealth"));
        response.set_power_zone("Test Power Zone");
        response.set_thermal_zone("Test Thermal Zone");
        response.set_slot(1);
        response.set_oem(agent_framework::generic::OEMData());
        response.set_fru_info(agent_framework::generic::FruInfo(
          "TestSerialNumber", "TestManufacturer", "TestModelNumber", "TestPartNumber"));
    }

    virtual ~GetComputeInfo();
};

GetComputeInfo::~GetComputeInfo() { }

class GetComputeInfoTest : public ::testing::Test {
public:
    virtual ~GetComputeInfoTest();
};

GetComputeInfoTest::~GetComputeInfoTest() { }

TEST_F(GetComputeInfoTest, PositiveExecute) {
    compute::json::GetComputeInfo command_json;
    GetComputeInfo* command = new GetComputeInfo("TestCompute");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["compute"] = "TestCompute";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result["powerZone"].isString());
    ASSERT_TRUE(result["slot"].isUInt());
    ASSERT_TRUE(result["thermalZone"].isString());
    ASSERT_TRUE(result["status"].isObject());
    ASSERT_TRUE(result["status"]["state"].isString());
    ASSERT_TRUE(result["status"]["health"].isString());
    ASSERT_TRUE(result["oem"].isObject());
    ASSERT_TRUE(result["fruInfo"].isObject());
    ASSERT_TRUE(result["fruInfo"]["serialNumber"].isString());
    ASSERT_TRUE(result["fruInfo"]["manufacturer"].isString());
    ASSERT_TRUE(result["fruInfo"]["modelNumber"].isString());
    ASSERT_TRUE(result["fruInfo"]["partNumber"].isString());
    ASSERT_EQ(result["powerZone"].asString(), "Test Power Zone");
    ASSERT_EQ(result["slot"].asUInt(), 1);
    ASSERT_EQ(result["thermalZone"].asString(), "Test Thermal Zone");
    ASSERT_EQ(result["status"]["state"], "TestState");
    ASSERT_EQ(result["status"]["health"], "TestHealth");
    ASSERT_EQ(result["fruInfo"]["serialNumber"], "TestSerialNumber");
    ASSERT_EQ(result["fruInfo"]["manufacturer"], "TestManufacturer");
    ASSERT_EQ(result["fruInfo"]["modelNumber"], "TestModelNumber");
    ASSERT_EQ(result["fruInfo"]["partNumber"], "TestPartNumber");
}

TEST_F(GetComputeInfoTest, NegativeModuleNotFound) {
    compute::json::GetComputeInfo command_json;
    GetComputeInfo* command = new GetComputeInfo("TestCompute");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["compute"] = "OtherTestCompute";

    EXPECT_THROW(command_json.method(params, result), jsonrpc::JsonRpcException);
}

