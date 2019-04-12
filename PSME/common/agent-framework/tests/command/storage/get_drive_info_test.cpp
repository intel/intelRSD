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

class MyGetDriveInfo {
private:
    std::string m_component{};
    std::uint32_t m_drive{};
    std::string m_interface{};
    std::string m_type{};
    std::uint32_t m_capacity_gb{};
    std::uint32_t m_rpm{};

    class TestOEMData : public OEMData {
    public:
        json::Json to_json() const {
            json::Json json_oem = json::Json();
            json_oem["testOEM"] = "TestOEMSpecificData";
            return json_oem;
        }
    };
public:
    MyGetDriveInfo(
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

    void execute(const GetDriveInfo::Request& request,
                 GetDriveInfo::Response& response) {
        auto component = request.get_component();
        auto drive = request.get_drive();

        if (component != m_component || drive != m_drive) {
            throw std::runtime_error("Not found");
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
};

TEST(GetDriveInfoTest, PositiveExecute) {
    MyGetDriveInfo command{
        "TestComponentId",
        1,
        "Test Interface",
        "Test Type",
        2048,
        7200};
    GetDriveInfo::Request request{""};
    GetDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params["component"] = "TestComponentId";
    params["drive"] = 1;

    EXPECT_NO_THROW(request = GetDriveInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

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

TEST(GetDriveInfoTest, NegativeComponentNotFound) {
    MyGetDriveInfo command{
        "TestComponentId",
        1,
        "Test Interface",
        "Test Type",
        2048,
        7200};
    GetDriveInfo::Request request{""};
    GetDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params["component"] = "OtherTestComponentId";
    params["drive"] = 1;

    EXPECT_NO_THROW(request = GetDriveInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}

TEST_F(GetDriveInfoTest, NegativeSlotNotFound) {
    MyGetDriveInfo command{
        "TestComponentId",
        1,
        "Test Interface",
        "Test Type",
        2048,
        7200};
    GetDriveInfo::Request request{""};
    GetDriveInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params["component"] = "TestComponentId";
    params["drive"] = 2;

    EXPECT_NO_THROW(request = GetDriveInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
