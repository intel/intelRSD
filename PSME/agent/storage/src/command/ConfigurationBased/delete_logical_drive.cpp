/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/command/storage/delete_logical_drive.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "lvm/lvm_api.hpp"
#include "agent-framework/eventing/event_data.hpp"
#include "hotswap/hotswap_event.hpp"

using namespace agent_framework::command;
using namespace agent_framework::generic;
using namespace agent::storage::lvm;

/*! DeleteLogicalDrive implementation */
class DeleteLogicalDrive : public storage::DeleteLogicalDrive {
public:
    using storage::DeleteLogicalDrive::execute;
    using LogicalDriveSharedPtr = std::shared_ptr<LogicalDrive>;

    bool has_target(const std::string& logical_drive_uuid);

    void execute(const Request& request, Response& response) {
        const auto logical_drive_uuid = request.get_drive();
        const auto logical_drive = ModuleManager::find_logical_drive(logical_drive_uuid).lock();
        if (!logical_drive) {
            THROW(agent_framework::exceptions::InvalidUuid,
                  "storage-agent", "Logical drive not found!");
        }

        if (is_state(logical_drive, "Starting")) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "storage-agent",
                  "Cannot remove logical drive with status 'Starting'.");
        }

        if (has_target(logical_drive_uuid)) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "storage-agent", "Unable to remove logical drive - "
                         "target defined on this drive.");
        }

        if (LogicalDrive::LvmTypes::LOGICAL_VOLUME != logical_drive->get_mode()) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "storage-agent",
                  "Selected logical drive is not Logical Volume.");
        }

        if (logical_drive->is_protected()) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "storage-agent",
                  "Selected logical drive is protected and cannot be removed.");
        }


        LogicalDriveSharedPtr volume_group = logical_drive->get_volume_group();

        if (!volume_group) {
            THROW(agent_framework::exceptions::LvmError,
                  "storage-agent", "Volume group for logical drive not found!");
        }

        LvmAPI lvm_api;
        lvm_api.remove_logical_volume(
                                volume_group->get_name(),
                                logical_drive->get_name());

        log_debug(GET_LOGGER("storage-agent"),
                  "Logical volume removed: " << logical_drive_uuid);
        // remove from cache
        volume_group->delete_logical_drive(logical_drive_uuid);
        response.set_oem(agent_framework::model::attribute::Oem());
    }

    ~DeleteLogicalDrive();

    bool is_state(const LogicalDriveSharedPtr& drive, const std::string& state) {
        return drive->get_status().get_state() == state;
    }
};

bool DeleteLogicalDrive::has_target(const std::string& logical_drive_uuid) {
    for (const auto& module : ModuleManager::get_modules()) {
        for (auto& submodule : module->get_submodules()) {
            auto& target_manager = submodule->get_target_manager();
            for (const auto& target : target_manager.get_targets()) {
                for (const auto& logical_drive : target->get_logical_drives()) {
                    if (logical_drive_uuid == logical_drive->get_uuid()) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

DeleteLogicalDrive::~DeleteLogicalDrive() {}

static Command::Register<DeleteLogicalDrive> g("ConfigurationBased");
