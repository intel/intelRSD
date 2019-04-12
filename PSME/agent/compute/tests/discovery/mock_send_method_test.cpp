/*!
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
 * */
#include "agent-framework/module/common_components.hpp"
#include "discovery/discovery_manager.hpp"
#include "loader/compute_loader.hpp"

#include "mock/mock_send_method.hpp"
#include "mock/mock_const.hpp"
#include "mock/configuration_full.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace ipmi;
using namespace agent::compute::loader;
using namespace agent_framework::module;
using namespace agent::compute::discovery;

/*!
 * @brief This class is responsible for testing DiscoveryManager.
 * It checks how DiscoveryManager handle the runtime exception when
 * it's thrown by the send method in mock ManagementController class.
 */
class MockSendMethodTest: public ::testing::Test {
private:
    void load_config_and_build_compute() {
        config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);
        bool is_loaded = loader.load(config);

        if (!is_loaded) {
            throw std::runtime_error("Load must be successful before test DOA!");
        }
    }

    string get_manager_uuid() {
        auto& module_manager = CommonComponents::get_instance()->get_module_manager();
        auto uuids = module_manager.get_keys("");

        if (1 > uuids.size()) {
            throw std::runtime_error("One manager should be available!");
        }
        return uuids.front();
    }

    static bool is_initialized;
protected:

    ComputeLoader loader{};
    json::Json config = json::Json();
    string manager_uuid{};
public:

    MockSendMethodTest() {
        if (is_initialized) {
            return;
        }

        load_config_and_build_compute();
        manager_uuid = get_manager_uuid();
        unique_ptr<ManagementController> mc(new MockSendMethod());
        DiscoveryManager discovery_manager{mc};
        discovery_manager.discovery(manager_uuid);
        is_initialized = true;
    }

    void SetUp() {

    }

    virtual ~MockSendMethodTest();
};

bool MockSendMethodTest::is_initialized{false};

MockSendMethodTest::~MockSendMethodTest() { }

TEST_F(MockSendMethodTest, DiscoverManagerThrowOnSend) {
    try {

    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        ASSERT_TRUE(false);
    }
}
