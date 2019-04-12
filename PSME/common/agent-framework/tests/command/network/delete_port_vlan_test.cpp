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

static constexpr char TEST_UUID[] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

class MyDeletePortVlan {
private:
    std::string m_port_vlan{};
public:
    MyDeletePortVlan(
        std::string port_vlan) {
        m_port_vlan = port_vlan;
        }

    void execute(const DeletePortVlan::Request& request,
                 DeletePortVlan::Response& response) {
        auto port_vlan = request.get_port_vlan();

        if (port_vlan != m_port_vlan) {
            throw std::runtime_error("Not Found");
        }

        response.set_oem(attribute::Oem());
    }

    virtual ~MyDeletePortVlan();
};

MyDeletePortVlan::~MyDeletePortVlan() { }

TEST(DeletePortVlanTest, PositiveExecute) {
    MyDeletePortVlan command{TEST_UUID};
    DeletePortVlan::Request request{""};
    DeletePortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT_VLAN] = TEST_UUID;

    EXPECT_NO_THROW(request = DeletePortVlan::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::EthernetSwitchPortVlan::OEM].isObject());
}

TEST(DeletePortVlanTest, NegativeVlanPortNotFound) {
    MyDeletePortVlan command{TEST_UUID};
    DeletePortVlan::Request request{""};
    DeletePortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT_VLAN] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_NO_THROW(request = DeletePortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(DeletePortVlanTest, NegativeInvalidUUIDFormat) {
    MyDeletePortVlan command{TEST_UUID};
    DeletePortVlan::Request request{""};
    DeletePortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT_VLAN] = "TestUUID";

    EXPECT_NO_THROW(request = DeletePortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}
