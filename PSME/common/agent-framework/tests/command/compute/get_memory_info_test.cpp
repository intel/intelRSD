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

static constexpr enums::MemoryType TEST_MEMORY_TYPE = enums::MemoryType::DRAM;
static constexpr enums::DeviceType TEST_DEVICE_TYPE = enums::DeviceType::DDR;
static constexpr char TEST_DEVICE_LOCATOR[] = "DIMM 1";
static constexpr int TEST_SIZE = 16;
static constexpr int TEST_SPEED = 1333;
static constexpr int TEST_DATA_WIDTH = 8;
static constexpr int TEST_TOTAL_WIDTH = 10;
static constexpr double TEST_VOLTAGE = 3.2;
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";

class MyGetMemoryInfo {
private:
    std::string m_memory{};
public:
    MyGetMemoryInfo(
        const std::string& memory) {
        m_memory = memory;
    }

    using Memory =  agent_framework::model::Memory;

    void execute(const GetMemoryInfo::Request& request,
                 GetMemoryInfo::Response& response) {
        const auto& memory_uuid = request.get_uuid();

        if (memory_uuid != m_memory) {
            throw std::runtime_error("Not found");
        }

        Memory memory{};
        memory.set_memory_type(TEST_MEMORY_TYPE);
        memory.set_device_type(TEST_DEVICE_TYPE);
        memory.set_capacity_mib(TEST_SIZE);
        memory.set_operating_speed_mhz(TEST_SPEED);
        memory.set_voltage_volt(TEST_VOLTAGE);
        memory.set_data_width_bits(TEST_DATA_WIDTH);
        memory.set_bus_width_bits(TEST_TOTAL_WIDTH);
        memory.set_device_locator(TEST_DEVICE_LOCATOR);
        memory.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));

        response = std::move(memory);
    }

    virtual ~MyGetMemoryInfo();
};

MyGetMemoryInfo::~MyGetMemoryInfo() { }

TEST(GetMemoryInfoTest, PositiveExecute) {
    MyGetMemoryInfo command{"TestMemoryId"};
    GetMemoryInfo::Request request{""};
    GetMemoryInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Memory::MEMORY] = "TestMemoryId";

    EXPECT_NO_THROW(request = GetMemoryInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::Memory::MEMORY_TYPE].isString());
    ASSERT_TRUE(result[literals::Memory::DEVICE_TYPE].isString());
    ASSERT_TRUE(result[literals::Memory::CAPACITY_MIB].isUInt());
    ASSERT_TRUE(result[literals::Memory::OPERATING_SPEED_MHZ].isUInt());
    ASSERT_TRUE(result[literals::Memory::VOLTAGE].isNumeric());
    ASSERT_TRUE(result[literals::Memory::DATA_WIDTH_BITS].isUInt());
    ASSERT_TRUE(result[literals::Memory::BUS_WIDTH_BITS].isUInt());
    ASSERT_TRUE(result[literals::Memory::DEVICE_LOCATOR].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO].isObject());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_EQ(result[literals::Memory::MEMORY_TYPE].asString(), TEST_MEMORY_TYPE.to_string());
    ASSERT_EQ(result[literals::Memory::DEVICE_TYPE].asString(), TEST_DEVICE_TYPE.to_string());
    ASSERT_EQ(result[literals::Memory::CAPACITY_MIB].asUInt(), TEST_SIZE);
    ASSERT_EQ(result[literals::Memory::OPERATING_SPEED_MHZ].asUInt(), TEST_SPEED);
    ASSERT_EQ(result[literals::Memory::VOLTAGE].asDouble(), TEST_VOLTAGE);
    ASSERT_EQ(result[literals::Memory::DATA_WIDTH_BITS].asUInt(), TEST_DATA_WIDTH);
    ASSERT_EQ(result[literals::Memory::BUS_WIDTH_BITS].asUInt(), TEST_TOTAL_WIDTH);
    ASSERT_EQ(result[literals::Memory::DEVICE_LOCATOR].asString(), TEST_DEVICE_LOCATOR);
}

TEST(GetMemoryInfoTest, NegativeModuleNotFound) {
    MyGetMemoryInfo command{"TestMemoryId"};
    GetMemoryInfo::Request request{""};
    GetMemoryInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Memory::MEMORY] = "OtherTestMemoryId";

    EXPECT_NO_THROW(request = GetMemoryInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}

