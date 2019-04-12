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


#include "gtest/gtest.h"

#include "loader/network_config.hpp"

#include <string>
#include <algorithm>

using namespace agent::network::loader;

class NetworkConfigTest: public ::testing::Test {
protected:
    NetworkConfig config{};
    const std::string PORT_IDENTIFIER{"sw0p1"};
    const std::string SWITCH_IDENTIFIER{"0"};
    const std::string UUID1{"4083539a-49a9-55b8-9269-11567a5e6305"};
    const std::string UUID2{"5083539a-49a9-55b8-9269-11567a5e6305"};
public:

    virtual ~NetworkConfigTest();
};

// To avoid vtable warning.
NetworkConfigTest::~NetworkConfigTest() { }

TEST_F(NetworkConfigTest, EthModeTest) {
    config.add_port_ethmode(PORT_IDENTIFIER, EthMode::Mode1000BaseKX);

    ASSERT_EQ(config.get_port_ethmode(PORT_IDENTIFIER), EthMode::Mode1000BaseKX);
}

TEST_F(NetworkConfigTest, EthModeDefaultTest) {
    ASSERT_EQ(config.get_port_ethmode("abcd"), EthMode::Default);
}

TEST_F(NetworkConfigTest, AutoNegTest) {
    config.add_port_autoneg(PORT_IDENTIFIER, AutoNeg::SGMII);

    ASSERT_EQ(config.get_port_autoneg(PORT_IDENTIFIER), AutoNeg::SGMII);
}

TEST_F(NetworkConfigTest, AutoNegDefaultTest) {
    ASSERT_EQ(config.get_port_autoneg("xyz"), AutoNeg::Default);
}

TEST_F(NetworkConfigTest, SwitchConfigTest) {
    config.add_switch_mgmt_port(SWITCH_IDENTIFIER, "p1p1");

    ASSERT_EQ(config.get_switch_mgmt_port(SWITCH_IDENTIFIER), "p1p1");
}

TEST_F(NetworkConfigTest, MgmtPortDefaultTest) {
    ASSERT_EQ(config.get_switch_mgmt_port("1"), "None");
}

TEST_F(NetworkConfigTest, PublicVlanTest) {
    config.add_public_vlan(4000);
    auto vlans = config.get_public_vlans();
    auto it = find_if(vlans.begin(), vlans.end(),
                           [](uint32_t vlan) { return vlan == 4000; });
    ASSERT_NE(it, vlans.end());
}

TEST_F(NetworkConfigTest, ACLNameTest) {
    config.add_acl_name(UUID1, "acl1");

    ASSERT_EQ(config.get_acl_name(UUID1), "acl1");
}

TEST_F(NetworkConfigTest, ACLNameUUIDTest) {
    ASSERT_EQ(config.get_acl_name(UUID2), UUID2);
}

