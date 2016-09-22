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
 * @file HotswapManager.cpp
 *
 * @brief ...
 * */
#include "hotswap/hotswap_manager.hpp"
#include "hotswap/hotswap_event.hpp"
#include "sysfs/sysfs_api.hpp"
#include "lvm/lvm_api.hpp"
#include "lvm/lvm_attribute.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"

#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/module/hard_drive_partition.hpp"
#include "agent-framework/module/target.hpp"
#include "agent-framework/module/logical_drive.hpp"
#include "agent-framework/module/storage_controller.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

#include <mutex>
#include <condition_variable>
#include <algorithm>

using namespace agent::storage::hotswap_discovery;
using namespace agent::storage::iscsi;
using namespace agent::storage::lvm;

using SysfsAPI = agent::storage::sysfs::SysfsAPI;
using HardDrive = agent_framework::generic::HardDrive;
using HardDrivePartition = agent_framework::generic::HardDrivePartition;
using FruInfo = agent_framework::generic::FruInfo;
using Target = agent_framework::generic::Target;

bool HotswapManager::compare_disks(const SysfsAPI::HardDrive& hd_detected, HardDriveSharedPtr hd_model) {
    return ((hd_detected.get_serial_number() == hd_model->get_fru_info().get_serial_number()) &&
            (hd_detected.get_model() == hd_model->get_fru_info().get_model_number()) &&
            (hd_detected.get_device_path() == hd_model->get_device_path()));
}

void HotswapManager::fill_disk_parameters(HardDriveSharedPtr& hard_drive, SysfsAPI::HardDrive& new_drive) {
    hard_drive->set_name(new_drive.get_name());
    hard_drive->set_device_path(new_drive.get_device_path());
    hard_drive->set_capacity_gb(new_drive.get_capacity_gb());
    hard_drive->set_type(new_drive.get_type());
    hard_drive->set_interface(new_drive.get_interface());
    hard_drive->set_rpm(new_drive.get_rpm());
    FruInfo fru_info;
    fru_info.set_manufacturer(new_drive.get_manufacturer());
    fru_info.set_model_number(new_drive.get_model());
    fru_info.set_serial_number(new_drive.get_serial_number());
    hard_drive->set_fru_info(fru_info);
    hard_drive->set_status({lvm::attribute::STATE_ENABLED, lvm::attribute::HEALTH_OK_STATUS});
}

void HotswapManager::add_disk(agent_framework::generic::StorageController* storage_controller,
        SysfsAPI::HardDrive& new_drive, const SubmoduleUniquePtr& submodule) {
    auto hard_drive = std::make_shared<HardDrive>();
    fill_disk_parameters(hard_drive, new_drive);
    log_debug(GET_LOGGER("storage-agent"), "Added disk path: " << hard_drive->get_device_path());
    log_debug(GET_LOGGER("storage-agent"), "Added serial number: " << hard_drive->get_fru_info().get_serial_number());
    (*storage_controller).add_hard_drive(std::move(hard_drive));

    agent::storage::hotswap_event::send_event(hard_drive->get_uuid(), ::agent_framework::model::enums::Component::PhysicalDrive,
            ::agent_framework::eventing::Notification::Add, submodule->get_name());
}

void HotswapManager::remove_disk(agent_framework::generic::StorageController* storage_controller,
        HardDriveSharedPtr& hd_removed, const SubmoduleUniquePtr& submodule) {

    agent::storage::hotswap_event::send_event((*hd_removed).get_uuid(), ::agent_framework::model::enums::Component::PhysicalDrive,
        ::agent_framework::eventing::Notification::Remove, submodule->get_name());

    log_debug(GET_LOGGER("storage-agent"), "Removed disk path: " << (*hd_removed).get_device_path());
    log_debug(GET_LOGGER("storage-agent"), "Removed serial number: " << (*hd_removed).get_fru_info().get_serial_number());

    resolve_dependencies(hd_removed);

    storage_controller->remove_hard_drive(hd_removed);

}

bool HotswapManager::check_physical_volumes_state(const SubmoduleUniquePtr& submodule, HardDriveSharedPtr& hd_removed) {
    bool pv_found = false;
    for (const auto& ld : submodule->get_logical_drives()) {
        const string& logical_drive_mode = ld->get_mode();
        if ((0 == logical_drive_mode.compare(agent_framework::generic::LogicalDrive::LvmTypes::PHYSICAL_VOLUME))
                            && (ld->get_device_path() == (*hd_removed).get_device_path())) {
            if (ld->get_status().get_state() == lvm::attribute::STATE_STANDBY_OFFLINE &&
                    ld->get_status().get_health() == lvm::attribute::HEALTH_CRITICAL_STATUS) {
                break;
            }
            ld->set_status({lvm::attribute::STATE_STANDBY_OFFLINE, lvm::attribute::HEALTH_CRITICAL_STATUS});
            ld->remove_hard_drive(hd_removed);
            log_debug(GET_LOGGER("storage-agent"), "Physical Volume in Critical and StandbyOffline state: " << ld->get_name());
            agent::storage::hotswap_event::send_event(ld->get_uuid(), ::agent_framework::model::enums::Component::LogicalDrive,
                        ::agent_framework::eventing::Notification::Update, submodule->get_name());
            pv_found = true;
            break;
        }
    }
    return pv_found;
}

