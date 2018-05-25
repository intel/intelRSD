/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * */

#include "discovery/storage_discoverer.hpp"

namespace tests {

class MockHotswapDiscoverer final : public agent::storage::discovery::StorageDiscoverer {
public:

    MockHotswapDiscoverer(std::initializer_list<agent_framework::model::Drive> list) : m_discovered_drives(list) {

    }

    bool discover_drives(std::vector<agent_framework::model::Drive>& drives, const Uuid& parent_uuid) override {
        drives = m_discovered_drives;
        for(unsigned int i = 0; i < drives.size(); i++) {
            drives[i].set_parent_uuid(parent_uuid);
        }
        return true;
    }

    bool discover_storage_pools(std::vector<agent_framework::model::StoragePool>&, const Uuid&) override {
        return true;
    }

    bool discover_volumes(std::vector<agent_framework::model::Volume>&, const Uuid&) override {
        return true;
    }

    bool discover_capacity_sources(agent_framework::model::StoragePool&) override {
        return true;
    }

private:
    std::vector<agent_framework::model::Drive> m_discovered_drives;

};

}
