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

#include "agent-framework/command/network/delete_port_vlan.hpp"
#include "agent-framework/command/network/json/delete_port_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr char TEST_UUID[] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

class DeletePortVlan : public network::DeletePortVlan {
private:
    std::string m_port_vlan{};
public:
    DeletePortVlan(
        std::string port_vlan) {
        m_port_vlan = port_vlan;
        }

    using network::DeletePortVlan::execute;

    void execute(const Request& request, Response& response) {
        auto port_vlan = request.get_port_vlan();

        if (port_vlan != m_port_vlan) {
            throw exception::NotFound();
        }

        response.set_oem(attribute::Oem());
    }

    virtual ~DeletePortVlan();
};

DeletePortVlan::~DeletePortVlan() { }

TEST(DeletePortVlanTest, PositiveExecute) {
    network::json::DeletePortVlan command_json;
    DeletePortVlan* command = new DeletePortVlan(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT_VLAN] = TEST_UUID;

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::PortVlan::OEM].isObject());
}

TEST(DeletePortVlanTest, NegativeVlanPortNotFound) {
    network::json::DeletePortVlan command_json;
    DeletePortVlan* command = new DeletePortVlan(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT_VLAN] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(DeletePortVlanTest, NegativeInvalidUUIDFormat) {
    network::json::DeletePortVlan command_json;
    DeletePortVlan* command = new DeletePortVlan(TEST_UUID);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT_VLAN] = "TestUUID";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
