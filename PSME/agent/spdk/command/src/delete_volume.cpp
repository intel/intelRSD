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
#include "agent-framework/action/task_creator.hpp"
#include "agent-framework/action/task_runner.hpp"



using namespace agent::spdk;
using namespace agent::spdk::utils;
using namespace agent_framework::model;
using namespace agent_framework;

namespace {

void delete_volume(DeleteVolume::ContextPtr context,
                   const DeleteVolume::Request& request,
                   DeleteVolume::Response& response) {

    auto volume = module::get_manager<Volume>().get_entry(request.get_volume());
    auto state = volume.get_status().get_state();

    if (enums::State::Starting == state) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Cannot remove volume with status Starting.");
    }

    if (Volume::is_volume_connected_to_endpoint(volume.get_uuid())) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Unable to remove volume. Endpoints are connected to this volume.");
    }

    auto pools = module::get_m2m_manager<StoragePool, Volume>().get_parents(volume.get_uuid());
    if (pools.empty()) {
        THROW(exceptions::SpdkError, "spdk-agent", "Volume is not assigned to storage pool!");
    }

    auto pool = module::get_manager<StoragePool>().get_entry(pools.front());

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();

    task_creator.add_subtask([context, state, pool, volume]() {
        std::lock_guard<std::mutex> lock{context->spdk_mutex};
        bool success{};

        if (enums::State::Enabled == state) {
            try {
                auto lvol_bdev_name = attribute::Identifier::get_uuid(volume);
                auto lvol_bdev_alias = SpdkConverter::make_alias(pool.get_name(), lvol_bdev_name);

                // FIXME: Workaround for SPDK limitation with deletion of snapshots in chain
                if (Volume::is_volume_snapshot(volume)) {
                    context->hidden_volumes.insert(volume.get_uuid());
                    agent_framework::database::VolumeEntity db(volume.get_uuid());
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
            module::get_manager<Volume>().remove_entry(volume.get_uuid());
            module::get_m2m_manager<StoragePool, Volume>().remove_entry(pool.get_uuid(), volume.get_uuid());
            log_info("spdk-agent", "Volume " << volume.get_uuid() << " removed successfully.");
            agent::spdk::command::refresh_storage_pool(context, pool.get_uuid());
            agent::spdk::command::refresh_volumes(context, pool.get_parent_uuid());

            // TODO: update relations when chain is broken (if absent)
        }
        else {
            THROW(exceptions::SpdkError, "spdk-agent", "Failed to destroy Logical Volume!");
        }
    });

    task_creator.set_promised_response([]() {
        DeleteVolume::Response r{};
        return r.to_json();
    });

    task_creator.set_promised_error_thrower([](const exceptions::GamiException& exception) {
        return exception;
    });

    task_creator.register_task();
    auto delete_task = task_creator.get_task();

    agent_framework::action::TaskRunner::get_instance().run(delete_task);
    log_info("spdk-agent", "Started task deleting volume...");
    response.set_task(task_creator.get_task_resource().get_uuid());

}
}

REGISTER_SPDK_COMMAND(DeleteVolume, ::delete_volume);