void HotswapManager::check_volume_groups_state(const SubmoduleUniquePtr& submodule) {
    for (const auto& ld : submodule->get_logical_drives()) {
        const string& logical_drive_mode = ld->get_mode();
        if ((0 == logical_drive_mode.compare(agent_framework::generic::LogicalDrive::LvmTypes::VOLUME_GROUP))) {
            if (ld->get_status().get_health() == lvm::attribute::HEALTH_CRITICAL_STATUS) {
                continue;
            }
            ld->set_status({ld->get_status().get_state(), lvm::attribute::HEALTH_CRITICAL_STATUS});
            log_debug(GET_LOGGER("storage-agent"), "Volume Group in Critical state: " << ld->get_name());
            agent::storage::hotswap_event::send_event(ld->get_uuid(), ::agent_framework::model::enums::Component::LogicalDrive,
                            ::agent_framework::eventing::Notification::Update, submodule->get_name());
        }
    }
}

void HotswapManager::check_logical_volumes_state(const SubmoduleUniquePtr& submodule) {
    for (const auto& ld : submodule->get_logical_drives()) {
        const string& logical_drive_mode = ld->get_mode();
        if ((0 == logical_drive_mode.compare(agent_framework::generic::LogicalDrive::LvmTypes::LOGICAL_VOLUME))) {
            if (ld->get_status().get_health() == lvm::attribute::HEALTH_CRITICAL_STATUS) {
                continue;
            }
            check_targets_state(submodule, ld);
            ld->set_status({ld->get_status().get_state(), lvm::attribute::HEALTH_CRITICAL_STATUS});
            log_debug(GET_LOGGER("storage-agent"), "Logical Volume in Critical state: " << ld->get_name());
            agent::storage::hotswap_event::send_event(ld->get_uuid(), ::agent_framework::model::enums::Component::LogicalDrive,
                            ::agent_framework::eventing::Notification::Update, submodule->get_name());
        }
    }
}

void HotswapManager::check_targets_state(const SubmoduleUniquePtr& submodule, const agent_framework::generic::LogicalDriveSharedPtr& ld) {
    auto& target_manager = submodule->get_target_manager();
    auto& targets = target_manager.get_targets();
    for (auto& target : targets) {
        auto& target_lun_vec = target->get_target_lun();
        for (auto& target_lun : target_lun_vec) {
            if (target_lun.get_device_path() == ld->get_device_path()) {
                log_debug(GET_LOGGER("storage-agent"), "iSCSI Target in Critical state: " << target->get_target_iqn());
                target->set_status({target->get_status().get_state(), lvm::attribute::HEALTH_CRITICAL_STATUS});
                agent::storage::hotswap_event::send_event(target->get_uuid(), ::agent_framework::model::enums::Component::IscsiTarget,
                                        ::agent_framework::eventing::Notification::Update, submodule->get_name());
            }
        }
    }
}

void HotswapManager::resolve_dependencies(HardDriveSharedPtr& hd_removed) {
    for (const auto& module : ModuleManager::get_modules()) {
        if (!module->get_submodules().size()) {
            log_error(GET_LOGGER("storage-agent"), "Submodules empty!");
            continue;
        }

        auto& submodule = module->get_submodules().front();
        bool pv_found = check_physical_volumes_state(submodule, hd_removed);
        if (pv_found) {
            check_volume_groups_state(submodule);
            check_logical_volumes_state(submodule);
        }
    }
}

void HotswapManager::hotswap_discover_hard_drives() {

    auto sc = ModuleManager::get_storage_controllers();
    auto& storage_submodule = ModuleManager::get_modules().front()->get_submodules().front();

    std::vector<SysfsAPI::HardDrive> hds_detected;
    SysfsAPI* sysfs = SysfsAPI::get_instance();

    sysfs->get_hard_drives(hds_detected);

    for (auto storage_controller = sc.begin(); storage_controller != sc.end(); storage_controller++) {

        auto hds_in_model = (*storage_controller)->get_hard_drives();
        auto hds_removed = (*storage_controller)->get_hard_drives();
        std::vector<SysfsAPI::HardDrive> hds_added(hds_detected.begin(), hds_detected.end());
        log_debug(GET_LOGGER("storage-agent"), "Disks in model: " << hds_in_model.size());
        log_debug(GET_LOGGER("storage-agent"), "Disks detected: " << hds_detected.size());
        for (auto hd_model = hds_in_model.begin(); hd_model != hds_in_model.end(); hd_model++) {
            for (auto& hd_detected : hds_detected) {
                if (HotswapManager::compare_disks(hd_detected, *hd_model)) {
                    hds_removed.erase(std::remove(hds_removed.begin(), hds_removed.end(), (*hd_model)), hds_removed.end());
                    hds_added.erase(std::remove(hds_added.begin(), hds_added.end(), hd_detected));
                    break;
                }
            }
        }

        log_debug(GET_LOGGER("storage-agent"), "New disks: " << hds_added.size());
        for (auto& new_drive : hds_added) {
            add_disk(*storage_controller, new_drive, storage_submodule);
        }

        log_debug(GET_LOGGER("storage-agent"), "Removed disks: " << hds_removed.size());
        for (auto& hd_removed : hds_removed) {
            remove_disk(*storage_controller, hd_removed, storage_submodule);

        }
    }
}
