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

#include "agent-framework/command/network/get_port_vlan_info.hpp"
#include "agent-framework/command/network/json/get_port_vlan_info.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr char TEST_UUID[] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

class GetPortVlanInfo : public network::GetPortVlanInfo {
private:
    std::string m_port_vlan{};
public:
    GetPortVlanInfo(
        const std::string& port_vlan) {
        m_port_vlan = port_vlan;
        }

    using network::GetPortVlanInfo::execute;

    void execute(const Request& request, Response& response) {
        auto port_vlan = request.get_port_vlan();

        if (port_vlan != m_port_vlan) {
            throw exception::NotFound();
        }

        agent_framework::model::PortVlan port{};
        port.set_vlan_id(std::uint32_t(1));
        port.set_vlan_enable(true);
        port.set_tagged(true);
        port.set_oem(attribute::Oem());
        response.set_port_vlan(port);
    }

    virtual ~GetPortVlanInfo();
};

GetPortVlanInfo::~GetPortVlanInfo() { }

TEST(GetPortVlanInfoTest, PositiveExecute) {
    network::json::GetPortVlanInfo command_json;
    GetPortVlanInfo* command = new GetPortVlanInfo(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT_VLAN] = TEST_UUID;

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::PortVlan::VLAN_ID].isUInt());
    ASSERT_TRUE(result[literals::PortVlan::VLAN_ENABLE].isBool());
    ASSERT_TRUE(result[literals::PortVlan::TAGGED].isBool());
    ASSERT_TRUE(result[literals::PortVlan::OEM].isObject());
    ASSERT_EQ(result[literals::PortVlan::VLAN_ID].asUInt(), 1);
    ASSERT_EQ(result[literals::PortVlan::VLAN_ENABLE].asBool(), true);
    ASSERT_EQ(result[literals::PortVlan::TAGGED].asBool(), true);
}

TEST(GetPortVlanInfoTest, NegativeVlanPortNotFound) {
    network::json::GetPortVlanInfo command_json;
    GetPortVlanInfo* command = new GetPortVlanInfo(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT_VLAN] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(GetPortVlanInfoTest, NegativeInvalidUUIDFormat) {
    network::json::GetPortVlanInfo command_json;
    GetPortVlanInfo* command = new GetPortVlanInfo(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT_VLAN] = "TestUUID";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
