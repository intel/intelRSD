/*!
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file nvmf_subsystem_create_test.cpp
 */

#include "spdk/spdk_api.hpp"
#include "gtest/gtest.h"

#include "connectors/nvmf_subsystem_create_connector.hpp"



class NvmfSubsystemCreateTest : public ::testing::Test {
protected:

    std::shared_ptr<spdk::SpdkApi> spdk_api = {};


    void SetUp() override {

        std::shared_ptr<spdk::NvmfSubsystemCreateConnector> stub_connector = std::make_shared<spdk::NvmfSubsystemCreateConnector>();
        auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);

    }


    NvmfSubsystemCreateTest();


    virtual ~NvmfSubsystemCreateTest();
};


NvmfSubsystemCreateTest::NvmfSubsystemCreateTest() {}


NvmfSubsystemCreateTest::~NvmfSubsystemCreateTest() {}


namespace testing {

TEST_F(NvmfSubsystemCreateTest, NvmfSubsystemCreateWithNqn) {

    ASSERT_TRUE(spdk_api->nvmf_subsystem_create(spdk::NvmfSubsystemCreateConnector::NQN));
    ASSERT_EQ(1, spdk_api->get_nvmf_subsystems().size());
    auto nvmf_subsystem = spdk_api->get_nvmf_subsystems().front();
    ASSERT_EQ(nvmf_subsystem.get_nqn(), spdk::NvmfSubsystemCreateConnector::NQN);
    ASSERT_FALSE(nvmf_subsystem.is_allow_any_host());
    ASSERT_TRUE(nvmf_subsystem.get_hosts().empty());
    ASSERT_TRUE(nvmf_subsystem.get_listen_addresses().empty());

}


TEST_F(NvmfSubsystemCreateTest, NvmfSubsystemCreateWithParams) {

    ASSERT_TRUE(spdk_api->nvmf_subsystem_create(spdk::NvmfSubsystemCreateConnector::NQN, "serial_number", 0, true));
    ASSERT_EQ(1, spdk_api->get_nvmf_subsystems().size());
    auto nvmf_subsystem = spdk_api->get_nvmf_subsystems().front();

    ASSERT_EQ("serial_number", nvmf_subsystem.get_serial_number().value());
    ASSERT_EQ(nvmf_subsystem.get_nqn(), spdk::NvmfSubsystemCreateConnector::NQN);
    ASSERT_TRUE(nvmf_subsystem.is_allow_any_host());
    ASSERT_TRUE(nvmf_subsystem.get_hosts().empty());
    ASSERT_TRUE(nvmf_subsystem.get_listen_addresses().empty());

}


TEST_F(NvmfSubsystemCreateTest, NvmfSubsystemCreateInvalidNqn) {

    ASSERT_FALSE(spdk_api->nvmf_subsystem_create(spdk::NvmfSubsystemCreateConnector::INVALID_NQN));

}

}


