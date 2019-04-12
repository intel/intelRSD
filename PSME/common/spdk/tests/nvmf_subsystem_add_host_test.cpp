/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file nvmf_subsystem_add_host_test.cpp
 */

#include "spdk/spdk_api.hpp"
#include "gtest/gtest.h"

#include "connectors/nvmf_subsytem_add_host_connector.hpp"



class NvmfSubsystemAddHostTest : public ::testing::Test {
protected:

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {

        std::shared_ptr<spdk::NvmfSubsystemAddHostConnector> stub_connector = std::make_shared<spdk::NvmfSubsystemAddHostConnector>();
        auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);

    }


    NvmfSubsystemAddHostTest();


    virtual ~NvmfSubsystemAddHostTest();
};

constexpr const char spdk::NvmfSubsystemAddHostConnector::INVALID_NQN[];


NvmfSubsystemAddHostTest::NvmfSubsystemAddHostTest() {}


NvmfSubsystemAddHostTest::~NvmfSubsystemAddHostTest() {}


namespace testing {

TEST_F(NvmfSubsystemAddHostTest, AddingHostToNvmf) {

    ASSERT_TRUE(spdk_api->nvmf_subsystem_add_host(spdk::NvmfSubsystemAddHostConnector::NQN,
                                                  spdk::NvmfSubsystemAddHostConnector::HOST_NQN));

    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_FALSE(nvmf_subsystem.get_hosts().empty());
    ASSERT_EQ(nvmf_subsystem.get_hosts()[0].get_nqn(), spdk::NvmfSubsystemAddHostConnector::HOST_NQN);
    ASSERT_EQ(nvmf_subsystem.get_nqn(), spdk::NvmfSubsystemAddHostConnector::NQN);
}


TEST_F(NvmfSubsystemAddHostTest, AddingHostToNvmfOverload) {

    spdk::model::Host host{};
    host.set_nqn(spdk::NvmfSubsystemAddHostConnector::HOST_NQN);
    ASSERT_TRUE(spdk_api->nvmf_subsystem_add_host(spdk::NvmfSubsystemAddHostConnector::NQN,
                                                  host));

    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_FALSE(nvmf_subsystem.get_hosts().empty());
    ASSERT_EQ(nvmf_subsystem.get_hosts()[0].get_nqn(), host.get_nqn());
    ASSERT_EQ(nvmf_subsystem.get_nqn(), spdk::NvmfSubsystemAddHostConnector::NQN);
}


TEST_F(NvmfSubsystemAddHostTest, InvalidNvmfSubsystemNqn) {
    for (const auto& nvmf: spdk_api->get_nvmf_subsystems()) {
        ASSERT_NE(nvmf.get_nqn(), spdk::NvmfSubsystemAddHostConnector::INVALID_NQN);
    }
    ASSERT_FALSE(spdk_api->nvmf_subsystem_add_host(spdk::NvmfSubsystemAddHostConnector::INVALID_NQN,
                                                   spdk::NvmfSubsystemAddHostConnector::HOST_NQN));

}

}


