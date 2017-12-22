/*!
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
 * @file storage/src/discovery/discovery_manager.cpp
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/logger_ext.hpp"

#include "discovery/discovery_manager.hpp"
#include "tree_stability/storage_tree_stabilizer.hpp"
#include "sysfs/sysfs_api.hpp"
#include "lvm/lvm_api.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"



using std::map;
using std::string;
using std::vector;
using std::runtime_error;
using std::to_string;
using std::unique_lock;

using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::module::managers;

using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace agent::storage::lvm;
using namespace agent::storage::iscsi;


void DiscoveryManager::wait_for_complete() {
    unique_lock<std::mutex> lock{m_mutex};
    m_cv.wait(lock);
}


string DiscoveryManager::get_logical_drive_uuid(const string& device_path) const {
    auto uuids = get_manager<LogicalDrive>().get_keys();
    for (const auto& uuid : uuids) {
        auto logical_drive = get_manager<LogicalDrive>().get_entry(uuid);
        if (device_path == logical_drive.get_device_path()) {
            return logical_drive.get_uuid();
        }
    }
    return {};
}


string DiscoveryManager::get_physical_drive_uuid(const string& device_path) const {
    auto uuids = get_manager<PhysicalDrive>().get_keys();
    for (const auto& uuid : uuids) {
        auto phy_drive = get_manager<PhysicalDrive>().get_entry(uuid);
        if (device_path == phy_drive.get_device_path()) {
            return phy_drive.get_uuid();
        }
    }
    return {};
}


void DiscoveryManager::discovery_iscsi_targets(const string& uuid) const {
    tgt::Manager manager{};
    auto response = manager.show_targets();
    if (!response.is_valid()) {
        throw runtime_error("Invalid ISCSI show target response: " +
                            tgt::Errors::get_error_message(response.get_error()));
    }
    tgt::TargetParser parser{};
    std::string iscsi_text(response.get_extra_data().data());
    if (response.get_extra_data().size()
        && (iscsi_text.size() + 1) < response.get_extra_data().size()) {
        log_warning(GET_LOGGER("agent"),
                "show targets response size: " << response.get_extra_data().size()
                << " truncated to: " << (iscsi_text.size() + 1));
    }
    const auto targets = parser.parse(iscsi_text);
    auto& target_manager = get_manager<IscsiTarget, IscsiTargetManager>();
    for (const auto& target : targets) {
        IscsiTarget iscsi_target{uuid};
        const auto& iscsi_data = target_manager.get_iscsi_data();
        iscsi_target.set_status({State::Enabled, Health::OK});
        iscsi_target.set_initiator_iqn(
            target->get_target_initiator().empty() ?
            OptionalField<std::string>() :
            OptionalField<std::string>(target->get_target_initiator())
        );
        iscsi_target.set_target_address(iscsi_data.get_portal_ip());
        iscsi_target.set_target_port(to_string(iscsi_data.get_portal_port()));
        iscsi_target.set_target_iqn(target->get_target_iqn());
        iscsi_target.set_target_id(target->get_target_id());
        if (target->get_authentication_method().has_value()) {
            iscsi_target.set_authentication_method(target->get_authentication_method().value().to_string());
        }
        iscsi_target.set_chap_username(
                target->get_chap_username().empty() ?
                OptionalField<std::string>() :
                OptionalField<std::string>(target->get_chap_username())
        );
        iscsi_target.set_mutual_chap_username(
                target->get_mutual_chap_username().empty() ?
                OptionalField<std::string>() :
                OptionalField<std::string>(target->get_mutual_chap_username())
        );
        /* add target LUNs */
        for (const auto& lun : target->get_luns()) {
            auto drive_uuid = get_logical_drive_uuid(lun->get_device_path());
            TargetLun target_lun{uint32_t(lun->get_lun()), drive_uuid};
            iscsi_target.add_target_lun(target_lun);
        }
        /* add target id for ISCSI model */
        target_manager.add_target_id(
            uint32_t(target->get_target_id()), iscsi_target.get_uuid());
        /* add ISCSI model to the manager */
        target_manager.add_entry(iscsi_target);
    }
}


