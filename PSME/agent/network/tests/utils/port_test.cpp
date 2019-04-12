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

#include "utils/port.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include "loader/network_loader.hpp"
#include "configuration_full.hpp"

#include "json-wrapper/json-wrapper.hpp"


using namespace agent_framework::module;
using namespace agent::network::utils;

class PortTest: public ::testing::Test {
protected:
    agent::network::loader::NetworkLoader loader{};
    json::Json config = json::Json();
public:
    virtual ~PortTest();

    void SetUp() {
        config = json::Json::parse(NETWORK_FULL_CONFIGURATION);
    }

    std::string get_manager_uuid() {
        return CommonComponents::get_instance()->get_module_manager().get_keys("").front();
    }
};

PortTest::~PortTest() {}

TEST_F(PortTest, init_switch_vlan_port) {
    ASSERT_TRUE(loader.load(config));
    init_switch_vlan_port("sw0p1");
}

TEST_F(PortTest, init_switch_vlan_port_not_found) {
    ASSERT_THROW(init_switch_vlan_port("sw0p10"), std::runtime_error);
}
