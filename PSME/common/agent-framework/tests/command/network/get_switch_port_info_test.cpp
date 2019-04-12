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

static constexpr enums::PortType TEST_TYPE
    { enums::PortType::Unknown };
static constexpr enums::LinkTechnology TEST_LINK_TECHNOLOGY
    { enums::LinkTechnology::Unknown };

class MyGetEthernetSwitchPortInfo {
private:
    std::string m_port_uuid{};
public:
    MyGetEthernetSwitchPortInfo(const std::string& port_uuid) {
        m_port_uuid = port_uuid;
    }

    void execute(const GetEthernetSwitchPortInfo::Request& request,
                 GetEthernetSwitchPortInfo::Response& response) {
        const auto& port_uuid = request.get_uuid();

        if (port_uuid != m_port_uuid) {
            throw std::runtime_error("Not Found");
        }

        EthernetSwitchPort port{};
        port.set_port_identifier("Test Port Id");
        port.set_port_type(enums::PortType::Unknown);
        port.set_link_technology(enums::LinkTechnology::Unknown);
        port.set_link_speed_mbps(std::uint32_t(1000));
        port.set_max_speed_mbps(std::uint32_t(10000));
        port.set_port_width(std::uint32_t(1));
        port.set_frame_size(std::uint32_t(1));
        port.set_auto_sense(true);
        port.set_is_management_port(true);
        port.set_last_error_code(std::uint32_t(1));
        port.set_error_cleared(true);
        port.set_last_state_change_time("Test State Change Time");
        port.set_mac_address("Test Mac Address");
        port.set_operational_state(enums::OperationalState::Unknown);
        port.set_administrative_state(enums::AdministrativeState::Down);
        port.set_ipv4_address({"192.168.0.1", "255.255.255.0",
                enums::Ipv4AddressOrigin::Static,
                "192.168.0.254"});
        port.set_ipv6_address({"fe80::2e60:cff:fe5c:ca07", 10,
                enums::Ipv6AddressOrigin::Static,
                enums::Ipv6AddressState::Preferred});
        port.set_neighbor_info(attribute::NeighborInfo(
                "TestSwitchIdentifier", "TestPortIdentifier", "TestCableId"));
        port.set_vlan_enable(true);
        port.set_oem(attribute::Oem());
        response = port;
    }

    virtual ~MyGetEthernetSwitchPortInfo();
};

MyGetEthernetSwitchPortInfo::~MyGetEthernetSwitchPortInfo() { }

TEST(GetEthernetSwitchPortInfoTest, PositiveExecute) {
    MyGetEthernetSwitchPortInfo command{"8d2c1ac0-2f82-11e5-8333-0002a5d5c51b"};
    GetEthernetSwitchPortInfo::Request request{""};
    GetEthernetSwitchPortInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPort::PORT] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

    EXPECT_NO_THROW(request = GetEthernetSwitchPortInfo::Request::from_json(params));

    EXPECT_NO_THROW(command.execute(request, response));

    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::PORT_IDENTIFIER].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::LINK_TECHNOLOGY].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::PORT_TYPE].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::LINK_SPEED_MBPS].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::MAX_SPEED_MBPS].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::OPERATIONAL_STATE].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::ADMINISTRATIVE_STATE].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::PORT_WIDTH].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::FRAME_SIZE].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::AUTO_SENSE].isBool());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::IS_MANAGEMENT_PORT].isBool());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::LAST_ERROR_CODE].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::ERROR_CLEARED].isBool());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::LAST_STATE_CHANGE_TIME].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::IPV4_ADDRESS].isObject());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::IPV6_ADDRESS].isObject());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::NEIGHBOR_INFO].isObject());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::VLAN_ENABLE].isBool());
    ASSERT_TRUE(result[literals::EthernetSwitchPort::OEM].isObject());

    ASSERT_EQ(result[literals::EthernetSwitchPort::PORT_IDENTIFIER].asString(), "Test Port Id");
    ASSERT_EQ(result[literals::EthernetSwitchPort::LINK_TECHNOLOGY].asString(),
                    TEST_LINK_TECHNOLOGY.to_string());
    ASSERT_EQ(result[literals::EthernetSwitchPort::PORT_TYPE].asString(), TEST_TYPE.to_string());
    ASSERT_EQ(result[literals::EthernetSwitchPort::LINK_SPEED_MBPS].asUInt(), 1000);
    ASSERT_EQ(result[literals::EthernetSwitchPort::MAX_SPEED_MBPS].asUInt(), 10000);
    ASSERT_EQ(result[literals::EthernetSwitchPort::OPERATIONAL_STATE].asString(), "Unknown");
    ASSERT_EQ(result[literals::EthernetSwitchPort::ADMINISTRATIVE_STATE].asString(), "Down");
    ASSERT_EQ(result[literals::EthernetSwitchPort::PORT_WIDTH].asUInt(), 1);
    ASSERT_EQ(result[literals::EthernetSwitchPort::FRAME_SIZE].asUInt(), 1);
    ASSERT_EQ(result[literals::EthernetSwitchPort::AUTO_SENSE].asBool(), true);
    ASSERT_EQ(result[literals::EthernetSwitchPort::IS_MANAGEMENT_PORT].asBool(), true);
    ASSERT_EQ(result[literals::EthernetSwitchPort::LAST_ERROR_CODE].asUInt(), 1);
    ASSERT_EQ(result[literals::EthernetSwitchPort::ERROR_CLEARED].asBool(), true);
    ASSERT_EQ(result[literals::EthernetSwitchPort::LAST_STATE_CHANGE_TIME].asString(),
                     "Test State Change Time");
    ASSERT_EQ(result[literals::EthernetSwitchPort::MAC_ADDRESS].asString(), "Test Mac Address");
    ASSERT_EQ(result[literals::EthernetSwitchPort::NEIGHBOR_INFO]
                    [literals::NeighborInfo::SWITCH_IDENTIFIER].asString(),
                    "TestSwitchIdentifier");
    ASSERT_EQ(result[literals::EthernetSwitchPort::NEIGHBOR_INFO]
                    [literals::NeighborInfo::PORT_IDENTIFIER].asString(),
                    "TestPortIdentifier");
    ASSERT_EQ(result[literals::EthernetSwitchPort::VLAN_ENABLE].asBool(), true);
}

TEST(GetEthernetSwitchPortInfoTest, NegativeComponentNotFound) {
    MyGetEthernetSwitchPortInfo command{"8d2c1ac0-2f82-11e5-8333-0002a5d5c51b"};
    GetEthernetSwitchPortInfo::Request request{""};
    GetEthernetSwitchPortInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitchPort::PORT] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_NO_THROW(request = GetEthernetSwitchPortInfo::Request::from_json(params));

    EXPECT_ANY_THROW(command.execute(request, response));
}
