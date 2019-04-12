/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file nvmf_subsystem_add_listener_test.hpp
 */

#include "spdk/spdk_api.hpp"
#include "gtest/gtest.h"
#include "connectors/nvmf_subsytem_add_listener_connector.hpp"


class NvmfSubsystemAddListenerTest : public ::testing::Test {
protected:
    std::shared_ptr<spdk::SpdkApi> spdk_api = {};

    void SetUp() override {
        auto stub_connector = std::make_shared<spdk::NvmfSubsystemAddListenerConnector>();
        auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }
};

namespace testing {

TEST_F(NvmfSubsystemAddListenerTest, AddingListener) {
    spdk::model::ListenAddress address{};
    address.set_trtype(spdk::model::TransportType::RDMA);
    address.set_traddr("1.2.3.4");
    address.set_trsvcid("1234");
    address.set_adrfam(spdk::model::AddressFamily::FC);

    ASSERT_TRUE(spdk_api->nvmf_subsystem_add_listener(spdk::NvmfSubsystemAddListenerConnector::SUBSYSTEM_NQN, address));


    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_FALSE(nvmf_subsystem.get_listen_addresses().empty());
    ASSERT_EQ(nvmf_subsystem.get_nqn(), spdk::NvmfSubsystemAddListenerConnector::SUBSYSTEM_NQN);

    ASSERT_EQ(nvmf_subsystem.get_listen_addresses()[0].get_traddr(), "1.2.3.4");
    ASSERT_EQ(nvmf_subsystem.get_listen_addresses()[0].get_trtype(), "RDMA");
    ASSERT_EQ(nvmf_subsystem.get_listen_addresses()[0].get_trsvcid().value_or_default(), "1234");
    ASSERT_EQ(nvmf_subsystem.get_listen_addresses()[0].get_adrfam().value_or_default(), "FC");
}


TEST_F(NvmfSubsystemAddListenerTest, AddingEmptyListener) {
    spdk::model::ListenAddress address{};
    ASSERT_THROW(spdk_api->nvmf_subsystem_add_listener(
        spdk::NvmfSubsystemAddListenerConnector::SUBSYSTEM_NQN, address),
            std::runtime_error);
}


TEST_F(NvmfSubsystemAddListenerTest, InvalidNvmfSubsystemNqn) {
    static constexpr const char INVALID_NQN[] = "nqn.test";
    for (const auto& nvmf : spdk_api->get_nvmf_subsystems()) {
        ASSERT_NE(nvmf.get_nqn(), INVALID_NQN);
    }

    spdk::model::ListenAddress address{};
    address.set_trtype(spdk::model::TransportType::RDMA);
    address.set_traddr("1.2.3.4");
    address.set_trsvcid("1234");
    address.set_adrfam(spdk::model::AddressFamily::FC);

    ASSERT_FALSE(spdk_api->nvmf_subsystem_add_listener(INVALID_NQN, address));
}

}

