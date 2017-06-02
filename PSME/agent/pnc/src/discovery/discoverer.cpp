/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file discoverer.cpp
 *
 * */

#include "discovery/discoverer.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::pnc::discovery::builders;
using namespace agent::pnc::discovery;
using namespace agent::pnc::tools;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::nvme;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc;

namespace {

}
Discoverer::~Discoverer() {}

Chassis Discoverer::discover_chassis(const Toolset&, const Chassis& chassis) const {
    return m_factory->init_builder(m_factory->get_chassis_builder(), chassis)->build();
}


Fabric Discoverer::discover_fabric(const std::string& manager_uuid) const {
    return m_factory->init_builder(m_factory->get_fabric_builder(), manager_uuid)->build();
}

System Discoverer::discover_system(const std::string& manager_uuid,
        const std::string& chassis_uuid) const {
    return m_factory->init_builder(m_factory->get_system_builder(), manager_uuid)
        ->update_links(chassis_uuid).build();
}

StorageSubsystem Discoverer::discover_storage_subsystem(const std::string& system_uuid) const {
    return m_factory->init_builder(m_factory->get_storage_subsystem_builder(), system_uuid)->build();
}

Switch Discoverer::discover_switch(const std::string& fabric_uuid, const Toolset& tools,
        const std::string& chassis_uuid, const SysfsSwitch& sysfs_switch) const {
    auto builder = m_factory->init_builder(m_factory->get_switch_builder(), fabric_uuid);
    builder->update_sysfs(sysfs_switch).update_links(chassis_uuid);

    Seeprom seeprom{};
    if (tools.i2c_tool->get_seeprom(seeprom)) {
        builder->update_seeprom(seeprom);
    }
    else {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot read switch seeprom data");
    }
    return builder->build();
}

Zone Discoverer::discover_zone(const std::string& fabric_uuid, const tools::Toolset&,
        const std::string& switch_uuid, GlobalAddressSpaceRegisters& gas, uint8_t zone_id) const {
    auto builder = m_factory->init_builder(m_factory->get_zone_builder(), fabric_uuid);
    builder->update_id(zone_id).update_links(switch_uuid);
    try {
        gas.read_partition(zone_id);
        builder->update_pc(gas.partition);
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot read Zone " << unsigned(zone_id) << " data. " << e.what());
    }
    return builder->build();
}

bool Discoverer::update_port(Port& port, const GlobalAddressSpaceRegisters& gas, const Toolset& tools) const {
    bool changed = false;
    try {
        // throw exception if anything went wrong
        auto lsr = tools.gas_tool->get_link_status(gas, uint8_t(port.get_phys_port_id()));

        uint32_t width{};
        uint32_t max_width{};
        double speed{};
        double max_speed = PM85X6_MAX_LINK_SPEED;
        std::tie(width, max_width, speed) =
            tools.map_tool->get_port_width_and_speed_from_link_status(lsr);
        // up to this point speed and max speed are in GT/s - they have to be converted into Gbps before going further
        // for that we need currently configured width and speed in GT/s
        changed |= tools.model_tool->set_port_width_and_speed(port,
            width, max_width,
            tools.map_tool->gtps_to_gbps(speed, width),
            tools.map_tool->gtps_to_gbps(max_speed, width));

        changed |= tools.model_tool->set_status(port,
            tools.map_tool->get_port_status_from_link_status(lsr));
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot update status on port " << port.get_uuid() <<
                                                                               ", exception: " << e.what());
        changed = tools.model_tool->set_status_offline(port);
    }
    return changed;
}


Port Discoverer::discover_port(const std::string& switch_uuid, const tools::Toolset& tools,
        GlobalAddressSpaceRegisters& gas, const PortBindingInfo& cmd, uint8_t entry_id) const {
    auto builder = m_factory->init_builder(m_factory->get_port_builder(), switch_uuid);
    builder->update_binding_info(entry_id, cmd, gas.top);

    // get configuration data
    uint8_t port_id = uint8_t(builder->build().get_phys_port_id());
    Port tmp_port{};
    if (tools.model_tool->get_port_by_phys_id(tmp_port, port_id, true)) {
        builder->update_twi_config(tmp_port.get_twi_port(), tmp_port.get_twi_channel());
    }
    else {
        log_warning(GET_LOGGER("pnc-discovery"),
            "Port configuration data not found for physical port:" << unsigned(port_id));
        throw PncDiscoveryExceptionNoConfiguration{};
    }

    // if host port -> update cable id and csr data
    if (builder->build().get_port_type() == enums::PciePortType::UpstreamPort) {
        CableId cable_id{m_platform};
        if (tools.i2c_tool->get_cable_id(cable_id, tmp_port)) {
            builder->update_cable_id(cable_id);
        }
        else {
            log_warning(GET_LOGGER("pnc-discovery"), "Reading CableId for Port " << unsigned(port_id) << " failed. ");
        }
    }
    return builder->build();
}

