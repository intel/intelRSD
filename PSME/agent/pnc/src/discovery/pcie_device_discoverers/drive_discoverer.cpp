/*!
 * @brief Implementation of class for discovery of NVMe drives
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file drive_discoverer.cpp
 */

#include "agent-framework/eventing/utils.hpp"
#include "discovery/device_discoverers/drive_discoverer.hpp"
#include "sysfs/sysfs_reader.hpp"



using namespace agent::pnc::discovery;
using namespace agent_framework::model;
using namespace agent::pnc::discovery::device_discoverers;
using namespace agent::pnc::tools;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent::pnc::nvme;
using namespace fru_eeprom::parser;


Drive DriveDiscoverer::discover_oob(const Uuid& downstream_port_uuid) const {
    Uuid chassis_uuid = m_tools.model_tool->get_chassis_uuid();
    PlatformType platform{get_manager<Chassis>().get_entry(chassis_uuid).get_platform()};

    auto builder = m_factory->init_builder(m_factory->get_drive_builder(), chassis_uuid);

    bool smart_failed = false;
    bool vpd_failed = false;
    bool fw_failed = false;
    Port port{};
    try {
        port = get_manager<Port>().get_entry(downstream_port_uuid);
        builder->update_location(port).add_dsp_port_uuid(downstream_port_uuid);
    }
    catch (...) {
        log_debug("pnc-discovery",
                  "Invalid downstream port uuid: " << downstream_port_uuid << " for the drive: " << chassis_uuid);
        log_error("pnc-discovery", "Cannot access downstream port of a drive");
    }

    Smart smart(platform);
    if (m_tools.i2c_tool->get_smart(smart, port)) {
        builder->update_smart(m_tools, smart);
    }
    else {
        smart_failed = true;
    }

    VitalProductData vpd{platform};
    if (m_tools.i2c_tool->get_vpd(vpd, port)) {
        // try parsing data as VPD IPMI FRU format
        try {
            FruEepromParser parser(vpd.raw_data, VitalProductData::NVME_VPD_FRU_IPMI_FORMAT_SIZE_BYTES);
            parser.parse();
            builder->update_vpd(parser);
        }
        catch (...) {
            // if parsing failed - try with SFF FORMAT
            builder->update_vpd(vpd);
        }
    }
    else {
        vpd_failed = true;
    }

    FirmwareVersion fw{platform};
    if (m_tools.i2c_tool->get_firmware_version(fw, port)) {
        builder->update_firmware_version(fw);
    }
    else {
        fw_failed = true;
    }

    if (vpd_failed && smart_failed && fw_failed) {
        PncDiscoveryExceptionDeviceNotFound dnf{};
        throw dnf;
    }

    if (vpd_failed) {
        log_error("pnc-discovery", "Cannot read drive VPD data.");
    }
    if (smart_failed) {
        log_warning("pnc-discovery", "Cannot read drive SMART data");
    }
    if (fw_failed) {
        log_error("pnc-discovery", "Cannot read drive Firmware version data");
    }

    return builder->build();
}


Uuid DriveDiscoverer::discover_oob_add_and_stabilize(const Uuid& downstream_port_uuid) const {
    auto port = agent_framework::module::get_manager<agent_framework::model::Port>().get_entry(downstream_port_uuid);

    Drive drive = discover_oob(downstream_port_uuid);

    log_debug("pnc-discovery", "Drive has been found on a physical port " << port.get_port_id());

    return stabilize_drive_update_model(drive);
}


void DriveDiscoverer::critical_state_device_discovery(const Uuid& drive_uuid) const {

    log_debug("pnc-discovery", "Drive is visible via VPD/Smart but not in the sysfs = " << drive_uuid);
    log_error("pnc-discovery", "Drive was detected but no pcie devices are present!");

    Drive drive = get_manager<Drive>().get_entry(drive_uuid);

    drive = discover_no_sysfs_ib_drive(drive);

    log_and_update(drive);

    // send update event for drive
    agent_framework::eventing::send_event(drive_uuid,
                                          enums::Component::Drive,
                                          Notification::Update,
                                          drive.get_parent_uuid());
}


Drive DriveDiscoverer::discover_no_sysfs_ib_drive(const Drive& drive) const {

    return m_factory->init_builder(m_factory->get_drive_builder(), drive)->update_critical_state().build();
}


void DriveDiscoverer::set_functional_device(agent_framework::model::PcieFunction& pcie_function,
                                            const Uuid& device_uuid) const {

    if (!device_uuid.empty()) {

        get_m2m_manager<Drive, PcieFunction>().add_entry(device_uuid, pcie_function.get_uuid());
        pcie_function.set_functional_device(device_uuid);
    }
}


