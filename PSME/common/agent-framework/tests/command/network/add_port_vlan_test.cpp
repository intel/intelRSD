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
static constexpr char TEST_VLAN_NAME[] = "TestVlanName";
static constexpr const uint TEST_VLAN_ID = 1;
static constexpr const bool TEST_TAGGED = true;

class MyAddPortVlan {
private:
    std::string m_port{};
    std::string m_vlan_name{};
    std::uint32_t m_vlan_id{};
    bool m_tagged{};

public:
    static constexpr char PORT_VLAN_UUID[] =
                                    "8d2c1ac1-2f82-11e3-8334-0002a5d5c515";

    MyAddPortVlan(
        std::string port,
        std::uint32_t vlan_id,
        std::string vlan_name,
        bool tagged) {
        m_port = port;
        m_vlan_name = vlan_name;
        m_vlan_id = vlan_id;
        m_tagged = tagged;
        }

    void execute(const AddPortVlan::Request& request,
                 AddPortVlan::Response& response) {
        const auto& port = request.get_port();
        const auto& vlan_name = request.get_vlan_name();
        auto vlan_id = request.get_vlan_id();
        auto tagged = request.get_tagged();

        if (port != m_port
            || vlan_name != m_vlan_name
            || vlan_id != m_vlan_id
            || tagged != m_tagged) {
            throw std::runtime_error("Not Found");
        }

        response.set_port_vlan(PORT_VLAN_UUID);
        response.set_oem(attribute::Oem());
    }

    virtual ~MyAddPortVlan();
};

constexpr char MyAddPortVlan::PORT_VLAN_UUID[];

MyAddPortVlan::~MyAddPortVlan() { }

TEST(AddPortVlanTest, PositiveExecute) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = TEST_UUID;
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::EthernetSwitchPortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_NO_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_EQ(result[literals::EthernetSwitchPortVlan::PORT_VLAN], MyAddPortVlan::PORT_VLAN_UUID);
    ASSERT_TRUE(result[literals::EthernetSwitchPortVlan::OEM].isObject());
}

TEST(AddPortVlanTest, NegativeComponentNotFound) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::EthernetSwitchPortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_NO_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(AddPortVlanTest, NegativeInvalidUUIDFormat) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = "TestUUID";
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::EthernetSwitchPortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_NO_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(AddPortVlanTest, NegativePortNotFound) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = TEST_UUID;
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = "OtherVlanId";
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::EthernetSwitchPortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_NO_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(AddPortVlanTest, NegativeVlanNotFound) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = TEST_UUID;
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = 2;
    params[literals::EthernetSwitchPortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_NO_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(AddPortVlanTest, NegativeTaggedNotFound) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = TEST_UUID;
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = TEST_VLAN_ID;
    params[literals::EthernetSwitchPortVlan::TAGGED] = false;

    EXPECT_NO_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}

TEST(AddPortVlanTest, NegativeVlanCommandError) {
    MyAddPortVlan command(TEST_UUID, TEST_VLAN_ID, TEST_VLAN_NAME, TEST_TAGGED);
    AddPortVlan::Request request{""};
    AddPortVlan::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPortVlan::PORT] = TEST_UUID;
    params[literals::EthernetSwitchPortVlan::VLAN_NAME] = TEST_VLAN_NAME;
    params[literals::EthernetSwitchPortVlan::VLAN_ID] = "OtherVlanId";
    params[literals::EthernetSwitchPortVlan::TAGGED] = TEST_TAGGED;

    EXPECT_ANY_THROW(request = AddPortVlan::Request::from_json(params));
    EXPECT_ANY_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());
}
