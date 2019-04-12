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

#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/command-ref/compute_commands.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command_ref;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;


static constexpr int TEST_FRAME_SIZE = 2048;
static constexpr int TEST_SPEED = 1;
static constexpr int TEST_FULL_DUPLEX = false;
static constexpr bool TEST_AUTOSENSE = true;
static constexpr char TEST_MAC_ADDRESS[] = "TestMACAddress";
static constexpr bool TEST_VLAN_ENABLE = true;

class MyGetNetworkInterfaceInfo {
private:
    std::string m_interface{};
public:
    MyGetNetworkInterfaceInfo(
        const std::string& interface) {
        m_interface = interface;
    }

    void execute(const GetNetworkInterfaceInfo::Request& request,
                 GetNetworkInterfaceInfo::Response& response) {
        const auto& interface_uuid = request.get_uuid();

        if (interface_uuid != m_interface) {
            throw std::runtime_error("Not found");
        }

        agent_framework::model::NetworkInterface interface{};
        interface.set_frame_size(TEST_FRAME_SIZE);
        interface.set_speed_mbps(TEST_SPEED);
        interface.add_ipv4_address({"192.168.0.1", "255.255.255.0",
                enums::Ipv4AddressOrigin::Static, "192.168.0.254"});
        interface.add_ipv6_address({"fe80::2e60:cff:fe5c:ca07", 10,
                enums::Ipv6AddressOrigin::Static, enums::Ipv6AddressState::Preferred});
        interface.set_oem(agent_framework::model::attribute::Oem());
        interface.set_full_duplex(TEST_FULL_DUPLEX);
        interface.set_autosense(TEST_AUTOSENSE);
        interface.set_vlan_enable(TEST_VLAN_ENABLE);
        interface.set_mac_address(TEST_MAC_ADDRESS);
        interface.set_factory_mac_address(TEST_MAC_ADDRESS);
        interface.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));

        response = std::move(interface);
    }

    virtual ~MyGetNetworkInterfaceInfo();
};

MyGetNetworkInterfaceInfo::~MyGetNetworkInterfaceInfo() { }

TEST(GetNetworkInterfaceInfoTest, PositiveExecute) {
    MyGetNetworkInterfaceInfo command{"TestInterfaceId"};
    GetNetworkInterfaceInfo::Request request{""};
    GetNetworkInterfaceInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::NetworkInterface::INTERFACE] = "TestInterfaceId";

    EXPECT_NO_THROW(request = GetNetworkInterfaceInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::NetworkInterface::FRAME_SIZE].isUInt());
    ASSERT_TRUE(result[literals::NetworkInterface::SPEED].isUInt());
    ASSERT_TRUE(result[literals::NetworkInterface::IPv4_ADDRESSES].isArray());
    ASSERT_TRUE(result[literals::NetworkInterface::IPv6_ADDRESSES].isArray());
    ASSERT_TRUE(result[literals::Oem::OEM].isObject());
    ASSERT_TRUE(result[literals::NetworkInterface::FULL_DUPLEX].isBool());
    ASSERT_TRUE(result[literals::NetworkInterface::AUTO_SENSE].isBool());
    ASSERT_TRUE(result[literals::NetworkInterface::VLAN_ENABLE].isBool());
    ASSERT_TRUE(result[literals::NetworkInterface::MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::NetworkInterface::FACTORY_MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::Status::STATUS].isObject());
    ASSERT_EQ(result[literals::NetworkInterface::FRAME_SIZE].asUInt(), TEST_FRAME_SIZE);
    ASSERT_EQ(result[literals::NetworkInterface::SPEED].asUInt(), TEST_SPEED);
    ASSERT_EQ(result[literals::NetworkInterface::FULL_DUPLEX].asBool(), TEST_FULL_DUPLEX);
    ASSERT_EQ(result[literals::NetworkInterface::AUTO_SENSE].asBool(), TEST_AUTOSENSE);
    ASSERT_EQ(result[literals::NetworkInterface::VLAN_ENABLE].asBool(), TEST_VLAN_ENABLE);
    ASSERT_EQ(result[literals::NetworkInterface::MAC_ADDRESS].asString(), TEST_MAC_ADDRESS);
    ASSERT_EQ(result[literals::NetworkInterface::FACTORY_MAC_ADDRESS].asString(), TEST_MAC_ADDRESS);
    ASSERT_EQ(result[literals::Status::STATUS][literals::Status::STATE], "InTest");
    ASSERT_EQ(result[literals::Status::STATUS][literals::Status::HEALTH], "Warning");
}

TEST(GetNetworkInterfaceInfoTest, NegativeInterfaceNotFound) {
    MyGetNetworkInterfaceInfo command{"TestInterfaceId"};
    GetNetworkInterfaceInfo::Request request{""};
    GetNetworkInterfaceInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::NetworkInterface::INTERFACE] = "OtherInterfaceId";

    EXPECT_NO_THROW(request = GetNetworkInterfaceInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}
