/*!
 * @brief Implementation of DeleteStoragePool command.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file delete_storage_pool.cpp
 */
#include "spdk_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent::spdk;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

void delete_storage_pool(DeleteStoragePool::ContextPtr, const DeleteStoragePool::Request& request,
                  DeleteStoragePool::Response&) {

    log_info("spdk-agent", "Deleting Storage Pool...");
    auto storage_pool_uuid = request.get_storage_pool();
    auto storage_pool = get_manager<StoragePool>().get_entry(storage_pool_uuid);

    if (storage_pool.get_status().get_state() == enums::State::Absent) {

        auto volumes = module::get_m2m_manager<StoragePool, Volume>().get_children(storage_pool_uuid);

        if(!volumes.empty()){
            THROW(exceptions::InvalidValue, "spdk-agent", "Storage Pool will not be deleted because it contains Volumes. Remove the Volumes from Storage Pool first.");
        }
        get_manager<StoragePool>().remove_entry(storage_pool_uuid);
        log_info("spdk-agent", "Storage Pool " << storage_pool_uuid << " removed successfully");
    }
    else {
        THROW(exceptions::InvalidValue, "spdk-agent", "Cannot delete Storage Pool if its state is not 'absent'.");
    }

}
}
REGISTER_SPDK_COMMAND(DeleteStoragePool, ::delete_storage_pool);