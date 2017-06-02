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
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task.hpp"
#include "agent-framework/action/task_creator.hpp"

#include "lvm/lvm_api.hpp"
#include "lvm/lvm_clone_task.hpp"
#include "event/storage_event.hpp"
#include "common/utils.hpp"
#include "tree_stability/storage_tree_stabilizer.hpp"

using namespace agent_framework;
using namespace agent_framework::command_ref;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent::storage::lvm;
using namespace agent::storage::utils;

using agent::storage::StorageTreeStabilizer;

namespace {

std::mutex lvm_mutex{};

void validate_type_and_mode(const AddLogicalDrive::Request& request) {
    // storage agent can only create LVM type of a drive
    LogicalDriveType type = LogicalDriveType::from_string(request.get_type());
    if (LogicalDriveType::LVM != type) {
        THROW(agent_framework::exceptions::UnsupportedValue,
              "storage-agent", "Only LVM type is supported.");
    }

    // storage agent can only create LV mode of a drive
    LogicalDriveMode mode = LogicalDriveMode::from_string(request.get_mode());
    if (LogicalDriveMode::LV != mode) {
        THROW(agent_framework::exceptions::UnsupportedValue,
              "storage-agent", "Only LV mode is supported.");
    }
}


void validate_creation_mode(const AddLogicalDrive::Request& request) {
    const auto master_drive = get_manager<LogicalDrive>().get_entry(request.get_master());

    if (master_drive.is_snapshot() && request.is_snapshot()) {
        THROW(agent_framework::exceptions::UnsupportedValue,
              "storage-agent", "Cannot create snapshot of a snapshot master drive");
    }
}


void validate_capacities(const AddLogicalDrive::Request& request) {
    const auto master_drive = get_manager<LogicalDrive>().get_entry(request.get_master());

    if (!request.is_snapshot()) {
        // A clone drive must have capacity greater than or
        // equal to that of its master drive
        if (!master_drive.get_capacity_gb().has_value()) {
            THROW(agent_framework::exceptions::LvmError,
                  "storage-agent", "Master drive's capacity is unknown.");
        }

        if (request.get_capacity_gb() < master_drive.get_capacity_gb()) {
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", "The specified capacity " + std::to_string(request.get_capacity_gb()) +
                                   "GB is too small to clone a master drive with capacity " +
                                   std::to_string(master_drive.get_capacity_gb()) + "GB."
            );
        }
    }
}


void init_logical_drive(LogicalDrive& logical_drive) {
    logical_drive.set_type(LogicalDriveType::LVM);
    logical_drive.set_mode(LogicalDriveMode::LV);
}


void add_logical_drive(const AddLogicalDrive::Request& req, AddLogicalDrive::Response& rsp) {
    log_debug(GET_LOGGER("storage-agent"), "Adding Logical Drive.");
    validate_type_and_mode(req);
    validate_creation_mode(req);
    validate_capacities(req);

    const auto master_drive = get_manager<LogicalDrive>().get_entry(req.get_master());

    log_debug(GET_LOGGER("storage-agent"),
              "Master drive name = " << master_drive.get_uuid());

    /*
     * volume group is taken from masters volume group in current implementation
     * instead of from request
     */
    auto volume_group = get_manager<LogicalDrive>().get_entry(master_drive.get_parent_uuid());
    if (LogicalDriveMode::LVG != volume_group.get_mode()) {
        THROW(agent_framework::exceptions::InvalidValue,
              "storage-agent", "No volume group for master drive.");
    }

    const auto volume_group_name = get_name_from_device_path(volume_group.get_device_path());
    const auto master_drive_name = get_name_from_device_path(master_drive.get_device_path());

    LogicalDrive logical_drive{volume_group.get_uuid()};
    init_logical_drive(logical_drive);
    const std::string logical_drive_uuid = logical_drive.get_uuid();

    // add entry in Volume group logical drives list
    logical_drive.set_protected(req.is_protected());
    logical_drive.set_capacity_gb(req.get_capacity_gb());
    logical_drive.set_device_path("/dev/" + volume_group_name + "/" + logical_drive_uuid);
    logical_drive.set_master(req.get_master());
    logical_drive.set_is_snapshot(req.is_snapshot());
    logical_drive.set_image(req.get_image());
    logical_drive.set_bootable(master_drive.get_bootable());
    logical_drive.set_status(attribute::Status({State::Enabled, Health::OK}));

    LvmCreateData lvm_create_data{};
    lvm_create_data.set_uuid(logical_drive_uuid);
    lvm_create_data.set_size(req.get_capacity_bytes());
    lvm_create_data.set_volume_group(volume_group_name);
    lvm_create_data.set_logical_volume(master_drive_name);
    lvm_create_data.set_create_name(logical_drive_uuid);
    // @TODO: add bootable flag to GAMI command
    lvm_create_data.set_bootable(true);

    if (req.is_snapshot()) {
        LvmAPI lvm_api;
        lvm_api.create_snapshot(lvm_create_data);

        get_manager<LogicalDrive>().add_entry(std::move(logical_drive));
        const std::string& logical_drive_persistent_uuid = StorageTreeStabilizer().stabilize_logical_drive(
            logical_drive_uuid);

        log_debug(GET_LOGGER("storage-agent"), "Logical volume added: " << logical_drive.get_uuid());

        rsp.set_drive(logical_drive_persistent_uuid);
    } else {
        auto lvm_create_clone_subtask = [lvm_create_data] () {
            lvm_mutex.lock();
            LvmAPI local_lvm_api{};
            local_lvm_api.create_clone(lvm_create_data);
        };

        auto promised_response_builder = [logical_drive, logical_drive_uuid] () {
            lvm_mutex.unlock();
            get_manager<LogicalDrive>().add_entry(std::move(logical_drive));

            const std::string& logical_drive_persistent_uuid = StorageTreeStabilizer().stabilize_logical_drive(
                logical_drive_uuid);

            log_debug(GET_LOGGER("storage-agent"), "Logical volume added: " << logical_drive.get_uuid());

            agent::storage::event::send_event(logical_drive_persistent_uuid, enums::Component::LogicalDrive,
                       eventing::Notification::Add, get_manager<StorageServices>().get_keys().front());

            AddLogicalDrive::Response res{};
            res.set_drive(logical_drive_persistent_uuid);
            return res.to_json();
        };

        auto promised_exception_builder = [] () {
            lvm_mutex.unlock();
            return exceptions::LvmError("Could not create clone");
        };

        action::TaskCreator task_creator;
        task_creator.prepare_task();

        task_creator.add_subtask(lvm_create_clone_subtask);
        task_creator.add_subtask(LvmCloneTask{lvm_create_data});
        task_creator.set_promised_response(promised_response_builder);
        task_creator.set_promised_error_thrower(promised_exception_builder);

        task_creator.register_task();
        auto clone_task = task_creator.get_task();
        action::TaskRunner::get_instance().run(clone_task);

        log_info(GET_LOGGER("storage-agent"), "Logical volume clone started");

        rsp.set_task(task_creator.get_task_resource().get_uuid());
    }
}
}

REGISTER_COMMAND(AddLogicalDrive, ::add_logical_drive);
