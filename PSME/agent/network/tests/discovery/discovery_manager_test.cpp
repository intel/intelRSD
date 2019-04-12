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

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include "discovery/discovery_manager.hpp"
#include "loader/network_loader.hpp"
#include "configuration_full.hpp"

#include "json-wrapper/json-wrapper.hpp"


#include <string>

using namespace agent::network::discovery;
using namespace agent_framework::module;

class DiscoveryManagerTest: public ::testing::Test {
protected:
    agent::network::loader::NetworkLoader loader{};
    json::Json config = json::Json();
public:
    virtual ~DiscoveryManagerTest();

    void SetUp() {
        config = json::Json::parse(NETWORK_FULL_CONFIGURATION);
    }

    std::string get_manager_uuid() {
        return CommonComponents::get_instance()->get_module_manager().get_keys("").front();
    }
};

DiscoveryManagerTest::~DiscoveryManagerTest() {}

TEST_F(DiscoveryManagerTest, discovery) {
    DiscoveryManager dm{};
    ASSERT_TRUE(loader.load(config));
    dm.discovery(get_manager_uuid());
}

TEST_F(DiscoveryManagerTest, discovery_mesh_ports) {
    DiscoveryManager dm{};
    setenv("MESH_PORTS", "sw0p1", 1);
    dm.discovery(get_manager_uuid());
}

TEST_F(DiscoveryManagerTest, discovery_no_switch) {
    DiscoveryManager dm{};
    auto uuid =
        NetworkComponents::get_instance()->get_switch_manager().get_keys(get_manager_uuid()).front();
    NetworkComponents::get_instance()->get_switch_manager().remove_entry(uuid);
    // no switch, expect discovery to throw exception
    ASSERT_THROW(dm.discovery(get_manager_uuid()), std::runtime_error);
}
