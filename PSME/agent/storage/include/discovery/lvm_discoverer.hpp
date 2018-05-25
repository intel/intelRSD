/*!
 * @brief Storage discoverer interface using LVM.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file lvm_discoverer.hpp
 */

#pragma once



#include "discovery/storage_discoverer.hpp"



namespace agent {
namespace storage {
namespace discovery {

class LvmDiscoverer final : public StorageDiscoverer {
public:

    /*! @brief Default constructor */
    LvmDiscoverer() = default;


    /*! @brief Destructor */
    ~LvmDiscoverer();


    bool discover_storage_pools(std::vector<agent_framework::model::StoragePool>& storage_pools,
                                const Uuid& parent_uuid) override;

    bool discover_volumes(std::vector<agent_framework::model::Volume>& volumes,
                          const Uuid& parent_uuid) override;

    bool discover_capacity_sources(agent_framework::model::StoragePool& storage_pool) override;

    /*!
     * @brief Update existing storage pool's capacity sources with LVM data.
     * @param storage_pool Pool to update.
     * @return Returns true if success.
     */
    bool rediscover_pool_capacity(agent_framework::model::StoragePool& storage_pool);

};

}
}
}
