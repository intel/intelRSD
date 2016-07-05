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

#include "agent-framework/command/network/get_ethernet_switch_info.hpp"
#include "agent-framework/command/network/json/get_ethernet_switch_info.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::command::exception;

static constexpr enums::SwitchTechnology TEST_TECHNOLOGY
    { enums::SwitchTechnology::Ethernet };

class GetEthernetSwitchInfo : public network::GetEthernetSwitchInfo {
private:
    std::string m_switch{};
public:
    GetEthernetSwitchInfo(const std::string& uuid) { m_switch = uuid; }

    using network::GetEthernetSwitchInfo::execute;

    void execute(const Request& request, Response& response) {
        auto uuid = request.get_switch();

        if (uuid != m_switch) {
            throw exception::NotFound();
        }

        Switch eth_switch{};
        eth_switch.set_location(std::uint32_t(1));
        eth_switch.set_technology(enums::SwitchTechnology::Ethernet);
        eth_switch.set_mac_address("Test Mac Address");
        eth_switch.set_fru_info(attribute::FruInfo("TestSerialNumber",
                                                   "TestManufacturer",
                                                   "TestModelNumber",
                                                   "TestPartNumber"));
        eth_switch.set_oem(attribute::Oem());
        response.set_switch(eth_switch);
    }

    virtual ~GetEthernetSwitchInfo();
};

GetEthernetSwitchInfo::~GetEthernetSwitchInfo() { }

TEST(GetEthernetSwitchInfoTest, PositiveExecute) {
    network::json::GetEthernetSwitchInfo command_json;
    GetEthernetSwitchInfo* command = new GetEthernetSwitchInfo("8d2c1ac0-2f82-11e5-8333-0002a5d5c51b");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Switch::SWITCH] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51b";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[literals::Switch::LOCATION].isUInt());
    ASSERT_TRUE(result[literals::Switch::TECHNOLOGY].isString());
    ASSERT_TRUE(result[literals::Switch::MAC_ADDRESS].isString());
    ASSERT_TRUE(result[literals::Switch::FRU_INFO].isObject());
    ASSERT_TRUE(result[literals::Switch::FRU_INFO][literals::FruInfo::SERIAL].isString());
    ASSERT_TRUE(result[literals::Switch::FRU_INFO][literals::FruInfo::MANUFACTURER].isString());
    ASSERT_TRUE(result[literals::Switch::FRU_INFO][literals::FruInfo::MODEL].isString());
    ASSERT_TRUE(result[literals::Switch::FRU_INFO][literals::FruInfo::PART].isString());
    ASSERT_TRUE(result[literals::Switch::OEM].isObject());
    ASSERT_EQ(result[literals::Switch::TECHNOLOGY].asString(), TEST_TECHNOLOGY.to_string());
    ASSERT_EQ(result[literals::Switch::MAC_ADDRESS].asString(), "Test Mac Address");
    ASSERT_EQ(result[literals::Switch::FRU_INFO][literals::FruInfo::SERIAL], "TestSerialNumber");
    ASSERT_EQ(result[literals::Switch::FRU_INFO][literals::FruInfo::MANUFACTURER], "TestManufacturer");
    ASSERT_EQ(result[literals::Switch::FRU_INFO][literals::FruInfo::MODEL], "TestModelNumber");
    ASSERT_EQ(result[literals::Switch::FRU_INFO][literals::FruInfo::PART], "TestPartNumber");
}

TEST(GetEthernetSwitchInfoTest, NegativeComponentNotFound) {
    network::json::GetEthernetSwitchInfo command_json;
    GetEthernetSwitchInfo* command = new GetEthernetSwitchInfo("8d2c1ac0-2f82-11e5-8333-0002a5d5c51b");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Switch::SWITCH] = "8d2c1ac0-2f82-11e5-8333-0002a5d5c51c";

    EXPECT_ANY_THROW(command_json.method(params, result));
}

TEST(GetEthernetSwitchInfoTest, NegativeInvalidUUIDFormat) {
    network::json::GetEthernetSwitchInfo command_json;
    GetEthernetSwitchInfo* command = new GetEthernetSwitchInfo("8d2c1ac0-2f82-11e5-8333-0002a5d5c51b");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::Switch::SWITCH] = "TestUUID";

    EXPECT_ANY_THROW(command_json.method(params, result));
}
