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
 * @file pnc/src/discovery/discovery_manager.cpp
 *
 * */

#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "sysfs/sysfs_reader.hpp"
#include "sysfs/sysfs_decoder.hpp"
#include "discovery/discovery_manager.hpp"
#include "discovery/discovery_utils.hpp"
#include "discovery/discoverer_mf3.hpp"
#include "discovery/builders/builder_factory_mf3.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/mrpc/unbind_port.hpp"
#include "nvme/vital_product_data.hpp"
#include "nvme/smart.hpp"
#include "seeprom.hpp"
#include "cable_id.hpp"
#include "configuration/configuration.hpp"
#include "tree_stability/pnc_tree_stabilizer.hpp"
#include "tools/model_tool.hpp"
#include "tools/i2c_tool.hpp"
#include "tools/gas_tool.hpp"
#include "i2c/i2c_access_interface_factory.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "tree_stability/pnc_dry_stabilizer.hpp"

#include <iomanip>
#include <chrono>
#include <thread>
#include <set>

using namespace agent::pnc::discovery::builders;
using namespace agent::pnc::tools;
using namespace agent::pnc;
using namespace agent::pnc::discovery;
using namespace agent::pnc::discovery::utils;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::gas::top;
using namespace agent::pnc::gas::partition;
using namespace agent::pnc::gas::csr;
using namespace agent::pnc::nvme;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent::pnc::i2c;

namespace {

/*!
 * This is a helper function that adds the resource to the model and logs it, Returns resource uuid
 * */
template <typename RESOURCE>
std::string log_and_remove(const std::string& uuid) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    enums::Component component = RAW_TYPE::get_component();
    log_info(GET_LOGGER("pnc-discovery"), component.to_string() << " removed");
    log_debug(GET_LOGGER("pnc-discovery"), "Removed " << component.to_string() << " uuid: " << uuid);
    get_manager<RAW_TYPE>().remove_entry(uuid);
    return uuid;
}

/*!
 * This is a helper function that adds the resource to the model and logs it, Returns resource uuid
 * */
template <typename RESOURCE>
std::string log_and_add(const RESOURCE& resource) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    std::string uuid = resource.get_uuid();
    enums::Component component = RAW_TYPE::get_component();
    log_info(GET_LOGGER("pnc-discovery"), component.to_string() << " found");
    log_debug(GET_LOGGER("pnc-discovery"), component.to_string() << " uuid: " << uuid);
    get_manager<RAW_TYPE>().add_entry(resource);
    return uuid;
}

/*!
 * This is a helper function that updates the resource in the model and logs it. Returns resource uuid
 * */
template <typename RESOURCE>
std::string log_and_update(const RESOURCE& resource) {
    using RAW_TYPE = typename std::remove_reference<RESOURCE>::type;
    std::string uuid = resource.get_uuid();
    enums::Component component = RAW_TYPE::get_component();
    log_debug(GET_LOGGER("pnc-discovery"), component.to_string() << " with uuid: " + uuid << " has been updated");
    get_manager<RAW_TYPE>().get_entry_reference(resource.get_uuid()).get_raw_ref() = resource;
    return uuid;
}

}

DiscoveryManager DiscoveryManager::create(const tools::Toolset& t) {
    std::string chassis_uuid = t.model_tool->get_chassis_uuid();
    Chassis chassis = get_manager<Chassis>().get_entry(chassis_uuid);
    DiscovererPtr discoverer{};

    if (chassis.get_platform() == enums::PlatformType::EDK) {
        BuilderFactoryPtr factory = std::make_shared<BuilderFactory>();
        discoverer = std::make_shared<Discoverer>(Discoverer(chassis.get_platform(), factory));
    }
    else if (chassis.get_platform() == enums::PlatformType::MF3) {
        BuilderFactoryPtr factory = std::make_shared<BuilderFactoryMf3>();
        discoverer = std::make_shared<DiscovererMf3>(factory);
    }
    else {
        log_error(GET_LOGGER("pnc-discovery"), "Not supported platform specified in the configuration");
        throw std::runtime_error("Not supported platform");
    }

    return DiscoveryManager(discoverer, t);
}

DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::discover_zones(const std::string& fabric_uuid, const std::string& switch_uuid,
        GlobalAddressSpaceRegisters& gas) const {
    for (std::uint8_t zone_id = 0; zone_id < gas.top.output.fields.part_num; zone_id++) {
        // Skip management Partition
        if (gas.top.output.fields.current_partition_id != zone_id) {
            log_and_add(m_discoverer->discover_zone(fabric_uuid, m_tools, switch_uuid, gas, zone_id));
        }
    }
}

void DiscoveryManager::discover_ports(const std::string& fabric_uuid, const std::string& switch_uuid,
        GlobalAddressSpaceRegisters& gas) const {
    // List all enabled PCIe Ports
    PortBindingInfo cmd{gas.get_interface()};
    try {
        cmd = m_tools.gas_tool->get_all_port_binding_info(gas);
    }
    catch (const std::exception& e) {
        log_debug(GET_LOGGER("pnc-discovery"), "Cannot get port binding info for all ports " << e.what());
        log_error(GET_LOGGER("pnc-discovery"), "Ports discovery failed");
        return;
    }
    for (uint8_t entry_id = 0; entry_id < cmd.output.fields.info_count; entry_id++) {
        try {
            Port port = m_discoverer->discover_port(switch_uuid, m_tools, gas, cmd, entry_id);
            m_discoverer->update_port(port, gas, m_tools);

            if (enums::PciePortType::UpstreamPort == port.get_port_type()) {
                std::string endpoint_uuid = log_and_add<Endpoint>(m_discoverer->discover_host_endpoint(fabric_uuid));
                get_m2m_manager<Endpoint, Port>().add_entry(endpoint_uuid, port.get_uuid());

                Zone zone{};
                auto zone_id = cmd.output.fields.port_binding_info[entry_id].partition_id;
                if (m_tools.model_tool->get_zone_by_id(zone, switch_uuid, zone_id)) {
                    get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
                }
                else {
                    log_debug(GET_LOGGER("pnc-discovery"), "Zone with id = " << zone_id << " does not exist!");
                    log_error(GET_LOGGER("pnc-discovery"), "Cannot link endpoint to a not existing zone");
                }
            }
            log_debug(GET_LOGGER("pnc-discovery"), "Discovered port with physical id =  " << port.get_port_id());
            log_and_add(std::move(port));
        }
        catch (PncDiscoveryExceptionNoConfiguration&) {
            // this is thrown when no configuration for the port is provided - we ignore ports like this and carry on.
        }
    }
}

void DiscoveryManager::discovery(const std::string&) {

    log_info(GET_LOGGER("pnc-discovery"), "Starting discovery for the PNC agent.");


        std::string manager_uuid = m_tools.model_tool->get_manager_uuid();
        std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();

        std::string fabric_uuid = log_and_add<Fabric>(m_discoverer->discover_fabric(manager_uuid));
        std::string system_uuid = log_and_add<System>(m_discoverer->discover_system(manager_uuid, chassis_uuid));
        log_and_add(m_discoverer->discover_storage_subsystem(system_uuid));

        log_and_update<Chassis>(m_discoverer->discover_chassis(m_tools,
                                                               get_manager<Chassis>().get_entry(chassis_uuid)));

        SysfsDecoder decoder = SysfsDecoder::make_instance(SysfsReader{});
        auto sysfs_switches = decoder.get_switches();
        if (sysfs_switches.size() > 0) {
            SysfsSwitch sysfs_switch = sysfs_switches.front();
            // get gas
            GlobalAddressSpaceRegisters gas =
                GlobalAddressSpaceRegisters::get_default(sysfs_switch.memory_resource);
            // init gas i2c interface
            I2cAccessInterfaceFactory::get_instance().init_gas_interface(sysfs_switch.memory_resource);

            gas.read_top();

            std::string switch_uuid =
                log_and_add(m_discoverer->discover_switch(fabric_uuid, m_tools, chassis_uuid, sysfs_switch));
            discover_zones(fabric_uuid, switch_uuid, gas);
            discover_ports(fabric_uuid, switch_uuid, gas);
        }
        else {
            throw PncDiscoveryExceptionSwitchNotFound{};
        }
        // Remove all temporary ports that were read from the configuration
        m_tools.model_tool->remove_temporary_ports();

    log_info(GET_LOGGER("pnc-discovery"), "Finished discovery of the PNC agent.");
}

