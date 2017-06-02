/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/storage_commands.hpp"

#include "lvm/lvm_api.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "event/storage_event.hpp"
#include "common/utils.hpp"

using namespace agent_framework::command_ref;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent::storage::lvm;
using namespace agent::storage::utils;

namespace {
    bool has_target(const std::string& logical_drive_uuid) {
        for (const auto& target_uuid : get_manager<IscsiTarget>().get_keys()) {
            auto target = get_manager<IscsiTarget>().get_entry(target_uuid);
            for (const auto& lun : target.get_target_lun()) {
                if (logical_drive_uuid == lun.get_logical_drive()) {
                    return true;
                }
            }
        }
        return false;
    }

    void delete_logical_drive(const DeleteLogicalDrive::Request& request, DeleteLogicalDrive::Response&) {
        const auto logical_drive_uuid = request.get_uuid();
        const auto logical_drive = get_manager<LogicalDrive>().get_entry(logical_drive_uuid);

        if (State::Starting == logical_drive.get_status().get_state()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Cannot remove logical drive with status Starting");
        }

        if (has_target(logical_drive_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Unable to remove logical drive - target defined on this drive.");
        }

        if (LogicalDriveMode::LV != logical_drive.get_mode()) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Selected logical drive is not Logical Volume.");
        }

        if (logical_drive.get_protected()){
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
                  "Cannot delete this Logical Drive because it's protected.");
        }

        const auto volume_group = get_manager<LogicalDrive>().get_entry(logical_drive.get_parent_uuid());
        if (LogicalDriveMode::LVG != volume_group.get_mode()) {
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", "Volume group for logical drive not found!");
        }

        LvmAPI lvm_api;
        lvm_api.remove_logical_volume(get_name_from_device_path(volume_group.get_device_path()),
                                      get_name_from_device_path(logical_drive.get_device_path()));

        log_info(GET_LOGGER("storage-agent"), "Logical volume removed: " << logical_drive_uuid);
        // remove from the model
        get_manager<LogicalDrive>().remove_entry(logical_drive_uuid);
    }

}

REGISTER_COMMAND(DeleteLogicalDrive, ::delete_logical_drive);
