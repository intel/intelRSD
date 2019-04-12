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

class MyGetPortVlanInfo {
private:
    std::string m_port_vlan{};
public:
    MyGetPortVlanInfo(
        const std::string& port_vlan) {
        m_port_vlan = port_vlan;
        }

    void execute(const GetPortVlanInfo::Request& request,
                 GetPortVlanInfo::Response& response) {
        auto port_vlan = request.get_uuid();

        if (port_vlan != m_port_vlan) {
            throw std::runtime_error("Not Found");
        }

        agent_framework::model::EthernetSwitchPortVlan port{};
        port.set_vlan_id(std::uint32_t(1));
        port.set_vlan_enable(true);
        port.set_tagged(true);
        port.set_oem(attribute::Oem());
        response = port;
    }

    virtual ~MyGetPortVlanInfo();
};

MyGetPortVlanInfo::~MyGetPortVlanInfo() { }

TEST(GetPortVlanInfoTest, PositiveExecute) {
    MyGetPortVlanInfo command{TEST_UUID};
    GetPortVlanInfo::Request request{""};
    GetPortVlanInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT_VLAN] = TEST_UUID;

    EXPECT_NO_THROW(request = GetPortVlanInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::EthernetSwitchPortVlan::VLAN_ID].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitchPortVlan::VLAN_ENABLE].isBool());
    ASSERT_TRUE(result[literals::EthernetSwitchPortVlan::TAGGED].isBool());
    ASSERT_TRUE(result[literals::EthernetSwitchPortVlan::OEM].isObject());
    ASSERT_EQ(result[literals::EthernetSwitchPortVlan::VLAN_ID].asUInt(), 1);
    ASSERT_EQ(result[literals::EthernetSwitchPortVlan::VLAN_ENABLE].asBool(), true);
    ASSERT_EQ(result[literals::EthernetSwitchPortVlan::TAGGED].asBool(), true);
}

TEST(GetPortVlanInfoTest, NegativeVlanPortNotFound) {
    MyGetPortVlanInfo command{TEST_UUID};
    GetPortVlanInfo::Request request{""};
    GetPortVlanInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT_VLAN] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";
    EXPECT_NO_THROW(request = GetPortVlanInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(GetPortVlanInfoTest, NegativeInvalidUUIDFormat) {
    MyGetPortVlanInfo command{TEST_UUID};
    GetPortVlanInfo::Request request{""};
    GetPortVlanInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT_VLAN] = "TestUUID";

    EXPECT_NO_THROW(request = GetPortVlanInfo::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}
