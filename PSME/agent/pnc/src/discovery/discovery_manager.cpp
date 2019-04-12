/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file pnc/src/discovery/discovery_manager.cpp
 *
 * */

#include "agent-framework/eventing/utils.hpp"
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
#include "loader/pnc_configuration.hpp"
#include "tree_stability/pnc_tree_stabilizer.hpp"
#include "tools/model_tool.hpp"
#include "tools/i2c_tool.hpp"
#include "tools/gas_tool.hpp"
#include "i2c/i2c_access_interface_factory.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "tree_stability/pnc_stabilizer.hpp"
#include "tools/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"

#include <iomanip>
#include <chrono>
#include <thread>
#include <set>
#include "agent-framework/discovery/builders/identifier_builder.hpp"



using namespace agent::pnc::discovery::builders;
using namespace agent::pnc::discovery::device_discoverers;
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
using namespace agent::pnc::i2c;

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent_framework::database;
using namespace agent_framework::model::attribute;


DiscoveryManager::DiscoveryManager(DiscovererPtr discoverer,
                                   const tools::Toolset& toolset,
                                   const builders::BuilderFactoryPtr builderFactory) : m_tools(toolset),
                                                                                       m_discoverer(discoverer) {
    m_pcie_device_discoverers = {
        // Declare discoverers for all PCIe device types here
        std::make_shared<device_discoverers::DriveDiscoverer>(toolset, builderFactory),
        std::make_shared<device_discoverers::ProcessorDiscoverer>(toolset, builderFactory),
        std::make_shared<device_discoverers::UnknownDeviceDiscoverer>(toolset, builderFactory) // Should be last
    };
}


DiscoveryManager DiscoveryManager::create(const tools::Toolset& t) {
    std::string chassis_uuid = t.model_tool->get_chassis_uuid();
    Chassis chassis = get_manager<Chassis>().get_entry(chassis_uuid);
    DiscovererPtr discoverer{};
    BuilderFactoryPtr factory{};

    if (chassis.get_platform() == enums::PlatformType::EDK) {
        factory = std::make_shared<BuilderFactory>();
        discoverer = std::make_shared<Discoverer>(Discoverer(chassis.get_platform(), factory));
    }
    else if (chassis.get_platform() == enums::PlatformType::MF3) {
        factory = std::make_shared<BuilderFactoryMf3>();
        discoverer = std::make_shared<DiscovererMf3>(factory);
    }
    else {
        log_error("pnc-discovery", "Not supported platform specified in the configuration");
        throw std::runtime_error("Not supported platform");
    }

    return DiscoveryManager(discoverer, t, factory);
}


DiscoveryManager::~DiscoveryManager() = default;


