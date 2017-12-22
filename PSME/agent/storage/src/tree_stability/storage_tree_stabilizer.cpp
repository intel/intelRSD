/*!
 * @brief Provides class for stabilizing storage agent resource tree
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
 * @file storage_tree_stabilizer.cpp
 * */

#include "tree_stability/storage_tree_stabilizer.hpp"
#include "tree_stability/helpers/update_relations.hpp"

#include "agent-framework/logger_ext.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include <algorithm>
#include <string>



using namespace agent_framework::module;
using namespace agent::storage;


using agent_framework::model::attribute::Array;
using agent_framework::model::attribute::TargetLun;
using agent_framework::model::enums::LogicalDriveMode;

/*! Auxiliary namespace to hold prefixes for unique key construction, needed to maintain data integrity */
namespace literals {
constexpr const char MODULE_KEY_BASE[] = "Module_";
constexpr const char SERVICE_KEY_BASE[] = "StorageService_";
constexpr const char TARGET_KEY_BASE[] = "Target_";
constexpr const char LOGICAL_DRIVE_KEY_BASE[] = "LogicalDrive_";
constexpr const char PHYSICAL_DRIVE_KEY_BASE[] = "PhysicalDrive_";
}


StorageTreeStabilizer::~StorageTreeStabilizer() { }


const std::string StorageTreeStabilizer::stabilize_target(const std::string& target_uuid) {
    auto& target_manager = StorageComponents::get_instance()->get_iscsi_target_manager();
    auto target = target_manager.get_entry(target_uuid);

    if (target.get_target_iqn().has_value()) {
        const auto target_iqn = target.get_target_iqn().value();
        const auto target_persistent_uuid = stabilize_single_resource(target_uuid, target_manager,
                                                                     literals::TARGET_KEY_BASE + target_iqn);
        helpers::update_target_in_relations(target_uuid, target_persistent_uuid);
        return target_persistent_uuid;
    }
    else {
        log_key_value_missing(target.get_component().to_string(), target_uuid);
        return target_uuid;
    }
}


const std::string StorageTreeStabilizer::stabilize_logical_drive(const std::string& drive_uuid) {
    auto& logical_drive_manager = StorageComponents::get_instance()->get_logical_drive_manager();

    auto logical_drive = logical_drive_manager.get_entry(drive_uuid);

    auto device_path = logical_drive.get_device_path();
    const auto drive_persistent_uuid = stabilize_single_resource(drive_uuid, logical_drive_manager,
                                                                 literals::LOGICAL_DRIVE_KEY_BASE + device_path);
    helpers::update_logical_drive_in_relations(drive_uuid, drive_persistent_uuid);
    return drive_persistent_uuid;
}


const std::string StorageTreeStabilizer::stabilize_physical_drive(const std::string& drive_uuid) {
    auto& physical_drive_manager = StorageComponents::get_instance()->get_physical_drive_manager();

    auto physical_drive = physical_drive_manager.get_entry(drive_uuid);
    const auto serial_number = physical_drive.get_fru_info().get_serial_number();

    if (serial_number.has_value()) {
        const std::string& drive_persistent_uuid = stabilize_single_resource(drive_uuid, physical_drive_manager,
                                  literals::PHYSICAL_DRIVE_KEY_BASE + serial_number.value());
        return drive_persistent_uuid;
    }
    else {
        log_key_value_missing(physical_drive.get_component().to_string(), drive_uuid);
        return drive_uuid;
    }
}


const std::string StorageTreeStabilizer::stabilize_storage_service(const std::string& service_uuid) {
    auto& service_manager = StorageComponents::get_instance()->get_storage_service_manager();
    auto& physical_drive_manager = StorageComponents::get_instance()->get_physical_drive_manager();
    auto& logical_drive_manager = StorageComponents::get_instance()->get_logical_drive_manager();
    auto& target_manager = StorageComponents::get_instance()->get_iscsi_target_manager();

    auto service = service_manager.get_entry(service_uuid);
    const auto& physical_drives_uuids = physical_drive_manager.get_keys();
    const auto& logical_drives_uuids = logical_drive_manager.get_keys();
    const auto& targets_uuids = target_manager.get_keys(service_uuid);

    const auto service_persistent_uuid = stabilize_single_resource(service_uuid, service_manager,
                                                                   literals::SERVICE_KEY_BASE);

    for (const auto& physical_drive_uuid : physical_drives_uuids) {
        stabilize_physical_drive(physical_drive_uuid);
    }

    for (const auto& logical_drive_uuid : logical_drives_uuids) {
        stabilize_logical_drive(logical_drive_uuid);
    }

    for (const auto& target_uuid : targets_uuids) {
        target_manager.get_entry_reference(target_uuid)->set_parent_uuid(service_persistent_uuid);
        stabilize_target(target_uuid);
    }

    return service_persistent_uuid;
}


const std::string StorageTreeStabilizer::stabilize(const std::string& module_uuid) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto& service_manager = StorageComponents::get_instance()->get_storage_service_manager();
    const auto& services_keys = service_manager.get_keys(module_uuid);

    if (services_keys.empty()) {
        throw std::runtime_error("No Storage Service found for module " + module_uuid);
    }

    const auto service = service_manager.get_entry(services_keys.front());
    const auto module_persistent_uuid = stabilize_single_resource(module_uuid, module_manager,
                                                                  literals::MODULE_KEY_BASE);

    service_manager.get_entry_reference(service.get_uuid())->set_parent_uuid(module_persistent_uuid);
    stabilize_storage_service(service.get_uuid());

    return module_persistent_uuid;
}