bool DiscoveryManager::oob_port_device_discovery(const GlobalAddressSpaceRegisters& gas, const std::string&,
        const std::string& dsp_port_uuid) const {
    try {
        log_debug(GET_LOGGER("pnc-discovery"), "Discovery: getting switch data...");

        std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();
        std::string fabric_uuid = m_tools.model_tool->get_fabric_uuid();
        auto dsp_port = get_manager<Port>().get_entry(dsp_port_uuid);

        log_debug(GET_LOGGER("pnc-discovery"), "Discovery: drive discovery...");
        Drive drive{};
        bool drive_found = false;
        try {
            drive = m_discoverer->discover_oob_drive(chassis_uuid, m_tools, dsp_port_uuid);
            drive_found = true;
        }
        catch (PncDiscoveryExceptionDriveNotFound&) {
            log_debug(GET_LOGGER("pnc-discovery"), "No drive detected on physical port " << dsp_port.get_port_id());
        }

        std::string endpoint_uuid{};
        if (drive_found) {
            auto drive_uuid = add_and_stabilize_drive(drive, dsp_port);
            auto endpoint = m_discoverer->discover_drive_endpoint(fabric_uuid, drive_uuid);
            endpoint_uuid = add_and_stabilize_endpoint(endpoint, dsp_port, true, drive_uuid);
        }
        else {
            auto endpoint = m_discoverer->discover_unknown_target_endpoint(fabric_uuid);
            endpoint_uuid = add_and_stabilize_endpoint(endpoint, dsp_port, false, std::string{});
        }

        update_endpoint_zone_binding(gas, endpoint_uuid, dsp_port);

        return true;
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Discovery FAILED: " << e.what());
        return false;
    }
}

std::string DiscoveryManager::add_and_stabilize_drive(const Drive& drive, const Port& port) const {
    std::string system_uuid = m_tools.model_tool->get_system_uuid();
    std::string storage_uuid = m_tools.model_tool->get_storage_uuid();
    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    log_debug(GET_LOGGER("pnc-discovery"), "Drive has been found on a physical port " << port.get_port_id());
    get_m2m_manager<StorageSubsystem, Drive>().add_entry(storage_uuid, drive.get_uuid());
    log_and_add(drive);
    std::string uuid = ::agent::pnc::PncTreeStabilizer().stabilize_drive(drive.get_uuid());
    m_tools.model_tool->send_event(system_uuid, storage_uuid, enums::Component::StorageSubsystem,
                                   Notification::Update);
    m_tools.model_tool->send_event(chassis_uuid, uuid, enums::Component::Drive, Notification::Add);
    return uuid;
}

std::string DiscoveryManager::add_and_stabilize_endpoint(Endpoint& endpoint, const Port& port,
        bool was_drive_found, const std::string& drive_uuid) const {
    PncDryStabilizer pds{};
    const std::string new_endpoint_uuid = pds.stabilize(endpoint, std::vector<Port>{port});
    bool already_exists = get_manager<Endpoint>().entry_exists(new_endpoint_uuid);
    std::string fabric_uuid = m_tools.model_tool->get_fabric_uuid();

    if (already_exists) {
        if (was_drive_found) {
            log_info(GET_LOGGER("pnc-discovery"), "Regenerating existing endpoint");
            log_debug(GET_LOGGER("pnc-discovery"), "Regenerating endpoint on a physical port " << port.get_port_id());
            m_tools.model_tool->regenerate_endpoint(new_endpoint_uuid, drive_uuid);
        }
        else {
            log_debug(GET_LOGGER("pnc-discovery"), "Endpoint already exists");
        }

    }
    else {
        log_debug(GET_LOGGER("pnc-discovery"), "New endpoint has been found on a physical port " << port.get_port_id());
        get_m2m_manager<Endpoint, Port>().add_entry(endpoint.get_uuid(), port.get_uuid());
        log_and_add(endpoint);
        m_tools.model_tool->send_event(fabric_uuid,
            ::agent::pnc::PncTreeStabilizer().stabilize_pcie_endpoint(endpoint.get_uuid()),
            enums::Component::Endpoint, Notification::Add);
    }
    return new_endpoint_uuid;
}

