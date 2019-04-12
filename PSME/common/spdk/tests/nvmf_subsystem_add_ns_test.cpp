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
 * @file nvmf_subsystem_add_ns_test.hpp
 */

#include "spdk/spdk_api.hpp"
#include "connectors/nvmf_subsytem_add_ns_connector.hpp"

#include "gtest/gtest.h"

class NvmfSubsystemAddNsTest : public ::testing::Test {
protected:
    std::shared_ptr<spdk::SpdkApi> spdk_api = {};

    void SetUp() override {
        auto stub_connector = std::make_shared<spdk::NvmfSubsystemAddNsConnector>();
        auto invoker = std::make_shared<json_rpc::JsonRpcRequestInvoker>();
        spdk_api = std::make_shared<::spdk::SpdkApi>(stub_connector, invoker);
    }
};

namespace testing {

TEST_F(NvmfSubsystemAddNsTest, AddingListener) {
    spdk::model::Namespace ns{};
    ns.set_bdev_name("lvol-bdev");

    std::uint32_t nsid{};
    ASSERT_NO_THROW(nsid = spdk_api->nvmf_subsystem_add_ns(spdk::NvmfSubsystemAddNsConnector::SUBSYSTEM_NQN, ns));
    ASSERT_NE(nsid, 0);

    auto nvmf_subsystems = spdk_api->get_nvmf_subsystems();
    auto nvmf_subsystem = nvmf_subsystems.front();
    ASSERT_EQ(nvmf_subsystem.get_nqn(), spdk::NvmfSubsystemAddNsConnector::SUBSYSTEM_NQN);
    ASSERT_FALSE(nvmf_subsystem.get_namespaces().empty());
    ASSERT_EQ(nvmf_subsystem.get_namespaces().front().get_bdev_name(), "lvol-bdev");
    ASSERT_EQ(nvmf_subsystem.get_namespaces().front().get_namespace_id(), nsid);

    ASSERT_FALSE(nvmf_subsystem.get_namespaces().front().get_uuid().value_or_default().empty());
    ASSERT_FALSE(nvmf_subsystem.get_namespaces().front().get_eui_64().value_or_default().empty());
    ASSERT_FALSE(nvmf_subsystem.get_namespaces().front().get_nguid().value_or_default().empty());
}


TEST_F(NvmfSubsystemAddNsTest, AddingEmptyListener) {
    spdk::model::Namespace ns{};
    ASSERT_THROW(spdk_api->nvmf_subsystem_add_ns(
        spdk::NvmfSubsystemAddNsConnector::SUBSYSTEM_NQN, ns),
            std::runtime_error);
}


TEST_F(NvmfSubsystemAddNsTest, InvalidNvmfSubsystemNqn) {
    static constexpr const char INVALID_NQN[] = "nqn.test";
    for (const auto& nvmf : spdk_api->get_nvmf_subsystems()) {
        ASSERT_NE(nvmf.get_nqn(), INVALID_NQN);
    }

    spdk::model::Namespace ns{};
    ns.set_bdev_name("lvol-bdev");
    ASSERT_FALSE(spdk_api->nvmf_subsystem_add_ns(INVALID_NQN, ns));
}

}

