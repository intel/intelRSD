/*!
 * @brief Storage pool discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file spdk_storage_pool_discoverer.cpp
 */

#include "discovery/discoverers/spdk_storage_pool_discoverer.hpp"
#include "discovery/builders/spdk_storage_pool_builder.hpp"

#include "agent-framework/module/storage_components.hpp"

#include "spdk/spdk_api.hpp"

using namespace agent::spdk;
using namespace agent::spdk::discovery;
using namespace agent_framework::model;
using namespace spdk::model;
using namespace spdk;

std::vector<StoragePool> SpdkStoragePoolDiscoverer::discover(const Uuid& parent_uuid) {
    std::vector<StoragePool> storage_pools{};
    std::vector<LvolStore> lvol_stores{};
    try {
        lvol_stores = m_context->spdk_api->get_lvol_stores();
    }
    catch (const std::exception& exception) {
        log_error("spdk-discovery", "Failed to call get_lvol_stores method on SPDK: " << exception.what());
    }

    for (const auto& lvol_store: lvol_stores) {
        try {
            log_debug("spdk-discovery", "\n" << lvol_store.to_string());
            auto pool = SpdkStoragePoolBuilder::build_default(parent_uuid);
            SpdkStoragePoolBuilder::update_with_spdk(pool, lvol_store);

            const auto& bdev_name = lvol_store.get_base_bdev();
            auto drive_uuids = agent_framework::module::get_manager<Drive>().get_keys([bdev_name](const Drive& drive) {
                return drive.get_name() == bdev_name;
            });
            for (const auto& drive_uuid : drive_uuids) {
                SpdkStoragePoolBuilder::add_drive_capacity_source(pool,
                    pool.get_capacity().get_allocated_bytes(),
                    // We are consuming all allocated bytes from providing drive
                    pool.get_capacity().get_allocated_bytes(),
                    drive_uuid);
            }

            storage_pools.emplace_back(std::move(pool));
        }
        catch (const std::exception& exception) {
            log_error("spdk-discovery",
                "Failed to convert storage pool model from SPDK LVOL Store: " << exception.what());
        }
    }
    return storage_pools;

}