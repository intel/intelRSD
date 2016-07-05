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
 * @file DiscoveryManager.cpp
 *
 * @brief ...
 * */
#include "discovery/discovery_manager.hpp"
#include "discovery/dependency_resolver/storage_dependency_resolver.hpp"
#include "sysfs/sysfs_api.hpp"
#include "lvm/lvm_api.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"

#include "agent-framework/module/hard_drive.hpp"
#include "agent-framework/module/hard_drive_partition.hpp"
#include "agent-framework/module/target.hpp"
#include "agent-framework/module/logical_drive.hpp"

#include <mutex>
#include <condition_variable>

using namespace agent::storage::discovery;
using namespace agent::storage::iscsi;
using namespace agent::storage::lvm;

using SysfsAPI = agent::storage::sysfs::SysfsAPI;
using HardDrive = agent_framework::generic::HardDrive;
using HardDrivePartition = agent_framework::generic::HardDrivePartition;
using FruInfo = agent_framework::generic::FruInfo;
using LogicalDrive = agent_framework::generic::LogicalDrive;
using Target = agent_framework::generic::Target;

struct DiscoveryManager::DiscoveryComplete {
    void notify_discovery_complete() {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_discovery_done = true;
        m_cv.notify_all();
        log_info(GET_LOGGER("storage"), " Discovery complete!");
    }

    void wait_for_discovery_complete() {
        log_info(GET_LOGGER("storage"), " Waiting for discovery to complete...");
        std::unique_lock<std::mutex> lock{m_mutex};
        m_cv.wait(lock, [this] { return m_discovery_done; });
    }
private:
    std::mutex m_mutex{};
    std::condition_variable m_cv{};
    volatile bool m_discovery_done{false};
};

DiscoveryManager::DiscoveryManager()
    : m_discovery_complete{new DiscoveryComplete()} {}

DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::discover(Module& module) const {
    discovery_hard_drives(module);
    discovery_logical_drives(module);
    discovery_iscsi_targets(module);
    resolve_dependencies(module);
    m_discovery_complete->notify_discovery_complete();
}

void DiscoveryManager::wait_for_discovery_complete() const {
    m_discovery_complete->wait_for_discovery_complete();
}

void DiscoveryManager::resolve_dependencies(Module& module) const {
    StorageDependencyResolver storageDependencyResolver(module);
    storageDependencyResolver.resolve();
}

void DiscoveryManager::discovery_hard_drives(Module& module) const {
    auto& submodule = module.get_submodules().front();
    auto& storage_controller = submodule->get_storage_controllers().front();
    std::vector<SysfsAPI::HardDrive> bd_drives;
    SysfsAPI* sysfs = SysfsAPI::get_instance();

    sysfs->get_hard_drives(bd_drives);

    for (auto& bd_drive : bd_drives) {

        auto hard_drive = std::make_shared<HardDrive>();

        hard_drive->set_name(bd_drive.get_name());
        hard_drive->set_device_path(bd_drive.get_device_path());
        hard_drive->set_capacity_gb(bd_drive.get_capacity_gb());
        hard_drive->set_type(bd_drive.get_type());
        hard_drive->set_interface(bd_drive.get_interface());
        hard_drive->set_rpm(bd_drive.get_rpm());

        FruInfo fru_info;
        fru_info.set_manufacturer(bd_drive.get_manufacturer());
        fru_info.set_model_number(bd_drive.get_model());
        fru_info.set_serial_number(bd_drive.get_serial_number());
        hard_drive->set_fru_info(fru_info);

        hard_drive->set_status({"Enabled", "OK"});

        storage_controller->add_hard_drive(std::move(hard_drive));
    }
}

