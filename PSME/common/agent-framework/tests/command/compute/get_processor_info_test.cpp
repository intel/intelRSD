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

static constexpr char TEST_CPU[] = "CPU 0";
static constexpr enums::ProcessorType TEST_PROCESSOR_TYPE{
    enums::ProcessorType::FPGA};
static constexpr enums::ProcessorArchitecture TEST_PROCESSOR_ARCHITECTURE{
    enums::ProcessorArchitecture::x86};
static constexpr enums::ProcessorInstructionSet TEST_INSTRUCTION_SET{
    enums::ProcessorInstructionSet::ProcessorInstructionSetEnum::ARM_A32};
static constexpr char TEST_MANUFACTURER[] = "Test Manufacturer";
static constexpr char TEST_MODEL_NAME[] = "Test Model Name";
static constexpr enums::ProcessorModel TEST_MODEL{enums::ProcessorModel::E3};
static constexpr int TEST_MHZ = 2700;
static constexpr int TEST_CORES = 4;
static constexpr int TEST_ENABLED_CORES = 2;
static constexpr int TEST_THREADS = 8;
static constexpr int TEST_ENABLED_THREADS = 6;
static constexpr char TEST_VENDOR[] = "Test Vendor";
static constexpr char TEST_NUMERIC_ID[] = "66";
static constexpr char TEST_FAMILY[] = "6";
static constexpr char TEST_CPUID_MODEL[] = "6";
static constexpr char TEST_STEP[] = "2";
static constexpr char TEST_MICROCODE_INFO[] = "4";

class MyGetProcessorInfo {
private:
    std::string m_processor{};
public:
    MyGetProcessorInfo(
        const std::string& processor) {
        m_processor = processor;
    }

    using Processor = agent_framework::model::Processor;
    using CpuId = agent_framework::model::attribute::CpuId;

    void execute(const GetProcessorInfo::Request& request,
                 GetProcessorInfo::Response& response) {
        const auto& processor_uuid = request.get_uuid();

        if (processor_uuid != m_processor) {
            throw std::runtime_error("Not Found");
        }

        Processor processor{};
        processor.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));
        processor.set_socket(TEST_CPU);
        processor.set_processor_type(TEST_PROCESSOR_TYPE);
        processor.set_processor_architecture(TEST_PROCESSOR_ARCHITECTURE);
        processor.set_instruction_set(TEST_INSTRUCTION_SET);
        processor.set_manufacturer(TEST_MANUFACTURER);
        processor.set_model(TEST_MODEL);
        processor.set_model_name(TEST_MODEL_NAME);
        processor.set_max_speed_mhz(TEST_MHZ);
        processor.set_total_cores(TEST_CORES);
        processor.set_enabled_cores(TEST_ENABLED_CORES);
        processor.set_total_threads(TEST_THREADS);
        processor.set_enabled_threads(TEST_ENABLED_THREADS);
        processor.set_oem(agent_framework::model::attribute::Oem());
        CpuId cpu_id{};
        cpu_id.set_vendor_id(TEST_VENDOR);
        cpu_id.set_numeric_id(TEST_NUMERIC_ID);
        cpu_id.set_family(TEST_FAMILY);
        cpu_id.set_model(TEST_CPUID_MODEL);
        cpu_id.set_step(TEST_STEP);
        cpu_id.set_microcode_info(TEST_MICROCODE_INFO);
        processor.set_cpu_id(std::move(cpu_id));
        response = std::move(processor);
    }

    virtual ~MyGetProcessorInfo();
};

MyGetProcessorInfo::~MyGetProcessorInfo() { }

