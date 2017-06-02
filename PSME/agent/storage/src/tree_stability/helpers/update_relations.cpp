/*!
 * @brief Provides class for stabilizing compute agent resource tree
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
 * @file update_relations.cpp
 * */

#include "tree_stability/helpers/update_relations.hpp"
#include "agent-framework/module/storage_components.hpp"



using agent_framework::module::StorageComponents;
using agent_framework::model::attribute::Array;
using agent_framework::model::attribute::TargetLun;
using agent_framework::model::enums::LogicalDriveMode;

namespace agent {
namespace storage {
namespace helpers {

void update_logical_drive_in_relations(const std::string& logical_drive_temporary_uuid,
                                       const std::string& logical_drive_persistent_uuid) {
    auto& target_manager = StorageComponents::get_instance()->get_iscsi_target_manager();
    auto& logical_drive_manager = StorageComponents::get_instance()->get_logical_drive_manager();
    auto& physical_drive_manager = StorageComponents::get_instance()->get_physical_drive_manager();

    /*! Targets hold references to physical drives */
    const auto& targets_uuids = target_manager.get_keys();

    auto uuid_update_operation =
        [&logical_drive_temporary_uuid, & logical_drive_persistent_uuid](const TargetLun& lun) -> TargetLun {
            TargetLun updated_lun{lun};
            lun.get_logical_drive() == logical_drive_temporary_uuid ?
            updated_lun.set_logical_drive(logical_drive_persistent_uuid) : void();
            return updated_lun;
        };

    /*! Resolve the conflicts */
    for (const auto& target_uuid : targets_uuids) {
        Array<TargetLun> updated_luns = target_manager.get_entry(target_uuid).get_target_lun();
        std::transform(updated_luns.begin(), updated_luns.end(), updated_luns.begin(), uuid_update_operation);
        target_manager.get_entry_reference(target_uuid)->set_target_lun(updated_luns);
    }




    /*! Physical and Logical Drives hold references to other Logical Drives */
    const auto& physical_drives_uuids = physical_drive_manager.get_keys();
    const auto& logical_drives_uuids = logical_drive_manager.get_keys();

    /*! Resolve dependencies between physical drives and logical drives */
    for (const auto& physical_drive_uuid : physical_drives_uuids) {
        auto physical_drive = physical_drive_manager.get_entry_reference(physical_drive_uuid);
        if (logical_drive_temporary_uuid == physical_drive->get_parent_uuid()) {
            physical_drive->set_parent_uuid(logical_drive_persistent_uuid);
        }
    }

    /*! Resolve dependencies between Logical Drives themselves */
    for (const auto& logical_drive_uuid : logical_drives_uuids) {
        const auto logical_drive = logical_drive_manager.get_entry(logical_drive_uuid);

        /*! Temporary Logical Drive UUID may be located in those fields */
        const auto master_uuid = logical_drive.get_master();
        const auto parent_uuid = logical_drive.get_parent_uuid();

        /*! Only snapshot drives have master UUID, this needs to be checked to avoid exceptions */
        if (master_uuid.has_value() && master_uuid == logical_drive_temporary_uuid) {
            logical_drive_manager.get_entry_reference(logical_drive_uuid)->
                set_master(logical_drive_persistent_uuid);
        }

        if (parent_uuid == logical_drive_temporary_uuid) {
            logical_drive_manager.get_entry_reference(logical_drive_uuid)->
                set_parent_uuid(logical_drive_persistent_uuid);
        }
    }
}


void update_target_in_relations(const std::string& target_temporary_uuid, const std::string target_persistent_uuid) {
    // Update mapping target UUID - target ID in iscsi target manager
    auto& target_manager = StorageComponents::get_instance()->get_iscsi_target_manager();

    auto target_id = target_manager.get_target_id(target_temporary_uuid);
    target_manager.remove_target_id(target_id);
    target_manager.add_target_id(target_id, target_persistent_uuid);
}

}
}
}
