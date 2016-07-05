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
 *
 * @file iscsi_target_dependency_resolver.cpp
 *
 * @brief ...
 * */

#include "discovery/dependency_resolver/iscsi_target_dependency_resolver.hpp"

using namespace agent::storage::discovery;

using LogicalDrive = agent_framework::generic::LogicalDrive;

void IscsiTargetDependencyResolver::resolve() {
    auto& submodule = m_module.get_submodules().front();
    auto& target_manager = submodule->get_target_manager();
    auto& targets = target_manager.get_targets();
    for (auto& target : targets) {
        resolve_iscsi_target(target);
    }
}

void IscsiTargetDependencyResolver::resolve_iscsi_target(Target::TargetSharedPtr target) {
    auto& target_lun_vec = target->get_target_lun();
    for (auto& target_lun : target_lun_vec) {
        auto& target_device_path = target_lun.get_device_path();
        auto found = m_logical_drive_map.find(target_device_path);
        if (found != m_logical_drive_map.end()) {
            target_lun.set_logical_drive_uuid(found->second->get_uuid());
            target->add_logical_drive(found->second);
        }
    }
}

void IscsiTargetDependencyResolver::initialize() {
    auto& submodule = m_module.get_submodules().front();
    auto& volume_groups = submodule->get_logical_drives();
    for (auto& volume_group : volume_groups) {
        for (auto& logical_drive : volume_group->get_logical_drives()) {
            const std::string device_path = logical_drive->get_device_path();
            m_logical_drive_map[device_path] = logical_drive;
        }
    }
}