Endpoint Discoverer::discover_host_endpoint(const std::string& fabric_uuid) const {
    return m_factory->init_builder(m_factory->get_endpoint_builder(), fabric_uuid)->add_host_entity().build();
}

Endpoint Discoverer::discover_drive_endpoint(const std::string& fabric_uuid, const std::string& drive_uuid) const {
    return m_factory->init_builder(m_factory->get_endpoint_builder(), fabric_uuid)
        ->add_drive_entity(drive_uuid).build();
}

Endpoint Discoverer::discover_unknown_target_endpoint(const std::string& fabric_uuid) const {
    return m_factory->init_builder(m_factory->get_endpoint_builder(), fabric_uuid)
        ->add_unknown_target_entity().build();
}

PcieDevice Discoverer::discover_pcie_device(const std::string& manager_uuid,
        const std::string& chassis_uuid, const SysfsDevice& sysfs_device) const {
    return m_factory->init_builder(m_factory->get_pcie_device_builder(), manager_uuid)->update_sysfs(sysfs_device)
        .update_links(chassis_uuid).build();
}

PcieFunction Discoverer::discover_pcie_function(const std::string& device_uuid,
        const std::string& dsp_port_uuid, const SysfsFunction& sysfs_function) const {
    return m_factory->init_builder(m_factory->get_pcie_function_builder(), device_uuid)
        ->update_sysfs(sysfs_function).update_links(dsp_port_uuid).build();
}

Drive Discoverer::discover_ib_drive(const Drive& drive, const SysfsDevice& sysfs_device,
        const SysfsDrive& sysfs_drive) const {
    return m_factory->init_builder(m_factory->get_drive_builder(), drive)->update_sysfs_drive_data(sysfs_drive)
        .update_sysfs_device_data(sysfs_device).build();
}

Drive Discoverer::discover_no_sysfs_ib_drive(const Drive& drive) const {
    return m_factory->init_builder(m_factory->get_drive_builder(), drive)->update_critical_state().build();
}

Drive Discoverer::discover_oob_drive(const std::string& chassis_uuid, const Toolset& tools,
        const std::string& port_uuid) const {
    auto builder = m_factory->init_builder(m_factory->get_drive_builder(), chassis_uuid);

    bool smart_failed = false;
    bool vpd_failed = false;
    bool fw_failed = false;
    Port port{};
    try {
        port = get_manager<Port>().get_entry(port_uuid);
        builder->update_location(port).add_dsp_port_uuid(port_uuid);
    }
    catch (...) {
        log_debug("pnc-discovery", "Invalid downstream port uuid: " << port_uuid << " for the drive: " << chassis_uuid);
        log_error("pnc-discovery", "Cannot access downstream port of a drive");
    }

    Smart smart(m_platform);
    if (tools.i2c_tool->get_smart(smart, port)) {
        builder->update_smart(tools, smart);
    }
    else {
        log_warning(GET_LOGGER("pnc-discovery"), "Cannot read drive SMART data");
        smart_failed = true;
    }
    VitalProductData vpd{m_platform};
    if (tools.i2c_tool->get_vpd(vpd, port)) {
        builder->update_vpd(vpd);
    }
    else {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot read drive VPD data.");
        vpd_failed = true;
    }

    FirmwareVersion fw{m_platform};
    if (tools.i2c_tool->get_firmware_version(fw, port)) {
        builder->update_firmware_version(fw);
    }
    else {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot read drive Firmware version data");
        fw_failed = true;
    }

    if (vpd_failed && smart_failed && fw_failed) {
        PncDiscoveryExceptionDriveNotFound dnf{};
        dnf.drive = builder->build();
        throw dnf;
    }

    return builder->build();
}
