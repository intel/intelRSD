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

#include "agent-framework/command/network/get_manager_info.hpp"
#include "agent-framework/command/network/json/get_manager_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr enums::SerialConsoleParity TEST_PARITY
    { enums::SerialConsoleParity::None };
static constexpr enums::SerialConsoleFlowControl TEST_FLOW
    { enums::SerialConsoleFlowControl::None };
static constexpr enums::SerialConsolePinOut TEST_PIN
    { enums::SerialConsolePinOut::Cisco };

class GetManagerInfo : public network::GetManagerInfo {
private:
    std::string m_component{};
public:
    GetManagerInfo(const std::string& component) { m_component = component; }

    using network::GetManagerInfo::execute;

    void execute(const Request& request, Response& response) {
        auto uuid = request.get_manager();

        if (uuid != m_component) {
            throw exception::NotFound();
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
        response.set_manager(manager);
    }

    virtual ~GetManagerInfo();
};

GetManagerInfo::~GetManagerInfo() { }

TEST(GetManagerInfoTest, PositiveExecute) {
    network::json::GetManagerInfo command_json;
    GetManagerInfo* command = new GetManagerInfo("TestModuleId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Manager::MANAGER] = "TestModuleId";

    EXPECT_NO_THROW(command_json.method(params, result));

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
    network::json::GetManagerInfo command_json;
    GetManagerInfo* command = new GetManagerInfo("TestModuleId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Manager::MANAGER] = "OtherTestModuleId";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
