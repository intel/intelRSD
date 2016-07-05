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

#include "agent-framework/command/network/add_port_vlan.hpp"
#include "agent-framework/command/network/json/add_port_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr char TEST_UUID[] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";
static constexpr char TEST_VLAN_NAME[] = "TestVlanName";
static constexpr const uint TEST_VLAN_ID = 1;
static constexpr const bool TEST_TAGGED = true;

class AddPortVlan : public network::AddPortVlan {
private:
    std::string m_port{};
    std::string m_vlan_name{};
    std::uint32_t m_vlan_id{};
    bool m_tagged{};

public:
    static constexpr char PORT_VLAN_UUID[] =
                                    "8d2c1ac1-2f82-11e3-8334-0002a5d5c515";

    AddPortVlan(
        std::string port,
        std::uint32_t vlan_id,
        std::string vlan_name,
        bool tagged) {
        m_port = port;
        m_vlan_name = vlan_name;
        m_vlan_id = vlan_id;
        m_tagged = tagged;
        }

    using network::AddPortVlan::execute;

    void execute(const Request& request, Response& response) {
        const auto& port = request.get_port();
        const auto& vlan_name = request.get_vlan_name();
        auto vlan_id = request.get_vlan_id();
        auto tagged = request.get_tagged();

        if (port != m_port
            || vlan_name != m_vlan_name
            || vlan_id != m_vlan_id
            || tagged != m_tagged) {
            throw exception::NotFound();
        }

        response.set_port_vlan(PORT_VLAN_UUID);
        response.set_oem(attribute::Oem());
    }

    virtual ~AddPortVlan();
};

constexpr char AddPortVlan::PORT_VLAN_UUID[];

AddPortVlan::~AddPortVlan() { }

TEST(AddPortVlanTest, PositiveExecute) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = TEST_UUID;
    params[literals::PortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::PortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::PortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(result[literals::PortVlan::PORT_VLAN], AddPortVlan::PORT_VLAN_UUID);
    ASSERT_TRUE(result[literals::PortVlan::OEM].isObject());
}

TEST(AddPortVlanTest, NegativeComponentNotFound) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";
    params[literals::PortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::PortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::PortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddPortVlanTest, NegativeInvalidUUIDFormat) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = "TestUUID";
    params[literals::PortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::PortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::PortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddPortVlanTest, NegativePortNotFound) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = TEST_UUID;
    params[literals::PortVlan::VLAN_NAME] = "OtherVlanId";
    params[literals::PortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::PortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddPortVlanTest, NegativeVlanNotFound) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = TEST_UUID;
    params[literals::PortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::PortVlan::VLAN_ID] = 2;
    params[literals::PortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddPortVlanTest, NegativeTaggedNotFound) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = TEST_UUID;
    params[literals::PortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::PortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::PortVlan::TAGGED] = false;

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(AddPortVlanTest, NegativeVlanCommandError) {
    network::json::AddPortVlan command_json;
    AddPortVlan* command = new AddPortVlan(TEST_UUID,
                                           TEST_VLAN_ID,
                                           TEST_VLAN_NAME,
                                           TEST_TAGGED);

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::PortVlan::PORT] = TEST_UUID;
    params[literals::PortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::PortVlan::VLAN_ID] = "OtherVlanId";
    params[literals::PortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_ANY_THROW(command_json.method(params, result));
}
