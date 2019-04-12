/*!
 * @brief Implementation of DeleteVolume command.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file delete_volume.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "tools/tools.hpp"
#include "agent-framework/eventing/events_queue.hpp"



using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;

namespace {

void remove_partition(DeleteVolume::ContextPtr context, StoragePool& pool, const Volume& volume) {

    // remove partition from drive and update storage pool
    auto handler = context->drive_handler_factory->get_handler(pool.get_name());
    if (!handler) {
        THROW(exceptions::NvmeError, "nvme-agent", "Unable to get handler for drive " + pool.get_name().value());
    }
    try {
        handler->load();
        handler->remove_volume(volume.get_name());
    }
    catch (const std::exception& e) {
        THROW(exceptions::NvmeError, "nvme-agent", std::string("Unable to remove volume: ") + e.what());
    }
}


void throw_if_volume_in_endpoint(const Volume& volume) {
    auto& manager = get_manager<Endpoint>();
    for (const auto& uuid : manager.get_keys()) {
        auto endpoint = manager.get_entry(uuid);
        for (const auto& ce : endpoint.get_connected_entities()) {
            if ((ce.get_entity_role() == enums::EntityRole::Target) && (ce.get_entity() == volume.get_uuid())) {
                THROW(exceptions::InvalidValue, "nvme-agent", "Volume is in endpoint");
            }
        }
    }
}


void delete_volume(DeleteVolume::ContextPtr context, const DeleteVolume::Request& req, DeleteVolume::Response&) {
    log_info("nvme-agent", "Deleting volume '" << req.get_volume() << "'");
    auto storage_services = get_manager<StorageService>().get_keys();
    if (storage_services.size() != 1) {
        THROW(exceptions::NvmeError, "nvme-agent", "Storage Service was not yet created.");
    }

    const Uuid& volume_uuid{req.get_volume()};
    // check if volume exists
    auto volume = get_manager<Volume>().get_entry(volume_uuid);
    throw_if_volume_in_endpoint(volume);
    const auto& sources = volume.get_capacity_sources();
    if (sources.empty()) {
        THROW(exceptions::NvmeError, "nvme-agent", "Volume has no capacity sources.");
    }
    const auto& pools = sources.front().get_providing_pools();
    if (pools.empty()) {
        THROW(exceptions::NvmeError, "nvme-agent", "Volume has no providing pools.");
    }
    auto pool_uuid = pools.front();
    auto pool = get_manager<StoragePool>().get_entry(pool_uuid);
    remove_partition(context, pool, volume);
    get_m2m_manager<StoragePool, Volume>().remove_entry(pool_uuid, volume_uuid);
    get_manager<Volume>().remove_entry(volume_uuid);
    log_info("nvme-agent", "Removed volume with UUID '" + volume_uuid + "'");

    tools::update_storage_pool_consumed_capacity(pool_uuid);
    attribute::EventData edat{};
    edat.set_component(pool_uuid);
    edat.set_parent(storage_services.front());
    edat.set_type(StoragePool::get_component());
    edat.set_notification(enums::Notification::Update);
    eventing::EventsQueue::get_instance()->push_back(edat);
}

}

REGISTER_NVME_COMMAND(DeleteVolume, ::delete_volume);
