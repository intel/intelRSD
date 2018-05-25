/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file hotswap_manager.hpp
 *
 * @brief Initial hotswap implementation.
 * */

#include "discovery/storage_discoverer.hpp"

namespace agent {
namespace storage {
namespace watcher {

/*!
 * @brief HotSwap manager implementation
 */
class HotswapManager {
public:
    /*!
     * @brief Constructor
     * @param discoverer Shared pointer to Storage Discoverer
     */
    HotswapManager(discovery::StorageDiscoverer::SPtr discoverer) : m_discoverer(discoverer) {}


    /*!
     * @brief Update storage physical drive information
     */
    void hotswap_discover_hard_drives();


private:
    discovery::StorageDiscoverer::SPtr m_discoverer{};


    void add_drive(agent_framework::model::Drive& detected_drive, const Uuid& storage_service_uuid);


    void remove_drive(const agent_framework::model::Drive& removed_drive);

};
}
}
}