void DiscoveryManager::discover_zones(const std::string& fabric_uuid,
                                      const std::string& switch_uuid,
                                      GlobalAddressSpaceRegisters& gas) const {

    const auto& discovery_mode = loader::PncConfiguration::get_instance()->get_fabric_discovery_mode();

    if (discovery_mode == loader::FabricDiscoveryMode::AUTOMATIC) {

        for (std::uint8_t partition_id = 0; partition_id < gas.top.output.fields.part_num; partition_id++) {

            // Skip management Partition
            if (gas.top.output.fields.current_partition_id != partition_id) {
                log_and_add(m_discoverer->discover_zone(fabric_uuid, m_tools, switch_uuid, gas, partition_id));
            }
        }
    }
    else {

        agent_framework::database::FabricEntity fabric_db{fabric_uuid};

        auto zones_uuids{fabric_db.get_multiple_values(db_keys::ZONES)};

        std::for_each(zones_uuids.begin(), zones_uuids.end(), [&](const Uuid& zone_uuid) {
            recreate_zone_from_db(fabric_uuid, switch_uuid, zone_uuid);
        });
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
        log_debug("pnc-discovery", "Cannot get port binding info for all ports " << e.what());
        log_error("pnc-discovery", "Ports discovery failed");
        return;
    }

    for (uint8_t entry_id = 0; entry_id < cmd.output.fields.info_count; entry_id++) {

        try {

            Port port = m_discoverer->discover_port(switch_uuid, m_tools, gas, cmd, entry_id);
            m_discoverer->update_port(port, gas, m_tools);
            Metric metric = m_discoverer->discover_port_health_metric(port, m_tools);
            log_and_add(std::move(metric));

            if (enums::PciePortType::UpstreamPort == port.get_port_type()) {

                const auto& endpoint_discovery_mode = loader::PncConfiguration::get_instance()->get_fabric_discovery_mode();

                if (endpoint_discovery_mode == loader::FabricDiscoveryMode::AUTOMATIC) {

                    std::string endpoint_uuid = log_and_add<Endpoint>(
                        m_discoverer->discover_host_endpoint(fabric_uuid));

                    get_m2m_manager<Endpoint, Port>().add_entry(endpoint_uuid, port.get_uuid());

                    Zone zone{};

                    auto zone_id = cmd.output.fields.port_binding_info[entry_id].partition_id;

                    if (m_tools.model_tool->get_zone_by_id(zone, switch_uuid, zone_id)) {

                        get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
                    }
                    else {

                        log_debug("pnc-discovery", "Zone with id = " << zone_id << " does not exist!");
                        log_error("pnc-discovery", "Cannot link endpoint to a not existing zone");
                    }
                }
                else {

                    recreate_host_endpoint_from_db(fabric_uuid, switch_uuid, port);
                }
            }

            log_debug("pnc-discovery", "Discovered port with physical id =  " << port.get_port_id());
            log_and_add(std::move(port));
        }
        catch (PncDiscoveryExceptionNoConfiguration&) {
            // this is thrown when no configuration for the port is provided - we ignore ports like this and carry on.
        }
    }
}


void DiscoveryManager::discovery() {

    log_info("pnc-discovery", "Starting discovery for the PNC agent.");

    PncTreeStabilizer pts{};
    auto definitions = m_discoverer->discover_metric_definitions();
    for (auto& definition: definitions) {
        log_and_add(definition);
        const std::string new_definition_uuid = pts.stabilize_metric_definition(definition.get_uuid());
    }

    std::string manager_uuid = m_tools.model_tool->get_manager_uuid();
    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();

    std::string system_uuid = log_and_add<System>(m_discoverer->discover_system(manager_uuid, chassis_uuid));
    log_and_add(m_discoverer->discover_storage_subsystem(system_uuid));

    PncStabilizer stabilizer{};
    Fabric fabric = m_discoverer->discover_fabric(manager_uuid);
    std::string fabric_uuid = stabilizer.stabilize(fabric);
    fabric_uuid = log_and_add<Fabric>(fabric);

    log_and_update<Chassis>(m_discoverer->discover_chassis(m_tools, get_manager<Chassis>().get_entry(chassis_uuid)));

    SysfsDecoder decoder = SysfsDecoder::make_instance(SysfsReader{});

    auto sysfs_switches = decoder.get_switches();

    if (sysfs_switches.size() > 0) {

        SysfsSwitch sysfs_switch = sysfs_switches.front();

        // get gas
        GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(sysfs_switch.memory_resource);

        // init gas i2c interface
        I2cAccessInterfaceFactory::get_instance().init_gas_interface(sysfs_switch.memory_resource);

        gas.read_top();

        Uuid switch_uuid = log_and_add(m_discoverer->discover_switch(fabric_uuid, m_tools, chassis_uuid, sysfs_switch));

        discover_zones(fabric_uuid, switch_uuid, gas);

        discover_ports(fabric_uuid, switch_uuid, gas);
    }
    else {
        throw PncDiscoveryExceptionSwitchNotFound{};
    }

    // Remove all temporary ports that were read from the configuration
    m_tools.model_tool->remove_temporary_ports();

    log_info("pnc-discovery", "Finished discovery of the PNC agent.");
}


