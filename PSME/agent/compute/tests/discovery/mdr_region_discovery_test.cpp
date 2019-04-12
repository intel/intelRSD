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
#include "loader/compute_loader.hpp"

#include "mock/configuration_full.hpp"
#include "mock/mock_management_controller.hpp"
#include "mock/mock_discovery_manager.hpp"

#include <gtest/gtest.h>

using namespace ipmi;
using namespace agent::compute::loader;
using namespace agent::compute::discovery;
using namespace agent_framework::module;

class MdrRegionTest: public ::testing::Test {
private:
    void load_config_and_build_compute() {
        ComputeLoader loader{};
        json::Json config = json::Json();
        config = json::Json::parse(COMPUTE_FULL_CONFIGURATION);
        bool is_loaded = loader.load(config);

        if (!is_loaded) {
            throw std::runtime_error("Load must be successful before test DOA!");
        }
    }

    std::string get_manager_uuid() {
        auto& module_manager = CommonComponents::get_instance()->get_module_manager();
        auto uuids = module_manager.get_keys("");

        if (uuids.size() < 1) {
            throw std::runtime_error("One manager should be available!");
        }
        return uuids.front();
    }

protected:
    std::string manager_uuid{};
    bool status{false};
    std::uint16_t bytes_to_read{};

public:

    MdrRegionTest() {
        load_config_and_build_compute();
        manager_uuid = get_manager_uuid();
    }

    void SetUp() {
        manager_uuid = get_manager_uuid();
        status = false;
        bytes_to_read = 0;
    }

    virtual ~MdrRegionTest();
};

MdrRegionTest::~MdrRegionTest() { }

TEST_F(MdrRegionTest, IpmiTimeout) {
    auto mock_controller = new MockManagementController();
    mock_controller->set_mdr_test_case(MockManagementController::TestCase::TIMEOUT);
    auto mc = std::unique_ptr<ManagementController>(mock_controller);
    MockDiscoveryManager discovery_manager{mc};

    ASSERT_NO_THROW(std::tie(status, bytes_to_read) = discovery_manager.public_get_mdr_data_region());
    ASSERT_FALSE(status);
    ASSERT_EQ(bytes_to_read, 0);
}

TEST_F(MdrRegionTest, MdrInvalidRegion) {
    auto mock_controller = new MockManagementController();
    mock_controller->set_mdr_test_case(MockManagementController::TestCase::INVALID_REGION);
    auto mc = std::unique_ptr<ManagementController>(mock_controller);
    MockDiscoveryManager discovery_manager{mc};

    ASSERT_NO_THROW(std::tie(status, bytes_to_read) = discovery_manager.public_get_mdr_data_region());
    ASSERT_FALSE(status);
    ASSERT_EQ(bytes_to_read, 0);
}

TEST_F(MdrRegionTest, MdrLockedRegion) {
    auto mock_controller = new MockManagementController();
    mock_controller->set_mdr_test_case(MockManagementController::TestCase::LOCKED_REGION);
    auto mc = std::unique_ptr<ManagementController>(mock_controller);
    MockDiscoveryManager discovery_manager{mc};

    ASSERT_NO_THROW(std::tie(status, bytes_to_read) = discovery_manager.public_get_mdr_data_region());
    ASSERT_FALSE(status);
    ASSERT_EQ(bytes_to_read, 0);
}

TEST_F(MdrRegionTest, MdrEmptyRegion) {
    auto mock_controller = new MockManagementController();
    mock_controller->set_mdr_test_case(MockManagementController::TestCase::EMPTY_REGION);
    auto mc = std::unique_ptr<ManagementController>(mock_controller);
    MockDiscoveryManager discovery_manager{mc};

    ASSERT_NO_THROW(std::tie(status, bytes_to_read) = discovery_manager.public_get_mdr_data_region());
    ASSERT_FALSE(status);
    ASSERT_EQ(bytes_to_read, 0);
}

TEST_F(MdrRegionTest, IpmiThrowException) {
    auto mock_controller = new MockManagementController();
    mock_controller->set_mdr_test_case(MockManagementController::TestCase::EMPTY_REGION);
    auto mc = std::unique_ptr<ManagementController>(mock_controller);
    MockDiscoveryManager discovery_manager{mc};

    ASSERT_NO_THROW(std::tie(status, bytes_to_read) = discovery_manager.public_get_mdr_data_region());
    ASSERT_FALSE(status);
    ASSERT_EQ(bytes_to_read, 0);
}
