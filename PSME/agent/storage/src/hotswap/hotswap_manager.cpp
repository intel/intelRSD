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
 *
 * @file HotswapManager.cpp
 *
 * @brief Implementation of Hotswap manager
 * */
#include "hotswap/hotswap_manager.hpp"
#include "event/storage_event.hpp"
#include "lvm/lvm_api.hpp"
#include "lvm/lvm_attribute.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"
#include "tree_stability/storage_tree_stabilizer.hpp"

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"



using std::vector;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent::storage;
using namespace agent::storage::hotswap_discovery;
using namespace agent::storage::event;


bool HotswapManager::compare_disks(const HardDrive& hd_detected,
                                   const PhysicalDrive& phy_drive) {
    const auto& fru_info = phy_drive.get_fru_info();
    return ((hd_detected.get_serial_number() == fru_info.get_serial_number()) &&
            (hd_detected.get_model() == fru_info.get_model_number()) &&
            (hd_detected.get_device_path() == phy_drive.get_device_path()));
}


void HotswapManager::add_disk(const HardDrive& new_hdrive) {
    PhysicalDrive phy_drive{};
    /* fill physical drive parameters using sysfs drive info */
    phy_drive.set_device_path(new_hdrive.get_device_path());
    phy_drive.set_capacity_gb(new_hdrive.get_capacity_gb());
    phy_drive.set_type(PhysicalDriveType::from_string(new_hdrive.get_type()));
    PhysicalDriveInterface interface{PhysicalDriveInterface::SATA};
    if (!new_hdrive.get_interface().empty()) {
        interface = PhysicalDriveInterface::from_string(
            new_hdrive.get_interface());
    }
    phy_drive.set_interface(interface);
    phy_drive.set_rpm(new_hdrive.get_rpm());
    phy_drive.set_fru_info({new_hdrive.get_serial_number(),
                            new_hdrive.get_manufacturer(),
                            new_hdrive.get_model(), ""});
    phy_drive.set_status({State::Enabled, Health::OK});
    /* add new physical drive to the storage manager */
    get_manager<PhysicalDrive>().add_entry(phy_drive);
    /* Stabilize added drive */
    const std::string phy_drive_persistent_uuid =
        StorageTreeStabilizer().stabilize_physical_drive(phy_drive.get_uuid());
    /* send event notification about adding new physical drive */
    auto storage_uuid = get_manager<StorageService>().get_keys().front();
    send_event(phy_drive_persistent_uuid, Component::PhysicalDrive,
               agent_framework::eventing::Notification::Add, storage_uuid);
    /* log about success adding */
    log_debug(GET_LOGGER("storage-agent"), "Added disk path: "
                                           << phy_drive.get_device_path());
    log_debug(GET_LOGGER("storage-agent"), "Added serial number: "
                                           << phy_drive.get_fru_info().get_serial_number());
}


void HotswapManager::remove_disk(const PhysicalDrive& phy_drive) {
    auto storage_uuid = get_manager<StorageService>().get_keys().front();
    /* send event notification about removing physical drive */
    send_event(phy_drive.get_uuid(), Component::PhysicalDrive,
               agent_framework::eventing::Notification::Remove, storage_uuid);
    /* resolve dependencies */
    resolve_dependencies(phy_drive);
    /* remove physical srive from storage manager */
    get_manager<PhysicalDrive>().remove_entry(phy_drive.get_uuid());
    /* log about success removing */
    log_debug(GET_LOGGER("storage-agent"), "Removed disk path: "
                                           << phy_drive.get_device_path());
    log_debug(GET_LOGGER("storage-agent"), "Removed serial number: "
                                           << phy_drive.get_fru_info().get_serial_number());
}


bool HotswapManager::check_physical_volumes_state(const PhysicalDrive& phy_drive,
                                                  const string& storage_uuid) {
    for (const auto& uuid : get_manager<LogicalDrive>().get_keys()) {
        auto logical_drive = get_manager<LogicalDrive>().get_entry_reference(uuid);
        if (LogicalDriveMode::PV == logical_drive->get_mode() &&
            logical_drive->get_device_path() == phy_drive.get_device_path()) {
            /* check PV status */
            if ((Health::Critical == logical_drive->get_status().get_health()) &&
                (State::UnavailableOffline == logical_drive->get_status().get_state())) {
                break;
            }
            /* change status of logical drive to critical */
            logical_drive->set_status({State::UnavailableOffline, Health::Critical});
            /* remove physical drive from manager */
            get_manager<PhysicalDrive>().remove_entry(phy_drive.get_uuid());
            log_debug(GET_LOGGER("storage-agent"),
                      "Critical PV health and Offline state: "
                      << logical_drive->get_device_path());
            /* send event about physical drive status change */
            send_event(logical_drive->get_uuid(), Component::LogicalDrive,
                       agent_framework::eventing::Notification::Update, storage_uuid);
            return true;
        }
    }
    return false;
}


