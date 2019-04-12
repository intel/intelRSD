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
 * @file helpers.cpp
 */

#include "command/helpers/helpers.hpp"
#include "discovery/builders/spdk_storage_pool_builder.hpp"
#include "discovery/discoverers/spdk_volume_discoverer.hpp"
#include "discovery/discovery_manager.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/eventing/utils.hpp"

using namespace agent_framework;

namespace agent {
namespace spdk {
namespace command {

void refresh_storage_pool(AgentContext::SPtr context, const Uuid& source_pool_uuid) {
    auto source_pool = module::get_manager<model::StoragePool>().get_entry(source_pool_uuid);
    auto lvol_store_uuid = model::attribute::Identifier::get_uuid(source_pool);

    try {
        auto lvol_store = context->spdk_api->get_lvol_stores_by_uuid(lvol_store_uuid).at(0);
        agent::spdk::discovery::SpdkStoragePoolBuilder::update_with_spdk(source_pool, lvol_store);
        auto result = module::get_manager<model::StoragePool>().add_or_update_entry(source_pool);
        if (result == agent_framework::module::TableInterface::UpdateStatus::Updated) {
            agent_framework::eventing::send_update(source_pool);
        }
    }
    catch (const std::exception& exception) {
        log_error("spdk-agent", "Could not refresh LVOL store data: " << exception.what());
    }
}

void refresh_volumes(AgentContext::SPtr context, const Uuid& storage_service_uuid) {
    try {
        auto storage_service = module::get_manager<model::StorageService>().get_entry(storage_service_uuid);
        auto volumes = discovery::DiscoveryManager{context}.discover_volumes(storage_service);
        for (auto& volume : volumes) {
            auto result = module::get_manager<model::Volume>().add_or_update_entry(volume);
            if (result == agent_framework::module::TableInterface::UpdateStatus::Updated) {
                agent_framework::eventing::send_update(volume);
            }
        }
    }
    catch (const std::exception& exception) {
        log_error("spdk-agent", "Could not refresh LVOLs data: " << exception.what());
    }
}

}
}
}