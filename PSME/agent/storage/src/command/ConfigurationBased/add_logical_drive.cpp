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

#include "agent-framework/command/storage/add_logical_drive.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "lvm/lvm_api.hpp"
#include "lvm/lvm_clone_task.hpp"
#include "hotswap/hotswap_event.hpp"

using namespace agent_framework::action;
using namespace agent_framework::command;
using namespace agent_framework::generic;
using namespace agent_framework::model::enums;
using namespace agent::storage::lvm;

/*! AddLogicalDrive implementation */
class AddLogicalDrive : public storage::AddLogicalDrive {
public:

    AddLogicalDrive() {
    }

    using storage::AddLogicalDrive::execute;

    void execute(const Request& request, Response& response) {

        validate_type_and_mode(request);

        auto master_drive =
                ModuleManager::find_logical_drive(request.get_master()).lock();

        if (!master_drive) {
            THROW(agent_framework::exceptions::InvalidUuid,
                    "storage-agent", "Master logical drive not found!");
        }
        log_debug(GET_LOGGER("storage-agent"),
                "Master drive name = " << master_drive->get_name());

        if (!request.is_snapshot()) {
            // A clone drive must have capacity greater than or equal
            // to that of its master drive
            if (request.get_capacity_gb() < master_drive->get_capacity_gb()) {
                THROW(agent_framework::exceptions::InvalidParameters, "storage-agent",
                    "The specified capacity " + std::to_string(request.get_capacity_gb()) +
                    "GB is too small to clone a master drive with capacity " +
                    std::to_string(master_drive->get_capacity_gb()) + "GB."
                );
            }
        }

        /*
         * volume group is taken from masters volume group in current implementation
         * instead of from request
         */
        LogicalDriveSharedPtr volume_group = master_drive->get_volume_group();
        if (nullptr == volume_group) {
            THROW(agent_framework::exceptions::LvmError,
                    "storage-agent", "No volume group for master drive!");
        }

        const std::string& volume_group_name{volume_group->get_name()};
        auto logical_drive = LogicalDrive::make_logical_drive();
        const std::string& name{logical_drive->get_uuid()};

        LvmCreateData lvm_create_data{};
        lvm_create_data.set_uuid(logical_drive->get_uuid());
        lvm_create_data.set_size(request.get_capacity_bytes());
        lvm_create_data.set_volume_group(volume_group_name);
        lvm_create_data.set_logical_volume(master_drive->get_name());
        lvm_create_data.set_create_name(name);
        //TODO: add bootable flag to GAMI command
        lvm_create_data.set_bootable(true);
        Status lvm_status{};
        lvm_status.set_health("OK");

        LvmAPI lvm_api;
        if (request.is_snapshot()) {
            lvm_api.create_snapshot(lvm_create_data);
            lvm_status.set_state("Enabled");
        }
        else {
            lvm_api.create_clone(lvm_create_data);
            lvm_status.set_state("Starting");
            TaskRunner::get_instance().run(LvmCloneTask{lvm_create_data});
        }

        log_debug(GET_LOGGER("storage-agent"), "Logical volume added: " << logical_drive->get_uuid());

        // add entry in Volume group logical drives list

        logical_drive->set_bootable(master_drive->is_bootable());

        logical_drive->set_protected(request.is_protected());
        logical_drive->set_capacity_gb(request.get_capacity_gb());
        logical_drive->set_type(request.get_type().to_string());
        logical_drive->set_volume_group(volume_group);
        logical_drive->set_status(lvm_status);
        logical_drive->set_name(name);
        logical_drive->set_mode(request.get_mode().to_string());
        logical_drive->set_device_path("/dev/" + volume_group_name + "/" + name);
        logical_drive->set_master(request.get_master());
        logical_drive->set_image(request.get_image());
        logical_drive->set_snapshot(request.is_snapshot());

        volume_group->add_logical_drive(logical_drive);
        response.set_drive_uuid(logical_drive->get_uuid());
        response.set_oem(agent_framework::model::attribute::Oem());
        log_debug(GET_LOGGER("storage-agent"),
                "Logical Volume " << logical_drive->get_uuid() << " created.");
    }


    ~AddLogicalDrive();

private:
    static void validate_type_and_mode(const Request& request) {
        // storage agent can only create LVM type of a drive
        if (LogicalDriveType::LVM != request.get_type()) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "storage-agent", "Only LVM type is supported");
        }

        // storage agent can only create LV mode of a drive
        if (LogicalDriveMode::LV != request.get_mode()) {
            THROW(agent_framework::exceptions::InvalidParameters,
                  "storage-agent", "Only LV mode is supported");
        }
    }
};

AddLogicalDrive::~AddLogicalDrive() {
}


static Command::Register<AddLogicalDrive> g("ConfigurationBased");