void HotswapManager::check_volume_groups_state(const string& storage_uuid) {
    for (const auto& uuid : get_manager<LogicalDrive>().get_keys()) {
        auto logical_drive = get_manager<LogicalDrive>().get_entry_reference(uuid);
        if (LogicalDriveMode::LVG == logical_drive->get_mode()) {
            /* check the VG status */
            const auto& status = logical_drive->get_status();
            if (Health::Critical == status.get_health()) {
                continue;
            }
            /* change status of logical drive to critical */
            logical_drive->set_status({status.get_state(), Health::Critical});
            log_debug(GET_LOGGER("storage-agent"), "Critical VG state: "
                                                   << logical_drive->get_device_path());
            /* send event about volume group status change */
            send_event(logical_drive->get_uuid(), Component::LogicalDrive,
                       agent_framework::eventing::Notification::Update, storage_uuid);
        }
    }
}


void HotswapManager::check_logical_volumes_state(const string& storage_uuid) {
    for (const auto& uuid : get_manager<LogicalDrive>().get_keys()) {
        auto logical_drive = get_manager<LogicalDrive>().get_entry_reference(uuid);
        if (LogicalDriveMode::LV == logical_drive->get_mode()) {
            /* check the VG status */
            const auto& status = logical_drive->get_status();
            if (Health::Critical == status.get_health()) {
                continue;
            }
            /* check iSCSI target state */
            check_targets_state(*logical_drive, storage_uuid);
            /* change status of logical drive to critical */
            logical_drive->set_status({status.get_state(), Health::Critical});
            log_debug(GET_LOGGER("storage-agent"), "Critical LV state: "
                                                   << logical_drive->get_device_path());
            /* send event about logical volume status change */
            send_event(logical_drive->get_uuid(), Component::LogicalDrive,
                       agent_framework::eventing::Notification::Update, storage_uuid);
        }
    }
}


void HotswapManager::check_targets_state(const LogicalDrive& ldrive,
                                         const string& storage_uuid) {
    for (const auto& uuid : get_manager<IscsiTarget>().get_keys()) {
        auto target = get_manager<IscsiTarget>().get_entry_reference(uuid);
        for (const auto& lun : target->get_target_lun()) {
            if (lun.get_logical_drive() == ldrive.get_uuid()) {
                log_debug(GET_LOGGER("storage-agent"), "Critical iSCSI target state: "
                                                       << target->get_target_iqn());
                /* change status of iSCSI target to critical */
                target->set_status({target->get_status().get_state(), Health::Critical});
                /* send event about iSCSI target status change */
                send_event(target->get_uuid(), Component::IscsiTarget,
                           agent_framework::eventing::Notification::Update, storage_uuid);
            }
        }
    }
}


void HotswapManager::resolve_dependencies(const PhysicalDrive& phy_drive) {
    auto& module = CommonComponents().get_instance()->get_module_manager();
    for (const auto& manager_uuid : module.get_keys()) {
        for (const auto& storage_uuid :
            get_manager<StorageService>().get_keys(manager_uuid)) {
            if (check_physical_volumes_state(phy_drive, storage_uuid)) {
                check_volume_groups_state(storage_uuid);
                check_logical_volumes_state(storage_uuid);
            }
        }
    }
}


void HotswapManager::hotswap_discover_hard_drives() {
    /* get list of hard drives on a system */
    vector<HardDrive> hds_detected = SysfsAPI::get_instance()->get_hard_drives();
    vector<HardDrive> hard_drives(hds_detected.begin(), hds_detected.end());
    auto phy_drive_uuids = get_manager<PhysicalDrive>().get_keys();
    auto phy_drive_end_it = phy_drive_uuids.end();
    auto hdrive_end_it = hard_drives.end();
    /* find new/removed drives */
    for (const auto& uuid : get_manager<PhysicalDrive>().get_keys()) {
        auto phy_drive = get_manager<PhysicalDrive>().get_entry(uuid);
        for (const auto& hd_detected : hds_detected) {
            if (compare_disks(hd_detected, phy_drive)) {
                phy_drive_end_it = std::remove(phy_drive_uuids.begin(), phy_drive_end_it, uuid);
                hdrive_end_it = std::remove(hard_drives.begin(), hdrive_end_it, hd_detected);
            }
        }
    }
    /* if phy_drive_uuids list isn't empty then some physical
     * drives have been removed from the system. Remove physical drives
     * from the storage manager */
    for (auto it = phy_drive_uuids.begin(); it != phy_drive_end_it; ++it) {
        log_debug(GET_LOGGER("storage-agent"), "Remove disk: " << *it);
        auto phy_drive = get_manager<PhysicalDrive>().get_entry(*it);
        remove_disk(phy_drive);
    }
    /* if hard_drives list is not empty, it means new hard(physical) drives
     * have been attached/added. Add these new drives to the storage manager */
    for (auto it = hard_drives.begin(); it != hdrive_end_it; ++it) {
        log_debug(GET_LOGGER("storage-agent"), "Add new disk: " << (*it).get_device_path());
        add_disk(*it);
    }
}
