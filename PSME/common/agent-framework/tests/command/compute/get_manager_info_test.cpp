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

static constexpr char TEST_FIRMWARE_VERSION[] = "Test Firmware Version";
static constexpr char TEST_IPV4_ADDRESS[] = "127.0.0.1";
static constexpr enums::NetworkServiceName TEST_NAME1{
    enums::NetworkServiceName::HTTP};
static constexpr enums::NetworkServiceName TEST_NAME2{
    enums::NetworkServiceName::HTTPS};
static constexpr uint32_t TEST_PORT1 = 22;
static constexpr uint32_t TEST_PORT2 = 70;
static constexpr bool TEST_ENABLED1 = true;
static constexpr bool TEST_ENABLED2 = false;
static constexpr enums::SerialConsoleSignalType TEST_SIGNAL_TYPE{
    enums::SerialConsoleSignalType::Rs232};
static constexpr enums::SerialConsoleParity TEST_PARITY{
    enums::SerialConsoleParity::None};
static constexpr enums::SerialConsoleFlowControl TEST_FLOW_CONTROL{
    enums::SerialConsoleFlowControl::None};
static constexpr enums::SerialConsolePinOut TEST_PIN_OUT{
    enums::SerialConsolePinOut::Cyclades};
static constexpr uint32_t TEST_BIT_RATE = 19200;
static constexpr uint32_t TEST_DATA_BITS = 6;
static constexpr uint32_t TEST_STOP_BITS = 2;
static constexpr bool TEST_CONSOLE_ENABLED = false;


class MyGetManagerInfo {
private:
    std::string m_manager{};
public:
    MyGetManagerInfo(const std::string& manager) { m_manager = manager; }

    void execute(const GetManagerInfo::Request& request,
                 GetManagerInfo::Response& response) {
        const auto& manager_uuid = request.get_uuid();

        if (manager_uuid != m_manager) {
            throw std::runtime_error("Not found");
        }

        agent_framework::model::Manager manager{};
        manager.set_firmware_version(TEST_FIRMWARE_VERSION);
        manager.set_ipv4_address(TEST_IPV4_ADDRESS);
        manager.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));

        agent_framework::model::attribute::NetworkService service1(TEST_NAME1, TEST_PORT1, TEST_ENABLED1);
        agent_framework::model::attribute::NetworkService service2(TEST_NAME2, TEST_PORT2, TEST_ENABLED2);
        manager.add_network_service(service1);
        manager.add_network_service(service2);

        agent_framework::model::attribute::SerialConsole console{};
        console.set_signal_type(TEST_SIGNAL_TYPE);
        console.set_bitrate(TEST_BIT_RATE);
        console.set_parity(TEST_PARITY);
        console.set_data_bits(TEST_DATA_BITS);
        console.set_stop_bits(TEST_STOP_BITS);
        console.set_flow_control(TEST_FLOW_CONTROL);
        console.set_pin_out(TEST_PIN_OUT);
        console.set_enabled(TEST_CONSOLE_ENABLED);
        manager.set_serial_console(console);
        response = manager;
    }

    virtual ~MyGetManagerInfo();
};

MyGetManagerInfo::~MyGetManagerInfo() { }

TEST(GetManagerInfoTest, PositiveExecute) {
    MyGetManagerInfo command{"TestModuleId"};
    GetManagerInfo::Request request{""};
    GetManagerInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Manager::MANAGER] = "TestModuleId";

    EXPECT_NO_THROW(request = GetManagerInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::Manager::FIRMWARE_VERSION].isString());
    ASSERT_TRUE(result[Ipv4Address::IPv4_ADDRESS].isString());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE].isObject());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_TRUE(result[Status::STATUS][Status::STATE].isString());
    ASSERT_TRUE(result[Status::STATUS][Status::HEALTH].isString());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::SIGNAL_TYPE].isString());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::BITRATE].isUInt());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::PARITY].isString());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::DATA_BITS].isUInt());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::STOP_BITS].isUInt());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::FLOW_CONTROL].isString());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::PIN_OUT].isString());
    ASSERT_TRUE(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::ENABLED].isBool());
    ASSERT_EQ(result[literals::Manager::FIRMWARE_VERSION].asString(), TEST_FIRMWARE_VERSION);
    ASSERT_EQ(result[Ipv4Address::IPv4_ADDRESS].asString(), TEST_IPV4_ADDRESS);
    ASSERT_EQ(result[Status::STATUS][Status::STATE].asString(), "InTest");
    ASSERT_EQ(result[Status::STATUS][Status::HEALTH].asString(), "Warning");
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::SIGNAL_TYPE].asString(), TEST_SIGNAL_TYPE.to_string());
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::BITRATE].asUInt(), TEST_BIT_RATE);
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::PARITY].asString(), TEST_PARITY.to_string());
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::DATA_BITS].asUInt(), TEST_DATA_BITS);
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::STOP_BITS].asUInt(), TEST_STOP_BITS);
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::FLOW_CONTROL].asString(),
                TEST_FLOW_CONTROL.to_string());
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::PIN_OUT].asString(), TEST_PIN_OUT.to_string());
    ASSERT_EQ(result[SerialConsole::SERIAL_CONSOLE]
            [SerialConsole::ENABLED].asBool(), TEST_CONSOLE_ENABLED);
    ASSERT_EQ(result[NetworkService::NETWORK_SERVICES][0]
            [NetworkService::NAME], TEST_NAME1.to_string());
    ASSERT_EQ(result[NetworkService::NETWORK_SERVICES][0]
            [NetworkService::ENABLED].asBool(), TEST_ENABLED1);
    if (result[NetworkService::NETWORK_SERVICES][0]
            [NetworkService::ENABLED].asBool()){
        ASSERT_EQ(result[NetworkService::NETWORK_SERVICES][0]
            [NetworkService::PORT].asUInt(), TEST_PORT1);
    }
    ASSERT_EQ(result[NetworkService::NETWORK_SERVICES][1]
            [NetworkService::NAME], TEST_NAME2.to_string());
    ASSERT_EQ(result[NetworkService::NETWORK_SERVICES][1]
            [NetworkService::ENABLED].asBool(), TEST_ENABLED2);
    if (result[NetworkService::NETWORK_SERVICES][1]
            [NetworkService::ENABLED].asBool()){
        ASSERT_EQ(result[NetworkService::NETWORK_SERVICES][1]
            [NetworkService::PORT].asUInt(), TEST_PORT2);
    }
}

TEST(GetManagerInfoTest, NegativeModuleNotFound) {
    MyGetManagerInfo command{"TestModuleId"};
    GetManagerInfo::Request request{""};
    GetManagerInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Manager::MANAGER] = "OtherTestModuleId";

    EXPECT_NO_THROW(request = GetManagerInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}
