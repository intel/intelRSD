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

#include "agent-framework/command/network/get_remote_ethernet_switch_info.hpp"
#include "agent-framework/command/network/json/get_remote_ethernet_switch_info.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr char TEST_UUID[] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

class GetRemoteEthernetSwitchInfo : public network::GetRemoteEthernetSwitchInfo {
private:
    std::string m_switch{};
public:
    GetRemoteEthernetSwitchInfo(
        const std::string& uuid) {
        m_switch = uuid;
        }

    using network::GetRemoteEthernetSwitchInfo::execute;

    void execute(const Request& request, Response& response) {
        auto uuid = request.get_switch();

        if (uuid != m_switch) {
            throw exception::NotFound();
        }
        RemoteSwitch remote_switch{};
        remote_switch.set_switch_identifier("Test Switch Identifier");
        remote_switch.set_mac_address("Test Mac Address");

        for (std::uint32_t i = 0; i < 10; ++i) {
            remote_switch.add_next_hop(attribute::NextHop(1, std::to_string(i),
                                       "127.0.0.1", "FE80::0202:B3FF:FE1E:8329"));
        }
        response.set_switch(remote_switch);
    }

    virtual ~GetRemoteEthernetSwitchInfo();
};

GetRemoteEthernetSwitchInfo::~GetRemoteEthernetSwitchInfo() { }

TEST(GetRemoteEthernetSwitchInfoTest, PositiveExecute) {
    network::json::GetRemoteEthernetSwitchInfo command_json;
    GetRemoteEthernetSwitchInfo* command = new GetRemoteEthernetSwitchInfo(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::RemoteSwitch::SWITCH] = TEST_UUID;

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::RemoteSwitch::NEXT_HOP].isArray());
    ASSERT_TRUE(result[literals::RemoteSwitch::SWITCH_IDENTIFIER].isString());
    ASSERT_TRUE(result[literals::RemoteSwitch::MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::RemoteSwitch::NEXT_HOP][0][literals::NextHop::PORT_IDENTIFIER].isString());
    ASSERT_TRUE(result[literals::RemoteSwitch::NEXT_HOP][0][literals::NextHop::IPV4_ADDRESS].isString());
    ASSERT_TRUE(result[literals::RemoteSwitch::NEXT_HOP][0][literals::NextHop::IPV6_ADDRESS].isString());
    ASSERT_TRUE(result[literals::RemoteSwitch::NEXT_HOP][0][literals::NextHop::METRIC].isUInt());
    ASSERT_EQ(result[literals::RemoteSwitch::NEXT_HOP][0][literals::NextHop::METRIC].asUInt(), 1);
    ASSERT_EQ(result[literals::RemoteSwitch::SWITCH_IDENTIFIER].asString(), "Test Switch Identifier");
    ASSERT_EQ(result[literals::RemoteSwitch::MAC_ADDRESS].asString(), "Test Mac Address");
    for (std::uint32_t i = 0; i < 10; ++i) {
        ASSERT_EQ(result[literals::RemoteSwitch::NEXT_HOP][i]
                        [literals::NextHop::PORT_IDENTIFIER].asString(), std::to_string(i));
        ASSERT_EQ(result[literals::RemoteSwitch::NEXT_HOP][i]
                        [literals::NextHop::IPV4_ADDRESS].asString(), "127.0.0.1");
        ASSERT_EQ(result[literals::RemoteSwitch::NEXT_HOP][i]
                        [literals::NextHop::IPV6_ADDRESS].asString(), "FE80::0202:B3FF:FE1E:8329");
    }
}

TEST(GetRemoteEthernetSwitchInfoTest, NegativeComponentNotFound) {
    network::json::GetRemoteEthernetSwitchInfo command_json;
    GetRemoteEthernetSwitchInfo* command = new GetRemoteEthernetSwitchInfo(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::RemoteSwitch::SWITCH] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(GetRemoteEthernetSwitchInfoTest, NegativeInvalidUUIDFormat) {
    network::json::GetRemoteEthernetSwitchInfo command_json;
    GetRemoteEthernetSwitchInfo* command = new GetRemoteEthernetSwitchInfo(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::RemoteSwitch::SWITCH] = "TestUUID";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