bool DiscoveryManager::oob_port_device_discovery(const GlobalAddressSpaceRegisters& gas,
                                                 const std::string& dsp_port_uuid) const {

    try {

        for (auto& discoverer: m_pcie_device_discoverers) {

            log_debug("pnc-discovery", "Out-of-band port device discovery for: " << discoverer->get_device_type_name());

            bool device_found = discoverer->oob_port_device_discovery(gas, dsp_port_uuid);

            if (device_found) {

                return true;
            }
        }

        return false;
    }
    catch (const std::exception& e) {

        log_error("pnc-discovery", "Discovery FAILED: " << e.what());

        return false;
    }
}


bool DiscoveryManager::ib_port_device_discovery(const std::string& switch_uuid, const std::string& dsp_port_uuid,
                                                uint8_t bridge_id, const std::string& device_uuid) const {
    try {

        for (auto& discoverer: m_pcie_device_discoverers) {

            log_debug("pnc-discovery", "In-band port device discovery for: " << discoverer->get_device_type_name());

            bool device_found = discoverer->ib_port_device_discovery(switch_uuid, dsp_port_uuid,
                                                                     bridge_id, device_uuid);

            if (device_found) {

                return true;
            }
        }

        return false;
    }
    catch (const std::exception& e) {

        log_error("pnc-discovery", "Discovery FAILED: " << e.what());

        return false;
    }
}


bool DiscoveryManager::update_drive_status(const std::string& port_uuid, const std::string& drive_uuid) const {
    Port port{};
    try {
        log_debug("pnc-discovery", "Drive status update - gathering data");
        port = get_manager<Port>().get_entry(port_uuid);
        std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();
        Chassis chassis = get_manager<Chassis>().get_entry(chassis_uuid);

        log_debug("pnc-discovery", "Reading drive status (phys port = " << port.get_port_id()
                                                                        << "), twi_port = "
                                                                        << unsigned(port.get_twi_port())
                                                                        << ", twi_channel = "
                                                                        << unsigned(port.get_twi_channel()));

        Smart smart{chassis.get_platform()};
        if (m_tools.i2c_tool->get_smart(smart, port)) {
            attribute::Status status = m_tools.map_tool->get_status_from_smart(smart);
            int media_life_left = 100 - smart.fields.percentage_drive_life_used;
            log_debug("pnc-discovery", "Updating drive...");
            m_tools.model_tool->update_drive_status(drive_uuid, status, media_life_left);
        }
        else {
            throw std::runtime_error("Reading smart failed");
        }
        return true;
    }
    catch (const std::exception& e) {
        log_error("pnc-discovery", "Cannot read drive status for drive on port "
            << port.get_port_id() << ", exception: " << e.what());
        return false;
    }
}


bool DiscoveryManager::update_port_health_metric(const agent_framework::model::Port& port) const {
    const auto metrics = get_manager<Metric>().get_keys(
        [&port](const Metric& metric) {
            return metric.get_component_uuid() == port.get_uuid();
        }
    );

    if (metrics.size() != 1) {
        log_error("pnc-discovery",
                  "Invalid number of Metrics (" << metrics.size() << ") for port with uuid " + port.get_uuid());
        return false;
    }
    auto metric = get_manager<Metric>().get_entry_reference(metrics.front());

    bool update = false;
    const auto& health = port.get_status().get_health();
    if (metric->get_value().is_null()) {
        if (health.has_value()) {
            update = true;
        }
    }
    else if (!health.has_value() || metric->get_value().get<std::string>() != health.value().to_string()) {
        update = true;
    }
    if (update) {
        metric->set_value(health);
        agent_framework::eventing::send_event(metric->get_uuid(),
                                              enums::Component::Metric,
                                              enums::Notification::Update,
                                              metric->get_parent_uuid());
    }
    return true;
}