void DiscoveryManager::update_endpoint_zone_binding(const GlobalAddressSpaceRegisters& gas,
        const std::string& endpoint_uuid, const Port& port) const {
    std::string fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    PortBindingInfo pbi = m_tools.gas_tool->get_port_binding_info(gas, uint8_t(port.get_phys_port_id()));
    for (unsigned i = 0; i < pbi.output.fields.info_count; ++i) {
        if (pbi.output.fields.port_binding_info[0].partition_id != gas.top.output.fields.current_partition_id) {
            Zone zone{};
            m_tools.model_tool->get_zone_by_id(zone, port.get_parent_uuid(),
                pbi.output.fields.port_binding_info[0].partition_id);
            get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
            m_tools.model_tool->send_event(fabric_uuid, zone.get_uuid(), enums::Component::Zone,
                Notification::Update);
        }
    }
}

bool DiscoveryManager::ib_port_device_discovery(const std::string& switch_uuid, const std::string& dsp_port_uuid,
        uint8_t bridge_id, const std::string& drive_uuid) const {
    try {
        // sysfs numbering is different than in pnc, in pnc bridge_id = 0 is reserved for upstream ports
        if (0 == bridge_id) {
            THROW(agent_framework::exceptions::PncError, "pnc-discovery", "Invalid bridge id = 0");
        }
        bridge_id = uint8_t(bridge_id - 1);
        SysfsDecoder decoder = SysfsDecoder::make_instance(SysfsReader{});
        Switch pcie_switch = get_manager<Switch>().get_entry(switch_uuid);
        SysfsBridge sysfs_bridge = decoder.get_bridge_by_switch_path(pcie_switch.get_bridge_path(), bridge_id);

        log_debug(GET_LOGGER("pnc-discovery"), "Discovery: bridge discovery...");
        std::vector<SysfsDevice> devices = decoder.get_devices(sysfs_bridge);
        if (devices.empty()) {
            if (drive_uuid.empty()) {
                // no oob drive + no ib devices -> nothing new detected
                log_debug(GET_LOGGER("pnc-discovery"), "Device not present!");
            }
            else {
                // drive was found via OOB discovery and devices are empty -> this should never happen
                // setting drive in permanent critical state
                critical_state_drive_discovery(drive_uuid);
            }
        }
        else if (devices.size() > 1) {
            // more than one device - should never happen
            log_debug(GET_LOGGER("pnc-discovery"), "Found too many (" << devices.size() << ") devices on port uuid = "
                << dsp_port_uuid);
            log_error(GET_LOGGER("pnc-discovery"), "Too many pcie devices found on port!");
            throw std::runtime_error("Too many devices on port");
        }
        else {
            sysfs_device_discovery(dsp_port_uuid, drive_uuid, decoder, devices.front());
            if (drive_uuid.empty()) {
                // no drives detected but sysfs device found
                log_warning(GET_LOGGER("pnc-discovery"), "Non drive device found!");
            }
            else {
                // normal situation
                sysfs_drive_discovery(drive_uuid, decoder, devices.front());
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Discovery FAILED: " << e.what());
        return false;
    }
}

void DiscoveryManager::sysfs_device_discovery(const std::string& dsp_port_uuid, const std::string drive_uuid,
        const SysfsDecoder& decoder, const SysfsDevice& sysfs_device) const {

    std::string manager_uuid = m_tools.model_tool->get_manager_uuid();
    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();
    // discover pcie device and functions
    PcieDevice device = m_discoverer->discover_pcie_device(manager_uuid, chassis_uuid, sysfs_device);
    std::vector<SysfsFunction> sysfs_functions = decoder.get_functions(sysfs_device);

    log_and_add(device);
    for (const auto& sysfs_function : sysfs_functions) {
        PcieFunction function = m_discoverer->discover_pcie_function(device.get_uuid(), dsp_port_uuid, sysfs_function);
        if (!drive_uuid.empty()) {
            get_m2m_manager<Drive, PcieFunction>().add_entry(drive_uuid, function.get_uuid());
            function.set_functional_device(drive_uuid);
        }
        log_and_add(function);
    }

    m_tools.model_tool->send_event(manager_uuid,
        ::agent::pnc::PncTreeStabilizer().stabilize_pcie_device(device.get_uuid()),
        enums::Component::PcieDevice, Notification::Add);
}

void DiscoveryManager::sysfs_drive_discovery(const std::string drive_uuid,
        const SysfsDecoder& decoder, const SysfsDevice& sysfs_device) const {
    // get all drives
    std::vector<SysfsDrive> sysfs_drives{};
    std::vector<SysfsFunction> sysfs_functions = decoder.get_functions(sysfs_device);
    for (const auto& sysfs_function : sysfs_functions) {
        for (const auto& drive : sysfs_function.drives) {
            sysfs_drives.push_back(drive);
        }
    }

    if (sysfs_drives.empty()) {
        log_error(GET_LOGGER("pnc-discovery"), "Drive was detected but no sysfs drives were discovered");
    }
    else {
        if (sysfs_drives.size() > 1) {
            log_warning(GET_LOGGER("pnc-discovery"), "More than one sysfs drive found! Taking the first one.");
        }
        Drive drive = get_manager<Drive>().get_entry(drive_uuid);
        drive = m_discoverer->discover_ib_drive(drive, sysfs_device, sysfs_drives.front());
        log_and_update(drive);

        // send update event for drive
        m_tools.model_tool->send_event(drive.get_parent_uuid(), drive_uuid, enums::Component::Drive,
            Notification::Update);
    }
}

void DiscoveryManager::critical_state_drive_discovery(const std::string& drive_uuid) const {
    log_debug(GET_LOGGER("pnc-discovery"), "Drive is visible via VPD/Smart but not in the sysfs = " << drive_uuid);
    log_error(GET_LOGGER("pnc-discovery"), "Drive was detected but no pcie devices are present!");
    Drive drive = get_manager<Drive>().get_entry(drive_uuid);
    drive = m_discoverer->discover_no_sysfs_ib_drive(drive);
    log_and_update(drive);
    // send update event for drive
    m_tools.model_tool->send_event(drive.get_parent_uuid(), drive_uuid,
        enums::Component::Drive, Notification::Update);
}

bool DiscoveryManager::update_drive_status(const std::string& port_uuid, const std::string& drive_uuid) const {
    Port port{};
    try {
        log_debug(GET_LOGGER("pnc-discovery"), "Drive status update - gathering data");
        port = get_manager<Port>().get_entry(port_uuid);
        std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();
        Chassis chassis = get_manager<Chassis>().get_entry(chassis_uuid);

        log_debug(GET_LOGGER("pnc-discovery"), "Reading drive status (phys port = " << port.get_port_id()
            << "), twi_port = " << unsigned(port.get_twi_port())
            << ", twi_channel = " << unsigned(port.get_twi_channel()));

        Smart smart{chassis.get_platform()};
        if (m_tools.i2c_tool->get_smart(smart, port)) {
            attribute::Status status = m_tools.map_tool->get_status_from_smart(smart);
            int media_life_left = 100 - smart.fields.percentage_drive_life_used;
            log_debug(GET_LOGGER("pnc-discovery"), "Updating drive...");
            m_tools.model_tool->update_drive_status(drive_uuid, status, media_life_left);
        }
        else {
            throw std::runtime_error("Reading smart failed");
        }
        return true;
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot read drive status for drive on port "
            << port.get_port_id() << ", exception: " << e.what());
        return false;
    }
}

bool DiscoveryManager::update_port_status(const GlobalAddressSpaceRegisters& gas, const std::string& port_uuid) const {

    try {
        auto port = get_manager<Port>().get_entry_reference(port_uuid);
        log_debug(GET_LOGGER("pnc-discovery"), "Updating status of port id = " << port->get_port_id());

        if (m_discoverer->update_port(port.get_raw_ref(), gas, m_tools)) {
            m_tools.model_tool->send_event(port->get_parent_uuid(), port->get_uuid(),
                                           enums::Component::Port, Notification::Update);
        }
        return true;
    }
    catch (const ::agent_framework::exceptions::InvalidUuid& iue) {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot update status on nonexisting port " << port_uuid <<
                                                                                           ", exception: " << iue.what());
        return false;
    }
}

