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
 *
 * @file ssdp_service.cpp
 *
 * @brief SsdpService.
 * */

#include "ssdp/ssdp_service.hpp"
#include "ssdp/ssdp_service_impl.hpp"
#include "ssdp/notification.hpp"

namespace ssdp {

SocketAddress SsdpService::SSDP_MCAST_ADDRESS =
                                    SocketAddress{"239.255.255.250", 1900};

SsdpService::SsdpService(const SsdpServiceConfig& config)
    : m_impl(new SsdpServiceImpl(std::make_shared<const SsdpServiceConfig>(config))) {}

SsdpService::~SsdpService() {}

void SsdpService::start() {
    m_impl->start();
}

void SsdpService::stop() {
    m_impl->stop();
}

void SsdpService::on_network_change(unsigned iface_index,
                net::NetworkChangeNotifier::ChangeType change_type) {
    m_impl->post_notification(NetworkChangeNotification(iface_index, change_type));
}

}
