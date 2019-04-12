/*!
 * @brief Nvmf subsystem remove host test.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file nvmf_subsystem_remove_host_test.cpp
 */

#include "spdk/spdk_api.hpp"
#include "gtest/gtest.h"

#include "connectors/nvmf_subsystem_remove_host_connector.hpp"



class NvmfSubsystemRemoveHostTest : public ::testing::Test {
protected:

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {

        std::shared_ptr<spdk::NvmfSubsystemRemoveHostConnector> stub_connector = std::make_shared<spdk::NvmfSubsystemRemoveHostConnector>();
        auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();

        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);

    }


    NvmfSubsystemRemoveHostTest();


    virtual ~NvmfSubsystemRemoveHostTest();
};

constexpr const char spdk::NvmfSubsystemRemoveHostConnector::INVALID_NQN[];


NvmfSubsystemRemoveHostTest::NvmfSubsystemRemoveHostTest() {}


NvmfSubsystemRemoveHostTest::~NvmfSubsystemRemoveHostTest() {}


namespace testing {

TEST_F(NvmfSubsystemRemoveHostTest, RemovingHostFromNvmf) {

    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_FALSE(nvmf_subsystem.get_hosts().empty());

    ASSERT_TRUE(spdk_api->nvmf_subsystem_remove_host(spdk::NvmfSubsystemRemoveHostConnector::NQN,
                                                  spdk::NvmfSubsystemRemoveHostConnector::HOST_NQN));

    nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_TRUE(nvmf_subsystem.get_hosts().empty());
}

TEST_F(NvmfSubsystemRemoveHostTest, RemoveHostFromNvmfOverload) {

    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_FALSE(nvmf_subsystem.get_hosts().empty());

    spdk::model::Host host{};
    host.set_nqn(spdk::NvmfSubsystemRemoveHostConnector::HOST_NQN);
    ASSERT_TRUE(spdk_api->nvmf_subsystem_remove_host(spdk::NvmfSubsystemRemoveHostConnector::NQN,
                                                  host));

    nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_TRUE(nvmf_subsystem.get_hosts().empty());
}


TEST_F(NvmfSubsystemRemoveHostTest, InvalidNvmfSubsystemNqn) {
    for (const auto& nvmf: spdk_api->get_nvmf_subsystems()) {
        ASSERT_NE(nvmf.get_nqn(), spdk::NvmfSubsystemRemoveHostConnector::INVALID_NQN);
    }
    ASSERT_FALSE(spdk_api->nvmf_subsystem_remove_host(spdk::NvmfSubsystemRemoveHostConnector::INVALID_NQN,
                                                   spdk::NvmfSubsystemRemoveHostConnector::HOST_NQN));

    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_FALSE(nvmf_subsystem.get_hosts().empty());
}

}


