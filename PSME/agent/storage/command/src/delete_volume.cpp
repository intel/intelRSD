/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 */

#include "agent-framework/action/task_creator.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/eventing/utils.hpp"

#include "discovery/discovery_manager.hpp"
#include "discovery/discoverers/lvm_storage_pool_discoverer.hpp"
#include "tree_stability/storage_stabilizer.hpp"
#include "storage_agent_commands.hpp"

#include "agent/utils/utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent::storage::utils;

namespace {

void delete_volume(DeleteVolume::ContextPtr context,
                   const DeleteVolume::Request& request,
                   DeleteVolume::Response& response) {

    log_info("storage-agent", "Deleting volume with UUID '" << request.get_volume() << "'");

    const auto& volume_uuid = request.get_volume();
    const auto& volume = get_manager<Volume>().get_entry(volume_uuid);

    if (enums::State::Starting == volume.get_status().get_state()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Cannot remove volume with status Starting.");
    }

    if (Volume::is_volume_connected_to_endpoint(volume_uuid)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Unable to remove volume. Endpoints are connected to this volume.");
    }

    if (Volume::is_source_for_another_volume(volume)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Cannot delete Volume because it is source replica for another volume.");
    }

    auto storage_pools = get_m2m_manager<StoragePool, Volume>().get_parents(volume_uuid);
    if (storage_pools.empty()) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "Storage pool for volume not found!");
    }

    const auto& storage_pool_uuid = storage_pools.front();
    auto storage_pool = get_manager<StoragePool>().get_entry(storage_pool_uuid);
    std::string pool_name{};
    std::string volume_name{};
    if (storage_pool.get_name().has_value() && volume.get_name().has_value()) {
        pool_name = storage_pool.get_name().value();
        volume_name = volume.get_name().value();
    }
    else {
        THROW(agent_framework::exceptions::LvmError, "storage-agent",
              "Storage Pool's and Volume's name must be specified.");
    }

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();

    /* Delete volume */
    task_creator.add_subtask([context, pool_name, volume_name, volume_uuid]() {
        try {
            context->lvm_api->remove_logical_volume(pool_name, volume_name);
        }
        catch (const std::exception& ex) {
            THROW(agent_framework::exceptions::LvmError, "lvm", ex.what());
        }

        log_info("storage-agent", "Volume removed: " << volume_uuid);

        /* Remove from the model */
        get_m2m_manager<StoragePool, Volume>().remove_child(volume_uuid);
        get_manager<Volume>().remove_entry(volume_uuid);

        agent_framework::eventing::send_event(volume_uuid, enums::Component::Volume,
                                              agent_framework::model::enums::Notification::Remove,
                                              get_manager<StorageService>().get_keys().front());
    });

    /* Update other volumes */
    task_creator.add_subtask([volume_uuid]() {
        for (const auto& replica_uuid : get_manager<Volume>().get_keys()) {
            auto replica = get_manager<Volume>().get_entry_reference(replica_uuid);
            auto replica_infos = replica->get_replica_infos().get_array();
            auto size_before = replica_infos.size();
            auto new_end_it = std::remove_if(replica_infos.begin(), replica_infos.end(),
                                             [volume_uuid](const ReplicaInfo& replica_info) {
                                                 // Other volume points to removed volume
                                                 return replica_info.get_replica() == volume_uuid;
                                             });

            replica_infos.erase(new_end_it, replica_infos.end());
            if (size_before != replica_infos.size()) {
                /* Something was removed -> send event */
                replica->set_replica_infos(replica_infos);
                agent_framework::eventing::send_update(replica.get_raw_ref());
            }
        }
    });

    /* Update storage pool after delete of volume */
    task_creator.add_subtask([context, storage_pool]() {
        DiscoveryManager(context).rediscover<LvmStoragePoolDiscoverer, StoragePool>(
            storage_pool.get_parent_uuid(), update_storage_pool_relations);
    });

    task_creator.set_promised_response([]() {
        DeleteVolume::Response r{};
        return r.to_json();
    });

    task_creator.set_promised_error_thrower([](const agent_framework::exceptions::GamiException& exception) {
        return exception;
    });

    task_creator.register_task();
    auto delete_task = task_creator.get_task();

    agent_framework::action::TaskRunner::get_instance().run(delete_task);
    log_info("storage-agent", "Started task deleting volume...");
    response.set_task(task_creator.get_task_resource().get_uuid());
}

}

REGISTER_STORAGE_COMMAND(DeleteVolume, ::delete_volume);
