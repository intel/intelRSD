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
 * @file nvmf-discovery/discovery_service.hpp
 */

#pragma once

#include "connection_listener.hpp"
#include "discovery_controller.hpp"
#include "interface.hpp"

#include <atomic>
#include <thread>
#include <vector>

namespace nvmf_discovery {

/*!
 * Discovery Service
 */
class DiscoveryService final {
public:
    /*!
     * Constructor
     * @param interfaces Interfaces on which discovery service is listening
     * @param discovery_entries_provider Discovery entries provider
     */
    DiscoveryService(Interfaces interfaces, DiscoveryEntriesProvider::Ptr discovery_entries_provider);

    /*! Destructor */
    ~DiscoveryService();

    /*!
     * Starts Nvmf Discovery Service
     */
    void start();

    /*!
     * Stops Nvmf Discovery Service
     */
    void stop();

private:

    void execute(Interface iface, FiInfo provider);

    std::atomic_bool m_is_running{false};
    std::vector<std::thread> m_threads{};
    Interfaces m_ifaces{};
    DiscoveryEntriesProvider::Ptr m_discovery_entries_provider{};
};

}
