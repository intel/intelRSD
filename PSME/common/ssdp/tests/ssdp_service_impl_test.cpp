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
 *
 * */

#include "ssdp/ssdp_service_impl.hpp"
#include "gtest/gtest.h"

using namespace ssdp;

namespace ssdp {

namespace {

NetworkInterface find_interface() {
    NetworkInterface::List ifaces = NetworkInterface::get_interfaces();
    for (const auto& iface: ifaces) {
        if (iface.supports_multicast() && iface.is_running()) {
            return iface;
        }
    }
    throw net::NetException("No valid network interface found.");
}

}

class SsdpServiceImplTest : public ::testing::Test {
protected:
    SsdpServiceImplTest() {}

    void SetUp() override {
        auto config = std::make_shared<const SsdpServiceConfig>(create_config());
        service_impl.reset(new SsdpServiceImpl(config));
    }

    void TearDown() override { }

    SsdpServiceConfig create_config() {
        SsdpServiceConfig config;
        config.enable_ssdp_service();
        config.add_nic_name(find_interface().get_name());
        config.set_announce_interval(std::chrono::seconds(15));
        config.set_service_url("http://localhost:8888/redfish/v1");
        config.set_service_uuid("7e698e98-f1cc-11e5-bc07-4325b1f9a7b9");
        return config;
    }

    ~SsdpServiceImplTest();

    bool is_running() const {
        return service_impl->is_running();
    }

    MessageQueue<ScheduledMessage>& get_scheduled_msgs() const {
        return service_impl->m_queue;
    }

    SsdpServiceImpl::NetworkInterfaceMap& get_network_interfaces() const {
        return service_impl->m_network_interfaces;
    }

    std::vector<MulticastSocket>& get_notify_sockets() const {
        return service_impl->m_notify_sockets;
    }

    std::unique_ptr<SsdpServiceImpl> service_impl{};
};

}

SsdpServiceImplTest::~SsdpServiceImplTest() {}

TEST_F(SsdpServiceImplTest, AfterConstruction) {
    ASSERT_FALSE(is_running());
    ASSERT_TRUE(get_network_interfaces().empty());
    ASSERT_TRUE(get_notify_sockets().empty());
}
