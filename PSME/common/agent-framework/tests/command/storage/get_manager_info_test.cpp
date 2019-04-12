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
using namespace agent_framework::model;

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
    std::string m_component{};
public:
    MyGetManagerInfo(const std::string& component) { m_component = component; }

    void execute(const GetManagerInfo::Request& request,
                 GetManagerInfo::Response& response) {
        auto uuid = request.get_uuid();

        if (uuid != m_component) {
            throw std::runtime_error("Not found");
        }
        Manager manager{};
        manager.set_firmware_version("Test Firmware Version");
        manager.set_ipv4_address("127.0.0.1");

        attribute::SerialConsole console{};
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
};

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
    ASSERT_TRUE(result[literals::Manager::IPv4_ADDRESS].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE].isObject());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::SIGNAL_TYPE].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::BITRATE].isUInt());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::PARITY].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::DATA_BITS].isUInt());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::STOP_BITS].isUInt());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::FLOW_CONTROL].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::PIN_OUT].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE]
                      [literals::SerialConsole::ENABLED].isBool());
    ASSERT_EQ(result[literals::Manager::FIRMWARE_VERSION].asString(), "Test Firmware Version");
    ASSERT_EQ(result[literals::Manager::IPv4_ADDRESS].asString(), "127.0.0.1");
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::SIGNAL_TYPE].asString(), TEST_SIGNAL_TYPE.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::BITRATE].asUInt(), TEST_BIT_RATE);
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::PARITY].asString(), TEST_PARITY.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::DATA_BITS].asUInt(), TEST_DATA_BITS);
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::STOP_BITS].asUInt(), TEST_STOP_BITS);
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::FLOW_CONTROL].asString(), TEST_FLOW_CONTROL.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::PIN_OUT].asString(), TEST_PIN_OUT.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE]
                    [literals::SerialConsole::ENABLED].asBool(), TEST_CONSOLE_ENABLED);
}

TEST(GetManagerInfoTest, NegativeModuleNotFound) {
    MyGetManagerInfo command{"TestModuleId"};
    GetManagerInfo::Request request{""};
    GetManagerInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::Manager::MANAGER] = "OtherTestModuleId";

    EXPECT_NO_THROW(request = GetManagerInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
}