void DiscoveryManager::discovery_iscsi_targets(Module& module) const {
    using namespace agent::storage::iscsi::tgt;

    if (!module.get_submodules().size()) {
        log_error(GET_LOGGER("storage"), "Submodules empty!");
        return;
    }
    auto& submodule = module.get_submodules().front();
    const auto& iscsi_data = submodule->get_iscsi_data();

    Manager manager;
    auto response = manager.show_targets();
    if (!response.is_valid()) {
        log_error(GET_LOGGER("storage"),
                "ISCSI show target invalid response: " +
                Errors::get_error_message(response.get_error()));
        return;
    }

    auto& extra_data = response.get_extra_data();
    TargetParser parser{};
    std::string iscsi_text(extra_data.cbegin(), extra_data.cend());
    const auto targets = parser.parse(iscsi_text);
    for (const auto& target : targets) {
        auto target_data = Target::make_target();
        target_data->set_target_id(target->get_target_id());
        target_data->set_target_iqn(target->get_target_iqn());
        target_data->set_target_address(iscsi_data.get_portal_ip());
        target_data->set_target_port(iscsi_data.get_portal_port());
        target_data->set_status({"Enabled", "OK"});
        target_data->set_initiator_iqn(target->get_target_initiator());
        for (const auto& lun : target->get_luns()) {
            Target::Lun lun_obj;
            lun_obj.set_lun_id(lun->get_lun());
            lun_obj.set_device_path(lun->get_device_path());
            target_data->add_target_lun(std::move(lun_obj));
        }

        log_debug(GET_LOGGER("storage"),
                "Add ISCSI Target: " << target_data->get_uuid());
        submodule->get_target_manager().add_target(std::move(target_data));
    }
}

void DiscoveryManager::discovery_logical_drives(Module& module) const {
    if (!module.get_submodules().size()) {
        log_error(GET_LOGGER("storage"), "Submodules empty!");
        return;
    }
    auto& submodule = module.get_submodules().front();

    LvmAPI lvm_api;

    static constexpr const char lvm_type_name[] = "LVM";

    std::vector<VolumeGroup> volume_groups =
        lvm_api.discovery_volume_groups();

    for(const auto& volume_group : volume_groups) {
        std::map<std::string, std::string> name_to_uuid_map{};
        std::map<std::string, std::string> name_to_master_map{};

        auto logical_drive = LogicalDrive::make_logical_drive();

        logical_drive->set_bootable(false);
        logical_drive->set_name(volume_group.get_name());
        logical_drive->set_mode(LogicalDrive::LvmTypes::VOLUME_GROUP);
        logical_drive->set_type(lvm_type_name);
        logical_drive->set_capacity_gb(volume_group.get_capacity_gb());
        logical_drive->set_device_path("/dev/" + volume_group.get_name());
        logical_drive->set_protected(volume_group.get_protection_status());
        logical_drive->set_status({volume_group.get_status(), volume_group.get_health()});

        for (const auto& physical_volume : volume_group.get_physical_volumes()) {
            auto physical_drive = LogicalDrive::make_logical_drive();

            physical_drive->set_bootable(false);
            physical_drive->set_name(physical_volume.get_name());
            physical_drive->set_capacity_gb(physical_volume.get_capacity_gb());
            physical_drive->set_type(lvm_type_name);
            physical_drive->set_protected(physical_volume.get_protection_status());
            physical_drive->set_device_path(physical_volume.get_name());
            physical_drive->set_mode(LogicalDrive::LvmTypes::PHYSICAL_VOLUME);
            physical_drive->set_status({physical_volume.get_status(), physical_volume.get_health()});

            logical_drive->add_logical_drive(physical_drive);
        }

        for (const auto& logical_volume : volume_group.get_logical_volumes()) {
            auto logical_drive_child = LogicalDrive::make_logical_drive();

            logical_drive_child->set_bootable(logical_volume.is_bootable());
            logical_drive_child->set_name(logical_volume.get_name());
            logical_drive_child->set_protected(logical_volume.get_protection_status());
            logical_drive_child->set_snapshot(logical_volume.is_snapshot());
            logical_drive_child->set_mode(LogicalDrive::LvmTypes::LOGICAL_VOLUME);
            logical_drive_child->set_capacity_gb(logical_volume.get_capacity_gb());
            logical_drive_child->set_type(lvm_type_name);
            logical_drive_child->set_device_path("/dev/" + volume_group.get_name()
                                                + "/" + logical_volume.get_name());
            logical_drive_child->set_status({logical_volume.get_status(), logical_volume.get_health()});
            logical_drive->add_logical_drive(logical_drive_child);

            name_to_uuid_map[logical_volume.get_name()] = logical_drive_child->get_uuid();
            if (logical_volume.is_snapshot()) {
                name_to_master_map[logical_volume.get_name()] = logical_volume.get_master_name();
            }
        }

        for (const auto& ld : logical_drive->get_logical_drives()) {
            if (LogicalDrive::LvmTypes::LOGICAL_VOLUME == ld->get_mode() && ld->is_snapshot()) {
                ld->set_master(name_to_uuid_map[name_to_master_map[ld->get_name()]]);
            }
        }

        submodule->add_logical_drive(logical_drive);
    }
}