bool DiscoveryManager::update_port_status(const GlobalAddressSpaceRegisters& gas, const std::string& port_uuid) const {

    try {
        auto port_ref = get_manager<Port>().get_entry_reference(port_uuid);
        auto& port = port_ref.get_raw_ref();
        log_debug("pnc-discovery", "Updating status of port id = " << port.get_port_id());

        if (m_discoverer->update_port(port, gas, m_tools)) {
            agent_framework::eventing::send_event(port.get_uuid(),
                                                  enums::Component::Port,
                                                  enums::Notification::Update,
                                                  port.get_parent_uuid());
            return update_port_health_metric(port);
        }
        return true;
    }
    catch (const ::agent_framework::exceptions::InvalidUuid& iue) {
        log_error("pnc-discovery", "Cannot update status on nonexisting port " << port_uuid <<
                                                                               ", exception: " << iue.what());
        return false;
    }
}


bool DiscoveryManager::remove_devices_on_port(const GlobalAddressSpaceRegisters& gas, const Uuid& port_uuid) const {

    try {
        /* here we are assuming the following:
         * -> if there is a function it is the only function of the device, and the only function of the pcie device
         * -> if there is a drive, it is a single-function, single-port drive
         * -> if there is a processor, it is a single-function, single-port processor
         * */
        std::vector<Uuid> device_uuids = m_tools.model_tool->get_devices_by_dsp_port_uuid<Drive, Processor>(port_uuid);
        std::vector<Uuid> function_uuids = m_tools.model_tool->get_functions_by_dsp_port_uuid(port_uuid);
        degenerate_endpoints_by_device_uuids(gas, device_uuids);
        remove_pcie_devices_by_function_uuids(function_uuids);
        remove_devices_by_uuids(device_uuids);

    }
    catch (const std::exception& e) {
        log_error("pnc-discovery", "Cannot remove devices on port "
            << get_manager<Port>().get_entry(port_uuid).get_port_id() << ", exception: " << e.what());
        return false;
    }
    return true;
}


void DiscoveryManager::remove_pcie_devices_by_function_uuids(const std::vector<Uuid>& function_uuids) const {

    Uuid manager_uuid = m_tools.model_tool->get_manager_uuid();
    for (const auto& function_uuid : function_uuids) {
        auto device_uuid = get_manager<PcieFunction>().get_entry(function_uuid).get_parent_uuid();
        log_and_remove<PcieFunction>(function_uuid);
        log_and_remove<PcieDevice>(device_uuid);
        get_m2m_manager<Drive, PcieFunction>().remove_child(function_uuid);
        agent_framework::eventing::send_event(device_uuid, enums::Component::PcieDevice,
                                              enums::Notification::Remove, manager_uuid);
    }
}


void DiscoveryManager::remove_devices_by_uuids(const std::vector<Uuid>& device_uuids) const {

    std::string chassis_uuid = m_tools.model_tool->get_chassis_uuid();
    std::string system_uuid = m_tools.model_tool->get_system_uuid();
    std::string storage_uuid = m_tools.model_tool->get_storage_uuid();

    for (const auto& drive_uuid : device_uuids) {
        if (get_manager<Drive>().entry_exists(drive_uuid)) {
            log_and_remove<Drive>(drive_uuid);
            get_m2m_manager<Drive, PcieFunction>().remove_parent(drive_uuid);
            get_m2m_manager<StorageSubsystem, Drive>().remove_child(drive_uuid);
            m_tools.model_tool->remove_device_from_db<Drive, Chassis>(drive_uuid,
                                                                      m_tools.model_tool->get_dry_stabilized_chassis_uuid());
            agent_framework::eventing::send_event(drive_uuid, enums::Component::Drive,
                                                  enums::Notification::Remove, chassis_uuid);
        }
    }
    agent_framework::eventing::send_event(storage_uuid, enums::Component::StorageSubsystem,
                                          enums::Notification::Update, system_uuid);

    for (const auto& processor_uuid : device_uuids) {
        if (get_manager<Processor>().entry_exists(processor_uuid)) {
            log_and_remove<Processor>(processor_uuid);
            get_m2m_manager<Processor, PcieFunction>().remove_parent(processor_uuid);
            m_tools.model_tool->remove_device_from_db<Processor, System>(processor_uuid,
                                                                         m_tools.model_tool->get_dry_stabilized_system_uuid());
            agent_framework::eventing::send_event(processor_uuid, enums::Component::Processor,
                                                  enums::Notification::Remove, system_uuid);
        }
    }
}