bool DiscoveryManager::remove_devices_on_port(const GlobalAddressSpaceRegisters& gas, const std::string& port_uuid) const {

    try {
        /* here we are assuming the following:
         * -> if there is a function it is the only function of the drive, and the only function of the pcie device
         * -> if there is a drive, it is a single-function, single-port drive
         * */
        std::vector<std::string> drive_uuids = m_tools.model_tool->get_drives_by_dsp_port_uuid(port_uuid);
        std::vector<std::string> function_uuids = m_tools.model_tool->get_functions_by_dsp_port_uuid(port_uuid);
        degenerate_endpoints_by_drive_uuids(gas, drive_uuids);
        remove_pcie_devices_by_function_uuids(function_uuids);
        remove_drives_by_uuids(drive_uuids);

    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("pnc-discovery"), "Cannot remove devices on port "
            << get_manager<Port>().get_entry(port_uuid).get_port_id() << ", exception: " << e.what());
        return false;
    }
    return true;
}

void DiscoveryManager::remove_pcie_devices_by_function_uuids(const std::vector<std::string>& function_uuids) const {

    std::string manager_uuid = m_tools.model_tool->get_manager_uuid();
    for (const auto& function_uuid : function_uuids) {
        auto device_uuid = get_manager<PcieFunction>().get_entry(function_uuid).get_parent_uuid();
        log_and_remove<PcieFunction>(function_uuid);
        log_and_remove<PcieDevice>(device_uuid);
        get_m2m_manager<Drive, PcieFunction>().remove_child(function_uuid);
        m_tools.model_tool->send_event(manager_uuid, device_uuid, enums::Component::PcieDevice, Notification::Remove);
    }
}

