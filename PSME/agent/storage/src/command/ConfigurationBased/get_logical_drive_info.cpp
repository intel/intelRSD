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

#include "agent-framework/command/storage/get_logical_drive_info.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/action/task_status_manager.hpp"

#include <algorithm>

using namespace agent_framework::generic;
using namespace agent_framework::command;

/*! GetLogicalDriveInfo implementation */
class GetLogicalDriveInfo : public storage::GetLogicalDriveInfo {
public:
    GetLogicalDriveInfo() {}

    using storage::GetLogicalDriveInfo::execute;
    using LogicalDriveSharedPtr = LogicalDrive::LogicalDriveSharedPtr;

    void execute(const Request& request, Response& response) {
        auto drive_uuid = request.get_drive();
        auto& module = ModuleManager::get_modules().front();
        if (!module->get_submodules().size()) {
            THROW(agent_framework::exceptions::LvmError, "storage-agent",
                  "Submodules empty!");
        }

        auto& submodule = module->get_submodules().front();
        auto drive_found = find_logical_drive(submodule->get_logical_drives(), drive_uuid);
        if (!drive_found) {
            THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
                  "LogicalDrive '" + drive_uuid + "' not found.");
        }
        check_clone_status(drive_uuid, drive_found);

        agent_framework::model::LogicalDrive logical_drive;
        logical_drive.set_status(agent_framework::model::attribute::Status(
                           drive_found->get_status().get_state(),
                           drive_found->get_status().get_health()));
        logical_drive.set_type(
            agent_framework::model::enums::LogicalDriveType::from_string(
                drive_found->get_type()));
        logical_drive.set_capacity_gb(drive_found->get_capacity_gb());
        logical_drive.set_mode(
            agent_framework::model::enums::LogicalDriveMode::from_string(
                drive_found->get_mode()));
        logical_drive.set_master(drive_found->get_master());
        logical_drive.set_image(drive_found->get_image());
        logical_drive.set_snapshot(drive_found->is_snapshot());
        logical_drive.set_bootable(drive_found->is_bootable());
        logical_drive.set_protected(drive_found->is_protected());
        logical_drive.add_collection(agent_framework::model::attribute::Collection(
                agent_framework::model::enums::CollectionName::LogicalDrives,
                agent_framework::model::enums::CollectionType::LogicalDrives,
                ""));
        logical_drive.add_collection(agent_framework::model::attribute::Collection(
                agent_framework::model::enums::CollectionName::PhysicalDrives,
                agent_framework::model::enums::CollectionType::PhysicalDrives,
                ""));
        response.set_drive(logical_drive);
    }

    static LogicalDriveSharedPtr find_logical_drive(
                const std::vector<LogicalDriveSharedPtr>& logical_drives,
                const std::string& drive_uuid) {

        std::function<LogicalDriveSharedPtr (const LogicalDriveSharedPtr)> internal_find;
        internal_find = [&](const LogicalDriveSharedPtr& logical_drive) {

            auto find_predicate = [&](const LogicalDriveSharedPtr& drive)
            { return bool(internal_find(drive)); };

            if (logical_drive->get_uuid()== drive_uuid) {
                return logical_drive;
            }

            // call recursive for children via find_predicate
            auto first_child = std::begin(logical_drive->get_logical_drives());
            auto last_child = std::end(logical_drive->get_logical_drives());
            auto found = std::find_if(first_child, last_child, find_predicate);

            if (found != last_child) {
                return *found;
            }

            return LogicalDriveSharedPtr{};
        };

        for (const auto& logical_drive : logical_drives) {
            if (auto drive = internal_find(logical_drive)) {
                return drive;
            }
        }

        return {};
    }

    void check_clone_status(const std::string& uuid, LogicalDriveSharedPtr drive) {
        using namespace agent_framework::action;
        bool found{false};
        auto status = TaskStatusManager::get_instance().get_status(uuid);
        if (status == TaskStatusManager::Status::Success) {
            drive->set_status({"Enabled", "OK"});
            found = true;
        }

        if (status == TaskStatusManager::Status::Fail) {
            drive->set_status({"Absent", "OK"});
            found = true;
        }

        if (found) {
            TaskStatusManager::get_instance().remove_status(uuid);
        }
    }

    ~GetLogicalDriveInfo();
};

GetLogicalDriveInfo::~GetLogicalDriveInfo() {}


static Command::Register<GetLogicalDriveInfo> g("ConfigurationBased");