Uuid DriveDiscoverer::sysfs_device_discovery(const Uuid& dsp_port_uuid,
                                             const Uuid& device_uuid,
                                             const agent::pnc::sysfs::SysfsDecoder& decoder,
                                             const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    log_debug("pnc-discovery", "Begin sysfs device discovery for drive: " << device_uuid);

    std::string manager_uuid = m_tools.model_tool->get_manager_uuid();
    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    // discover pcie device and functions
    PcieDevice pcie_device = BaseClass::discover_pcie_device(manager_uuid, chassis_uuid, sysfs_device);
    std::vector<agent::pnc::sysfs::SysfsFunction> sysfs_functions = decoder.get_functions(sysfs_device);

    // get all drives
    std::vector<agent::pnc::sysfs::SysfsDrive> sysfs_drives{};

    for (const auto& sysfs_function : sysfs_functions) {

        for (const auto& drive : sysfs_function.drives) {

            sysfs_drives.push_back(drive);
        }
    }

    if (sysfs_drives.empty()) {

        log_warning("pnc-discovery",
                    "Drive was detected but no sysfs drives "
                    "were discovered or discovered device is not a drive");

        throw PncDiscoveryExceptionDeviceNotFound();
    }

    log_and_add(pcie_device);

    Drive drive{};
    std::string serial_number{};
    Uuid drive_uuid{};

    if (get_manager<Drive>().entry_exists(device_uuid)) {

        drive = get_manager<Drive>().get_entry(device_uuid);
        serial_number = drive.get_fru_info().get_serial_number();
        drive_uuid = device_uuid;
    }
    else {

        serial_number = pcie_device.get_fru_info().get_serial_number();
        drive_uuid = add_from_sysfs(dsp_port_uuid, serial_number);
        sync_device_properties_with_db(drive_uuid);
    }

    for (const auto& sysfs_function : sysfs_functions) {

        PcieFunction function = BaseClass::discover_pcie_function(pcie_device.get_uuid(), dsp_port_uuid,
                                                                  sysfs_function);

        set_functional_device(function, drive_uuid);

        log_and_add(function);
    }

    const auto& persistent_pcie_device_uuid = ::agent::pnc::PncTreeStabilizer().stabilize_pcie_device(
        pcie_device.get_uuid());

    agent_framework::eventing::send_event(persistent_pcie_device_uuid,
                                          enums::Component::PcieDevice,
                                          agent_framework::model::enums::Notification::Add,
                                          manager_uuid);

    log_debug("pnc-discovery", "End sysfs device discovery for drive: " << drive_uuid);

    return drive_uuid;
}


void DriveDiscoverer::sysfs_discovery(const Uuid& drive_uuid,
                                      const agent::pnc::sysfs::SysfsDevice& sysfs_device) const {

    log_debug("pnc-discovery", "Drive ib sysfs discovery begin");

    agent::pnc::sysfs::SysfsDecoder decoder =
        agent::pnc::sysfs::SysfsDecoder::make_instance(agent::pnc::sysfs::SysfsReader{});

    // get all drives
    std::vector<agent::pnc::sysfs::SysfsDrive> sysfs_drives{};
    std::vector<agent::pnc::sysfs::SysfsFunction> sysfs_functions = decoder.get_functions(sysfs_device);

    for (const auto& sysfs_function : sysfs_functions) {

        for (const auto& drive : sysfs_function.drives) {

            sysfs_drives.push_back(drive);
        }
    }

    if (sysfs_drives.size() > 1) {
        log_warning("pnc-discovery", "More than one sysfs drive found! Taking the first one.");
    }

    Drive drive = get_manager<Drive>().get_entry(drive_uuid);

    drive = discover_ib_drive(drive, sysfs_device, sysfs_drives.front());

    log_and_update(drive);

    // send update event for drive
    agent_framework::eventing::send_event(drive_uuid,
                                          enums::Component::Drive,
                                          Notification::Update,
                                          drive.get_parent_uuid());
}


Drive DriveDiscoverer::discover_ib_drive(const Drive& drive,
                                         const agent::pnc::sysfs::SysfsDevice& sysfs_device,
                                         const agent::pnc::sysfs::SysfsDrive& sysfs_drive) const {

    log_debug("pnc-discovery", "ib drive discovery begin");

    return m_factory->init_builder(m_factory->get_drive_builder(), drive)->update_sysfs_drive_data(sysfs_drive)
        .update_sysfs_device_data(sysfs_device).build();
}


void DriveDiscoverer::sync_device_properties_with_db(const Uuid& device_uuid) const {
    auto drive = agent_framework::module::get_manager<agent_framework::model::Drive>().get_entry_reference(
        device_uuid);
    m_tools.model_tool->sync_device_with_db<agent_framework::model::Drive, agent_framework::model::Chassis>(
        m_tools.model_tool->get_dry_stabilized_chassis_uuid(), drive.get_raw_ref());
    log_debug("pnc-discovery", "Drive [UUID = " + device_uuid + "] properties synchronized with database");
}


Uuid DriveDiscoverer::add_from_sysfs(const Uuid& dsp_port_uuid,
                                     const std::string& serial_number) const {

    log_debug("pnc-discovery", "Add sysfs device: drive");

    Uuid chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    auto builder = m_factory->init_builder(m_factory->get_drive_builder(), chassis_uuid);

    builder->init(chassis_uuid);
    builder->add_dsp_port_uuid(dsp_port_uuid);

    attribute::FruInfo fru_info({serial_number,
                                 OptionalField<std::string>(),
                                 OptionalField<std::string>(),
                                 OptionalField<std::string>()});

    builder->update_fru_info(fru_info);

    Drive drive = builder->build();

    log_debug("pnc-discovery", "Drive was created based on sysfs data, dsp port uuid: " << dsp_port_uuid);

    return stabilize_drive_update_model(drive);
}


const Uuid DriveDiscoverer::stabilize_drive_update_model(const Drive& drive) const {
    Uuid system_uuid = m_tools.model_tool->get_system_uuid();
    Uuid storage_uuid = m_tools.model_tool->get_storage_uuid();
    Uuid chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    log_and_add(drive);

    auto drive_uuid = ::agent::pnc::PncTreeStabilizer().stabilize_drive(drive.get_uuid());
    agent_framework::module::get_m2m_manager<StorageSubsystem, Drive>().add_entry(storage_uuid, drive_uuid);
    agent_framework::eventing::send_event(storage_uuid, enums::Component::StorageSubsystem, Notification::Update,
                                          system_uuid);
    agent_framework::eventing::send_event(drive_uuid, enums::Component::Drive, Notification::Add, chassis_uuid);

    return drive_uuid;
}