void DiscoveryManager::discovery_logical_drives(const string& uuid) const {
    LvmAPI lvm_api{};
    vector<VolumeGroup> volume_groups{lvm_api.discovery_volume_groups()};
    for (const auto& volume_group : volume_groups) {
        map<string, string> name_to_uuid_map{};
        map<string, string> uuid_to_master_map{};
        /* create logical volume group */
        LogicalDrive logical_vg{uuid};
        init_logical_volume(logical_vg);
        logical_vg.set_mode(LogicalDriveMode::LVG);
        logical_vg.set_status({State::from_string(volume_group.get_status()),
                               Health::from_string(volume_group.get_health())});
        logical_vg.set_capacity_gb(volume_group.get_capacity_gb());
        logical_vg.set_device_path("/dev/" + volume_group.get_name());
        logical_vg.set_protected(volume_group.get_protection_status());
        logical_vg.add_collection({
                                      CollectionName::LogicalDrives, CollectionType::LogicalDrives, ""});
        for (const auto& physical_volume : volume_group.get_physical_volumes()) {
            /* create physical volume */
            LogicalDrive logical_pv{logical_vg.get_uuid()};
            init_logical_volume(logical_pv);
            logical_pv.set_status({State::from_string(physical_volume.get_status()),
                                   Health::from_string(physical_volume.get_health())});
            logical_pv.set_mode(LogicalDriveMode::PV);
            logical_pv.set_capacity_gb(physical_volume.get_capacity_gb());
            logical_pv.set_device_path(physical_volume.get_name());
            logical_pv.set_protected(physical_volume.get_protection_status());
            /* set physical drive parent UUID same as physical volume UUID */
            try {
                auto phy_drive = get_manager<PhysicalDrive>().get_entry_reference(
                    get_physical_drive_uuid(logical_pv.get_device_path()));
                phy_drive->set_parent_uuid(logical_pv.get_uuid());
            }
            catch (const agent_framework::exceptions::InvalidUuid&) {
                log_warning(GET_LOGGER("storage-agent"),
                            "No physical drive found for logical volume " << logical_pv.get_uuid());
            }

            /* add physical volume */
            get_manager<LogicalDrive>().add_entry(std::move(logical_pv));
        }
        for (const auto& logical_volume : volume_group.get_logical_volumes()) {
            /* create logical volume */
            LogicalDrive logical_lv{logical_vg.get_uuid()};
            logical_lv.set_status({State::from_string(logical_volume.get_status()),
                                   Health::from_string(logical_volume.get_health())});
            logical_lv.set_type(LogicalDriveType::LVM);
            logical_lv.set_mode(LogicalDriveMode::LV);
            logical_lv.set_bootable(logical_volume.is_bootable());
            logical_lv.set_is_snapshot(logical_volume.is_snapshot());
            logical_lv.set_capacity_gb(logical_volume.get_capacity_gb());
            logical_lv.set_device_path("/dev/" + volume_group.get_name()
                                       + "/" + logical_volume.get_name());
            logical_lv.set_protected(logical_volume.get_protection_status());
            /* add entry to the map [ LV name <> LV UUID] */
            name_to_uuid_map[logical_volume.get_name()] = logical_lv.get_uuid();
            /* if LV is snapshot, store the master [LV UUID <> LV master] */
            if (logical_volume.is_snapshot()) {
                uuid_to_master_map[logical_lv.get_uuid()] =
                    logical_volume.get_master_name();
            }
            /* add logical volume */
            get_manager<LogicalDrive>().add_entry(std::move(logical_lv));
        }
        /* resolve LV master UUID for each snapshot logical volume */
        for (const auto& element : uuid_to_master_map) {
            const auto& lv_uuid = element.first;
            const auto& lv_master_name = element.second;
            auto lv = get_manager<LogicalDrive>().get_entry_reference(lv_uuid);
            lv->set_master(name_to_uuid_map[lv_master_name]);
        }
        /* add volume group */
        get_manager<LogicalDrive>().add_entry(std::move(logical_vg));
    }
}


void DiscoveryManager::init_logical_volume(LogicalDrive& logical_volume) const {
    logical_volume.add_collection({
                                      CollectionName::PhysicalDrives, CollectionType::PhysicalDrives, ""});
    logical_volume.set_type(LogicalDriveType::LVM);
    logical_volume.set_bootable(false);
}


void DiscoveryManager::discovery_physical_drives(const string& uuid) const {
    /* get hard drive list */
    vector<SysfsAPI::HardDrive> bd_drives = SysfsAPI::get_instance()->get_hard_drives();
    for (auto& bd_drive : bd_drives) {
        /* create physical drive model */
        PhysicalDrive physical_drive{uuid};
        physical_drive.set_status({State::Enabled, Health::OK});
        physical_drive.set_device_path(bd_drive.get_device_path());
        physical_drive.set_capacity_gb(bd_drive.get_capacity_gb());
        auto drive_type = PhysicalDriveType::from_string(bd_drive.get_type());
        physical_drive.set_type(drive_type);
        PhysicalDriveInterface interface{PhysicalDriveInterface::SATA};
        if (!bd_drive.get_interface().empty()) {
            interface = PhysicalDriveInterface::
            from_string(bd_drive.get_interface());
        }
        physical_drive.set_interface(interface);

        if (PhysicalDriveType::HDD == drive_type) {
            physical_drive.set_rpm(bd_drive.get_rpm());
        }
        physical_drive.set_fru_info({bd_drive.get_serial_number(),
                                     bd_drive.get_manufacturer(),
                                     bd_drive.get_model(), ""});
        /* add new physical drive */
        get_manager<PhysicalDrive>().add_entry(std::move(physical_drive));
    }
}


void DiscoveryManager::discovery(const string& uuid) {
    /* get storage services UUID */
    auto storage_uuids = get_manager<StorageService>().get_keys(uuid);
    if (storage_uuids.empty()) {
        throw runtime_error("Storage service not found");
    }
    /* discovery devices using first storage service UUID */
    discovery_physical_drives(storage_uuids.front());
    discovery_logical_drives(storage_uuids.front());
    discovery_iscsi_targets(storage_uuids.front());

    StorageTreeStabilizer().stabilize(uuid);

    /* notify discovery done */
    m_cv.notify_all();
}


DiscoveryManager::~DiscoveryManager() { }