void DiscoveryManager::degenerate_endpoints_by_device_uuids(const GlobalAddressSpaceRegisters& gas,
                                                            const std::vector<Uuid>& device_uuids) const {

    Uuid fabric_uuid = m_tools.model_tool->get_fabric_uuid();
    // get list of all endpoints on the drives, we use set to exclude duplicates
    std::set<Uuid> endpoint_uuids{};
    for (const auto& drive_uuid : device_uuids) {
        auto uuids = m_tools.model_tool->get_endpoints_by_device_uuid(drive_uuid);
        endpoint_uuids.insert(uuids.begin(), uuids.end());
    }
    // degenerate all endpoints on the list
    for (const auto& endpoint_uuid : endpoint_uuids) {
        // find zones the endpoint is bound to
        auto zone_uuids = get_m2m_manager<Zone, Endpoint>().get_parents(endpoint_uuid);
        for (const auto& zone_uuid : zone_uuids) {
            m_tools.gas_tool->unbind_endpoint_from_zone(gas, zone_uuid, endpoint_uuid);
            get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);
            agent_framework::eventing::send_event(zone_uuid, enums::Component::Zone,
                                                  enums::Notification::Update, fabric_uuid);
        }
        // update endpoint to be 'degenerated' in the critical state, this also sends events
        m_tools.model_tool->degenerate_endpoint(endpoint_uuid, device_uuids);
    }
}


void DiscoveryManager::recreate_host_endpoint_from_db(const Uuid& fabric_uuid,
                                                      const Uuid& switch_uuid,
                                                      const Port& port) const {

    log_debug("pnc-discovery", "Recreate host endpoint from db");

    PncStabilizer stabilizer{};
    Switch pcie_switch = get_manager<Switch>().get_entry(switch_uuid);

    FabricEntity fabric_db{fabric_uuid};

    auto endpoints_uuids = fabric_db.get_multiple_values(db_keys::ENDPOINTS);
    auto zones_uuids = fabric_db.get_multiple_values(db_keys::ZONES);

    for (const auto& endpoint_uuid : endpoints_uuids) {

        EndpointEntity endpoint_db{endpoint_uuid};

        auto role_db = endpoint_db.get(db_keys::ENDPOINT_ROLE);

        if (literals::Endpoint::INITIATOR == role_db) {

            log_debug("pnc-discovery", "Found host endpoint in fabric db, endpoint uuid: " << endpoint_uuid);

            auto port_uuid_db = endpoint_db.get(db_keys::PORT);

            const Uuid predicted_port_uuid = stabilizer.dry_stabilize(port, pcie_switch);

            if (predicted_port_uuid == port_uuid_db) {

                EndpointBuilder endpoint_builder;
                endpoint_builder.init(fabric_uuid);

                Endpoint endpoint = endpoint_builder.add_host_entity().build();
                endpoint.set_uuid(endpoint_uuid);
                endpoint.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));

                agent_framework::discovery::IdentifierBuilder::set_uuid(endpoint, endpoint_uuid);

                log_and_add<Endpoint>(endpoint);

                get_m2m_manager<Endpoint, Port>().add_entry(endpoint_uuid, port.get_uuid());

                log_debug("pnc-discovery", "Created host endpoint from fabric db, endpoint uuid: " << endpoint_uuid);

                agent::pnc::discovery::utils::update_endpoint_zone_binding_from_db(zones_uuids, endpoint_uuid);

                break;
            }
        }
    }
}


void DiscoveryManager::recreate_zone_from_db(const Uuid& fabric_uuid,
                                             const Uuid& switch_uuid,
                                             const Uuid& zone_uuid) const {

    ZoneBuilder zone_builder;
    zone_builder.init(fabric_uuid);
    zone_builder.update_links(switch_uuid);

    Zone zone = zone_builder.build();
    zone.set_uuid(zone_uuid);

    log_and_add<Zone>(zone);

    log_debug("pnc-discovery", "Created zone from fabric db, zone uuid: " << zone_uuid);
}