void DiscoveryManager::remove_drives_by_uuids(const std::vector<std::string>& drive_uuids) const {

    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();
    std::string system_uuid = m_tools.model_tool->get_system_uuid();
    std::string storage_uuid = m_tools.model_tool->get_storage_uuid();
    for (const auto& drive_uuid : drive_uuids) {
        log_and_remove<Drive>(drive_uuid);
        get_m2m_manager<Drive, PcieFunction>().remove_parent(drive_uuid);
        get_m2m_manager<StorageSubsystem, Drive>().remove_child(drive_uuid);
        m_tools.model_tool->send_event(chassis_uuid, drive_uuid, enums::Component::Drive, Notification::Remove);
    }
    m_tools.model_tool->send_event(system_uuid, storage_uuid, enums::Component::StorageSubsystem, Notification::Update);
}

void DiscoveryManager::degenerate_endpoints_by_drive_uuids(const GlobalAddressSpaceRegisters& gas,
        const std::vector<std::string>& drive_uuids) const {

    std::string fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    // get list of all endpoints on the drives, we use set to exlude duplicates
    std::set<std::string> endpoint_uuids{};
    for (const auto& drive_uuid : drive_uuids) {
        auto uuids = m_tools.model_tool->get_endpoints_by_drive_uuid(drive_uuid);
        endpoint_uuids.insert(uuids.begin(), uuids.end());
    }
    // degenerate all endpoints on the list
    for (const auto& endpoint_uuid : endpoint_uuids) {
        // find zones the endpoint is bound to
        auto zone_uuids = get_m2m_manager<Zone, Endpoint>().get_parents(endpoint_uuid);
        for (const auto& zone_uuid : zone_uuids) {
            m_tools.gas_tool->unbind_endpoint_from_zone(gas, zone_uuid, endpoint_uuid);
            get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);
            m_tools.model_tool->send_event(fabric_uuid, zone_uuid, enums::Component::Zone, Notification::Update);
        }
        // update endpoint to be 'degenerated' in the critical state, this also sends events
        m_tools.model_tool->degenerate_endpoint(endpoint_uuid, drive_uuids);
    }
}
