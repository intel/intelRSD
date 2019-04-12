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

static constexpr enums::SwitchTechnology TEST_TECHNOLOGY
    { enums::SwitchTechnology::Ethernet };

class MyGetEthernetSwitchInfo {
private:
    std::string m_switch{};
public:
    MyGetEthernetSwitchInfo(const std::string& uuid) { m_switch = uuid; }

    void execute(const GetEthernetSwitchInfo::Request& request,
                 GetEthernetSwitchInfo::Response& response) {
        auto uuid = request.get_uuid();

        if (uuid != m_switch) {
            throw std::runtime_error("Not Found");
        }

        EthernetSwitch eth_switch{};
        eth_switch.set_location(std::uint32_t(1));
        eth_switch.set_technology(enums::SwitchTechnology::Ethernet);
        eth_switch.set_mac_address("Test Mac Address");
        eth_switch.set_fru_info(attribute::FruInfo("TestSerialNumber",
                                                   "TestManufacturer",
                                                   "TestModelNumber",
                                                   "TestPartNumber"));
        eth_switch.set_oem(attribute::Oem());
        response = eth_switch;
    }

    virtual ~MyGetEthernetSwitchInfo();
};

MyGetEthernetSwitchInfo::~MyGetEthernetSwitchInfo() { }

TEST(GetEthernetSwitchInfoTest, PositiveExecute) {
    MyGetEthernetSwitchInfo command{"8d2c1ac0-2f82-11e5-8333-0002a5d5c51b"};
    GetEthernetSwitchInfo::Request request{""};
    GetEthernetSwitchInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitch::SWITCH] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

    EXPECT_NO_THROW(request = GetEthernetSwitchInfo::Request::from_json(params));

    EXPECT_NO_THROW(command.execute(request, response));

    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::EthernetSwitch::LOCATION].isUInt());
    ASSERT_TRUE(result[literals::EthernetSwitch::TECHNOLOGY].isString());
    ASSERT_TRUE(result[literals::EthernetSwitch::MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::EthernetSwitch::FRU_INFO].isObject());
    ASSERT_TRUE(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::SERIAL].isString());
    ASSERT_TRUE(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::MANUFACTURER].isString());
    ASSERT_TRUE(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::MODEL].isString());
    ASSERT_TRUE(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::PART].isString());
    ASSERT_TRUE(result[literals::EthernetSwitch::OEM].isObject());
    ASSERT_EQ(result[literals::EthernetSwitch::TECHNOLOGY].asString(), TEST_TECHNOLOGY.to_string());
    ASSERT_EQ(result[literals::EthernetSwitch::MAC_ADDRESS].asString(), "Test Mac Address");
    ASSERT_EQ(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::SERIAL], "TestSerialNumber");
    ASSERT_EQ(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::MANUFACTURER], "TestManufacturer");
    ASSERT_EQ(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::MODEL], "TestModelNumber");
    ASSERT_EQ(result[literals::EthernetSwitch::FRU_INFO][literals::FruInfo::PART], "TestPartNumber");
}

TEST(GetEthernetSwitchInfoTest, NegativeComponentNotFound) {
    MyGetEthernetSwitchInfo command{"8d2c1ac0-2f82-11e5-8333-0002a5d5c51b"};
    GetEthernetSwitchInfo::Request request{""};
    GetEthernetSwitchInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::EthernetSwitch::SWITCH] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_NO_THROW(request = GetEthernetSwitchInfo::Request::from_json(params));

    EXPECT_ANY_THROW(command.execute(request, response));
}
