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

#include "agent-framework/command/compute/get_memory_info.hpp"
#include "agent-framework/command/compute/json/get_memory_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;

static constexpr char TEST_SOCKET[] = "Test Socket 1";
static constexpr char TEST_TYPE[] = "Test Type";
static constexpr enums::MemoryFormFactor TEST_FORM_FACTOR{
    enums::MemoryFormFactor::Unknown};
static constexpr int TEST_SIZE = 16;
static constexpr int TEST_SPEED = 1333;
static constexpr int TEST_DATA_WIDTH = 8;
static constexpr int TEST_TOTAL_WIDTH = 10;
static constexpr double TEST_VOLTAGE = 3.2;
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";

class GetMemoryInfo : public compute::GetMemoryInfo {
private:
    std::string m_memory{};
public:
    GetMemoryInfo(
        const std::string& memory) {
        m_memory = memory;
    }

    using compute::GetMemoryInfo::execute;
    using Memory =  agent_framework::model::Memory;

    void execute(const Request& request, Response& response) {
        const auto& memory_uuid = request.get_memory();

        if (memory_uuid != m_memory) {
            throw exception::NotFound();
        }

        Memory memory{};
        memory.set_socket(TEST_SOCKET);
        memory.set_type(TEST_TYPE);
        memory.set_size_gb(TEST_SIZE);
        memory.set_speed_mhz(TEST_SPEED);
        memory.set_voltage_volt(TEST_VOLTAGE);
        memory.set_data_width_bits(TEST_DATA_WIDTH);
        memory.set_total_width_bits(TEST_TOTAL_WIDTH);
        memory.set_form_factor(TEST_FORM_FACTOR);
        memory.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));
        memory.set_oem(agent_framework::model::attribute::OEMData());

        response.set_memory(std::move(memory));
    }

    virtual ~GetMemoryInfo();
};

GetMemoryInfo::~GetMemoryInfo() { }

class GetMemoryInfoTest : public ::testing::Test {
public:
    virtual ~GetMemoryInfoTest();
};

GetMemoryInfoTest::~GetMemoryInfoTest() { }

TEST_F(GetMemoryInfoTest, PositiveExecute) {
    compute::json::GetMemoryInfo command_json;
    GetMemoryInfo* command = new GetMemoryInfo("TestMemoryId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Memory::MEMORY] = "TestMemoryId";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::Memory::SOCKET].isString());
    ASSERT_TRUE(result[literals::Memory::TYPE].isString());
    ASSERT_TRUE(result[literals::Memory::SIZE].isUInt());
    ASSERT_TRUE(result[literals::Memory::SPEED].isUInt());
    ASSERT_TRUE(result[literals::Memory::VOLTAGE].isNumeric());
    ASSERT_TRUE(result[literals::Memory::DATA_WIDTH].isUInt());
    ASSERT_TRUE(result[literals::Memory::TOTAL_WIDTH].isUInt());
    ASSERT_TRUE(result[literals::Memory::FORM_FACTOR].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO].isObject());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_TRUE(result[OEMData::OEM].isObject());
    ASSERT_EQ(result[literals::Memory::SOCKET].asString(), TEST_SOCKET);
    ASSERT_EQ(result[literals::Memory::TYPE].asString(), TEST_TYPE);
    ASSERT_EQ(result[literals::Memory::SIZE].asUInt(), TEST_SIZE);
    ASSERT_EQ(result[literals::Memory::SPEED].asUInt(), TEST_SPEED);
    ASSERT_EQ(result[literals::Memory::VOLTAGE].asDouble(), TEST_VOLTAGE);
    ASSERT_EQ(result[literals::Memory::DATA_WIDTH].asUInt(), TEST_DATA_WIDTH);
    ASSERT_EQ(result[literals::Memory::TOTAL_WIDTH].asUInt(), TEST_TOTAL_WIDTH);
    ASSERT_EQ(result[literals::Memory::FORM_FACTOR].asString(), TEST_FORM_FACTOR.to_string());
}

TEST_F(GetMemoryInfoTest, NegativeModuleNotFound) {
    compute::json::GetMemoryInfo command_json;
    GetMemoryInfo* command = new GetMemoryInfo("TestMemoryId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Memory::MEMORY] = "OtherTestMemoryId";

    EXPECT_THROW(command_json.method(params, result), exception::NotFound);
}

