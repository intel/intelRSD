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

#include "agent-framework/command/storage/get_drive_info.hpp"
#include "agent-framework/command/storage/json/get_drive_info.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework;
using namespace agent_framework::command;

class GetDriveInfo : public storage::GetDriveInfo {
private:
    std::string m_component{};
    std::uint32_t m_drive{};
    std::string m_interface{};
    std::string m_type{};
    std::uint32_t m_capacity_gb{};
    std::uint32_t m_rpm{};

    class TestOEMData : public OEMData {
    public:
        Json::Value to_json() const {
            Json::Value json_oem;
            json_oem["testOEM"] = "TestOEMSpecificData";
            return json_oem;
        }

        ~TestOEMData();
    };
public:
    GetDriveInfo(
        const std::string& component,
        std::uint32_t drive,
        const std::string& interface,
        const std::string& type,
        std::uint32_t capacity_gb,
        std::uint32_t rpm) {
        m_component = component;
        m_drive = drive;
        m_interface = interface;
        m_type = type;
        m_capacity_gb = capacity_gb;
        m_rpm = rpm;
    }

    using storage::GetDriveInfo::execute;

    void execute(const Request& request, Response& response) {
        auto component = request.get_component();
        auto drive = request.get_drive();

        if (component != m_component || drive != m_drive) {
            throw exceptions::InvalidUuid("Wrong UUID!");
        }

        response.set_interface(m_interface);
        response.set_type(m_type);
        response.set_capacity_gb(m_capacity_gb);
        response.set_rpm(m_rpm);
        response.set_status(Status("TestState", "TestHealth"));
        response.set_fru_info(FRUInfo(
                                  "TestSerialNumber",
                                  "TestManufacturer",
                                  "TestModelNumber",
                                  "TestPartNumber"));
        response.set_oem(new TestOEMData);
    }

    virtual ~GetDriveInfo();
};

GetDriveInfo::~GetDriveInfo() { }

GetDriveInfo::TestOEMData::~TestOEMData() { }

class GetDriveInfoTest : public ::testing::Test {
public:
    virtual ~GetDriveInfoTest();
};

GetDriveInfoTest::~GetDriveInfoTest() { }

TEST_F(GetDriveInfoTest, PositiveExecute) {
    storage::json::GetDriveInfo command_json;
    GetDriveInfo* command = new GetDriveInfo(
        "TestComponentId",
        1,
        "Test Interface",
        "Test Type",
        2048,
        7200);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["component"] = "TestComponentId";
    params["drive"] = 1;

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result["interface"].isString());
    ASSERT_TRUE(result["type"].isString());
    ASSERT_TRUE(result["capacityGB"].isNumeric());
    ASSERT_TRUE(result["rpm"].isNumeric());
    ASSERT_TRUE(result["status"].isObject());
    ASSERT_TRUE(result["status"]["state"].isString());
    ASSERT_TRUE(result["status"]["health"].isString());
    ASSERT_TRUE(result["fruInfo"].isObject());
    ASSERT_TRUE(result["fruInfo"]["serialNumber"].isString());
    ASSERT_TRUE(result["fruInfo"]["manufacturer"].isString());
    ASSERT_TRUE(result["fruInfo"]["modelNumber"].isString());
    ASSERT_TRUE(result["fruInfo"]["partNumber"].isString());
    ASSERT_TRUE(result["oem"].isObject());
    ASSERT_EQ(result["interface"].asString(), "Test Interface");
    ASSERT_EQ(result["type"].asString(), "Test Type");
    ASSERT_EQ(result["capacityGB"].asInt(), 2048);
    ASSERT_EQ(result["rpm"].asInt(), 7200);
    ASSERT_EQ(result["status"]["state"], "TestState");
    ASSERT_EQ(result["status"]["health"], "TestHealth");
    ASSERT_EQ(result["fruInfo"]["serialNumber"], "TestSerialNumber");
    ASSERT_EQ(result["fruInfo"]["manufacturer"], "TestManufacturer");
    ASSERT_EQ(result["fruInfo"]["modelNumber"], "TestModelNumber");
    ASSERT_EQ(result["fruInfo"]["partNumber"], "TestPartNumber");
    ASSERT_EQ(result["oem"]["testOEM"], "TestOEMSpecificData");
}

TEST_F(GetDriveInfoTest, NegativeComponentNotFound) {
    storage::json::GetDriveInfo command_json;
    GetDriveInfo* command = new GetDriveInfo(
        "TestComponentId",
        1,
        "Test Interface",
        "Test Type",
        2048,
        7200);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["component"] = "OtherTestComponentId";
    params["drive"] = 1;

    EXPECT_THROW(command_json.method(params, result), jsonrpc::JsonRpcException);
}

TEST_F(GetDriveInfoTest, NegativeSlotNotFound) {
    storage::json::GetDriveInfo command_json;
    GetDriveInfo* command = new GetDriveInfo(
        "TestComponentId",
        1,
        "Test Interface",
        "Test Type",
        2048,
        7200);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params["component"] = "TestComponentId";
    params["drive"] = 2;

    EXPECT_THROW(command_json.method(params, result), jsonrpc::JsonRpcException);
}

