/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file nvmf-discovery/discovery_service.cpp
 */

#include "nvmf-discovery/discovery_service.hpp"
#include "logger/logger_factory.hpp"
#include "generic/worker_thread.hpp"

namespace nvmf_discovery {

DiscoveryService::DiscoveryService(Interfaces interfaces, DiscoveryEntriesProvider::Ptr discovery_entries_provider)
    : m_ifaces(std::move(interfaces)), m_discovery_entries_provider(std::move(discovery_entries_provider)) {
}

DiscoveryService::~DiscoveryService() {
    stop();
}

void DiscoveryService::start() {
    if (!m_is_running) {
        m_is_running = true;
        std::vector<FiInfo> providers{};
        for (const auto& iface: m_ifaces) {
            providers.push_back(obtain_connection_listener_provider(iface));
        }
        Interfaces::size_type i{0};
        for (auto& provider: providers) {
            m_threads.emplace_back(&DiscoveryService::execute, this, m_ifaces[i++], std::move(provider));
        }
    }
}

void DiscoveryService::stop() {
    if (m_is_running) {
        m_is_running = false;
        std::for_each(std::begin(m_threads), std::end(m_threads), [](std::thread& t) {
            if(t.joinable()) {
                t.join();
            }
        });
    }
}

void DiscoveryService::execute(Interface iface, FiInfo provider) {
    log_info("nvmf-discovery", "Starting nvmf discovery service on " << iface.traddr << ":" << iface.trsvcid);

    try {
        ::generic::WorkerThread worker{"worker-" + iface.traddr + ":" + iface.trsvcid};

        ConnectionListener listener(std::move(provider));
        listener.listen();

        while (m_is_running) {
            if (auto info = listener.wait_for_connection_event(FabricEndpoint::IDLE_TIMEOUT)) {
                try {
                    auto ctrl = std::make_shared<DiscoveryController>(std::move(info),
                                                                      listener.get_access_domain(),
                                                                      listener.get_fabric_domain(),
                                                                      m_discovery_entries_provider);
                    worker([ctrl]() { ctrl->handle_requests(); });
                }
                catch (const std::exception& e) {
                    log_error("nvmf-discovery", "Failed to accept host connection: " << e.what());
                }
            }
        }
    }
    catch (const std::exception& e) {
        log_error("nvmf-discovery", e.what());
    }

    log_info("nvmf-discovery", "Stopping nvmf discovery service on " << iface.traddr << ":" << iface.trsvcid);
}

}
