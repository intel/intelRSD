/*!
 * @brief Implementation of DeleteVolume command.
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
 * @file delete_volume.cpp
 */
#include "database_keys.hpp"
#include "spdk_agent_commands.hpp"
#include "spdk_converter.hpp"
#include "command/helpers/helpers.hpp"

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entities.hpp"



using namespace agent::spdk;
using namespace agent::spdk::utils;
using namespace agent_framework::model;
using namespace agent_framework;

namespace {

void delete_volume(DeleteVolume::ContextPtr context, const DeleteVolume::Request& request,
                   DeleteVolume::Response&) {

    const auto& volume_uuid = request.get_volume();
    const auto& volume = module::get_manager<Volume>().get_entry(volume_uuid);
    const auto& state = volume.get_status().get_state();

    if (enums::State::Starting == state) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Cannot remove volume with status Starting.");
    }

    if (Volume::is_volume_connected_to_endpoint(volume_uuid)) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Unable to remove volume. Endpoints are connected to this volume.");
    }

    bool success{};
    auto pools = module::get_m2m_manager<StoragePool, Volume>().get_parents(volume_uuid);
    if (pools.empty()) {
        THROW(exceptions::SpdkError, "spdk-agent", "Volume is not assigned to storage pool!");
    }
    auto pool = module::get_manager<StoragePool>().get_entry(pools.front());

    std::lock_guard<std::mutex> lock{context->spdk_mutex};
    if (enums::State::Enabled == state) {
        try {
            auto lvol_bdev_name = attribute::Identifier::get_uuid(volume);
            auto lvol_bdev_alias = SpdkConverter::make_alias(pool.get_name(), lvol_bdev_name);

            // FIXME: Workaround for SPDK limitation with deletion of snapshots in chain
            if (Volume::is_volume_snapshot(volume)) {
                context->hidden_volumes.insert(volume_uuid);
                agent_framework::database::VolumeEntity db(volume_uuid);
                db.put(VOLUME_HIDDEN_PROPERTY, std::to_string(1));
                db.put(VOLUME_SPDK_UUID_PROPERTY, volume.get_name());
                success = true;
            }
            else {
                success = context->spdk_api->destroy_lvol_bdev(lvol_bdev_alias);
            }
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent", "Could not remove volume: " + std::string{exception.what()});
        }
    }

    if (success || state == enums::State::Absent) {
        module::get_manager<Volume>().remove_entry(volume_uuid);
        module::get_m2m_manager<StoragePool, Volume>().remove_entry(pool.get_uuid(), volume.get_uuid());
        log_info("spdk-agent", "Volume " << volume_uuid << " removed successfully.");
        agent::spdk::command::refresh_storage_pool(context, pool.get_uuid());
        agent::spdk::command::refresh_volumes(context, pool.get_parent_uuid());

        // TODO: update relations when chain is broken (if absent)
    }
    else {
        THROW(exceptions::SpdkError, "spdk-agent", "Failed to destroy Logical Volume!");
    }
}
}

REGISTER_SPDK_COMMAND(DeleteVolume, ::delete_volume);