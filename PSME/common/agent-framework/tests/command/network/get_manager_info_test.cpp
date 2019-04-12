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

#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command_ref;

static constexpr enums::SerialConsoleParity TEST_PARITY
    { enums::SerialConsoleParity::None };
static constexpr enums::SerialConsoleFlowControl TEST_FLOW
    { enums::SerialConsoleFlowControl::None };
static constexpr enums::SerialConsolePinOut TEST_PIN
    { enums::SerialConsolePinOut::Cisco };

class MyGetManagerInfo {
private:
    std::string m_component{};
public:
    MyGetManagerInfo(const std::string& component) { m_component = component; }

    void execute(const GetManagerInfo::Request& request,
                 GetManagerInfo::Response& response) {
        auto uuid = request.get_uuid();

        if (uuid != m_component) {
            throw std::runtime_error("Not Found");
        }

        attribute::SerialConsole serial_console{};
        serial_console.set_bitrate(8);
        serial_console.set_parity(enums::SerialConsoleParity::None);
        serial_console.set_data_bits(8);
        serial_console.set_stop_bits(8);
        serial_console.set_flow_control(enums::SerialConsoleFlowControl::None);
        serial_console.set_pin_out(enums::SerialConsolePinOut::Cisco);
        serial_console.set_enabled(true);

        Manager manager{};
        manager.set_firmware_version("Test Firmware Version");
        manager.set_ipv4_address("127.0.0.1");
        manager.set_serial_console(serial_console);
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
    ASSERT_TRUE(result[literals::Manager::IPv4_ADDRESS].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE].isObject());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::BITRATE].isUInt());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::PARITY].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::DATA_BITS].isUInt());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::STOP_BITS].isUInt());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::FLOW_CONTROL].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::PIN_OUT].isString());
    ASSERT_TRUE(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::ENABLED].isBool());
    ASSERT_EQ(result[literals::Manager::FIRMWARE_VERSION].asString(), "Test Firmware Version");
    ASSERT_EQ(result[literals::Manager::IPv4_ADDRESS].asString(), "127.0.0.1");
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::BITRATE].asUInt(), 8);
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::PARITY], TEST_PARITY.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::DATA_BITS].asUInt(), 8);
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::STOP_BITS].asUInt(), 8);
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::FLOW_CONTROL], TEST_FLOW.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::PIN_OUT], TEST_PIN.to_string());
    ASSERT_EQ(result[literals::Manager::SERIAL_CONSOLE][literals::SerialConsole::ENABLED].asBool(), true);
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
    EXPECT_NO_THROW(result = response.to_json());
}