TEST(GetProcessorInfoTest, PositiveExecute) {
    MyGetProcessorInfo command{"TestModuleId"};
    GetProcessorInfo::Request request{""};
    GetProcessorInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Processor::PROCESSOR] = "TestModuleId";

    EXPECT_NO_THROW(request = GetProcessorInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_TRUE(result[Status::STATUS][Status::STATE].isString());
    ASSERT_TRUE(result[Status::STATUS][Status::HEALTH].isString());
    ASSERT_TRUE(result[literals::Processor::SOCKET].isString());
    ASSERT_TRUE(result[literals::Processor::TYPE].isString());
    ASSERT_TRUE(result[literals::Processor::ARCHITECTURE].isString());
    ASSERT_TRUE(result[literals::Processor::INSTRUCTION_SET].isString());
    ASSERT_TRUE(result[literals::Processor::MANUFACTURER].isString());
    ASSERT_TRUE(result[literals::Processor::MODEL].isString());
    ASSERT_TRUE(result[literals::Processor::MODEL_NAME].isString());
    ASSERT_TRUE(result[literals::Processor::MAX_SPEED].isNumeric());
    ASSERT_TRUE(result[literals::Processor::TOTAL_CORES].isNumeric());
    ASSERT_TRUE(result[literals::Processor::ENABLED_CORES].isNumeric());
    ASSERT_TRUE(result[literals::Processor::TOTAL_THREADS].isNumeric());
    ASSERT_TRUE(result[literals::Processor::ENABLED_THREADS].isNumeric());
    ASSERT_TRUE(result[Oem::OEM].isObject());
    ASSERT_TRUE(result[literals::Processor::CPU_ID].isObject());
    ASSERT_EQ(result[Status::STATUS][Status::STATE], "InTest");
    ASSERT_EQ(result[Status::STATUS][Status::HEALTH], "Warning");
    ASSERT_EQ(result[literals::Processor::SOCKET].asString(), TEST_CPU);
    ASSERT_EQ(result[literals::Processor::TYPE].asString(), TEST_PROCESSOR_TYPE.to_string());
    ASSERT_EQ(result[literals::Processor::ARCHITECTURE].asString(), TEST_PROCESSOR_ARCHITECTURE.to_string());
    ASSERT_EQ(result[literals::Processor::INSTRUCTION_SET].asString(), TEST_INSTRUCTION_SET.to_string());
    ASSERT_EQ(result[literals::Processor::MANUFACTURER].asString(), TEST_MANUFACTURER);
    ASSERT_EQ(result[literals::Processor::MODEL].asString(), TEST_MODEL.to_string());
    ASSERT_EQ(result[literals::Processor::MODEL_NAME].asString(), TEST_MODEL_NAME);
    ASSERT_EQ(result[literals::Processor::MAX_SPEED].asInt(), TEST_MHZ);
    ASSERT_EQ(result[literals::Processor::TOTAL_CORES].asInt(), TEST_CORES);
    ASSERT_EQ(result[literals::Processor::ENABLED_CORES].asInt(), TEST_ENABLED_CORES);
    ASSERT_EQ(result[literals::Processor::TOTAL_THREADS].asInt(), TEST_THREADS);
    ASSERT_EQ(result[literals::Processor::ENABLED_THREADS].asInt(), TEST_ENABLED_THREADS);
    ASSERT_EQ(result[literals::Processor::CPU_ID][CpuId::VENDOR_ID].asString(), TEST_VENDOR);
    ASSERT_EQ(result[literals::Processor::CPU_ID][CpuId::NUMERIC_ID].asString(), TEST_NUMERIC_ID);
    ASSERT_EQ(result[literals::Processor::CPU_ID][CpuId::FAMILY].asString(), TEST_FAMILY);
    ASSERT_EQ(result[literals::Processor::CPU_ID][CpuId::MODEL].asString(), TEST_CPUID_MODEL);
    ASSERT_EQ(result[literals::Processor::CPU_ID][CpuId::STEP].asString(), TEST_STEP);
    ASSERT_EQ(result[literals::Processor::CPU_ID][CpuId::MICROCODE_INFO].asString(), TEST_MICROCODE_INFO);
}

TEST(GetProcessorInfoTest, NegativeModuleNotFound) {
    MyGetProcessorInfo command{"TestModuleId"};
    GetProcessorInfo::Request request{""};
    GetProcessorInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Processor::PROCESSOR] = "OtherTestProcessorId";

    EXPECT_NO_THROW(request = GetProcessorInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}
