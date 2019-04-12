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

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command-ref/network_commands.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command_ref;

static constexpr char TEST_UUID[] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

class MyGetRemoteEthernetSwitchInfo {
private:
    std::string m_switch{};
public:
    MyGetRemoteEthernetSwitchInfo(const std::string& uuid) {
        m_switch = uuid;
    }

    void execute(const GetRemoteEthernetSwitchInfo::Request& request,
                 GetRemoteEthernetSwitchInfo::Response& response) {
        auto uuid = request.get_uuid();

        if (uuid != m_switch) {
            throw std::runtime_error("Not Found");
        }
        RemoteEthernetSwitch remote_switch{};
        remote_switch.set_switch_identifier("Test Switch Identifier");
        remote_switch.set_mac_address("Test Mac Address");

        for (std::uint32_t i = 0; i < 10; ++i) {
            remote_switch.add_next_hop(attribute::NextHop(1, std::to_string(i),
                                       "127.0.0.1", "FE80::0202:B3FF:FE1E:8329"));
        }
        response = remote_switch;
    }

    virtual ~MyGetRemoteEthernetSwitchInfo();
};

MyGetRemoteEthernetSwitchInfo::~MyGetRemoteEthernetSwitchInfo() { }

TEST(GetRemoteEthernetSwitchInfoTest, PositiveExecute) {
    MyGetRemoteEthernetSwitchInfo command{TEST_UUID};
    GetRemoteEthernetSwitchInfo::Request request{""};
    GetRemoteEthernetSwitchInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::RemoteEthernetSwitch::SWITCH] = TEST_UUID;

    EXPECT_NO_THROW(request = GetRemoteEthernetSwitchInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::NEXT_HOP].isArray());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::SWITCH_IDENTIFIER].isString());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::NEXT_HOP][0][literals::NextHop::PORT_IDENTIFIER].isString());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::NEXT_HOP][0][literals::NextHop::IPV4_ADDRESS].isString());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::NEXT_HOP][0][literals::NextHop::IPV6_ADDRESS].isString());
    ASSERT_TRUE(result[literals::RemoteEthernetSwitch::NEXT_HOP][0][literals::NextHop::METRIC].isUInt());
    ASSERT_EQ(result[literals::RemoteEthernetSwitch::NEXT_HOP][0][literals::NextHop::METRIC].asUInt(), 1);
    ASSERT_EQ(result[literals::RemoteEthernetSwitch::SWITCH_IDENTIFIER].asString(), "Test Switch Identifier");
    ASSERT_EQ(result[literals::RemoteEthernetSwitch::MAC_ADDRESS].asString(), "Test Mac Address");
    for (std::uint32_t i = 0; i < 10; ++i) {
        ASSERT_EQ(result[literals::RemoteEthernetSwitch::NEXT_HOP][i]
                        [literals::NextHop::PORT_IDENTIFIER].asString(), std::to_string(i));
        ASSERT_EQ(result[literals::RemoteEthernetSwitch::NEXT_HOP][i]
                        [literals::NextHop::IPV4_ADDRESS].asString(), "127.0.0.1");
        ASSERT_EQ(result[literals::RemoteEthernetSwitch::NEXT_HOP][i]
                        [literals::NextHop::IPV6_ADDRESS].asString(), "FE80::0202:B3FF:FE1E:8329");
    }
}

TEST(GetRemoteEthernetSwitchInfoTest, NegativeComponentNotFound) {
    MyGetRemoteEthernetSwitchInfo command{TEST_UUID};
    GetRemoteEthernetSwitchInfo::Request request{""};
    GetRemoteEthernetSwitchInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::RemoteEthernetSwitch::SWITCH] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_NO_THROW(request = GetRemoteEthernetSwitchInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(GetRemoteEthernetSwitchInfoTest, NegativeInvalidUUIDFormat) {
    MyGetRemoteEthernetSwitchInfo command{TEST_UUID};
    GetRemoteEthernetSwitchInfo::Request request{""};
    GetRemoteEthernetSwitchInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::RemoteEthernetSwitch::SWITCH] = "TestUUID";

    EXPECT_NO_THROW(request = GetRemoteEthernetSwitchInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}
