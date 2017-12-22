/*!
 * @brief Discovery manager for compute agent class implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file discovery_manager.cpp
 */

#include "ipmi/utils/sdv/grantley_mdr_region_accessor.hpp"
#include "discovery/discovery_manager.hpp"
#include "discovery/discoverers/generic_discoverer.hpp"
#include "discovery/builders/manager_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_chassis_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_system_builder.hpp"
#include "discovery/builders/network_device_builder.hpp"
#include "discovery/builders/platform_specific/grantley/grantley_network_device_function_builder.hpp"
#include "discovery/builders/storage_subsystem_builder.hpp"
#include "discovery/builders/thermal_zone_builder.hpp"
#include "discovery/builders/power_zone_builder.hpp"
#include "tree_stability/compute_dry_stabilizer.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "logger/logger_factory.hpp"
#include "ipmi/utils/sdv/purley_mdr_region_accessor.hpp"

#include "status/bmc.hpp"
#include <sstream>

using namespace agent::compute::discovery;
using namespace ipmi::command::generic;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::compute;


DiscoveryManager::DiscoveryManager(Bmc& bmc, ipmi::sdv::MdrRegionAccessorFactory::Ptr mdr_accessor_factory,
                                   DiscovererFactory::Ptr discoverer_factory)
    : m_bmc(bmc), m_discoverer{discoverer_factory->create(*bmc.get_platform_id(), bmc.ipmi(), mdr_accessor_factory)} {}


DiscoveryManager::~DiscoveryManager() {}


std::string DiscoveryManager::discovery() {
    log_info(GET_LOGGER("compute-discovery"), m_bmc.get_id() << " - Starting discovery");

    m_discoverer->read_mdr();

    auto mgr = discover_manager();
    m_dry_stabilizer.stabilize(mgr);
    auto chassis = discover_chassis(mgr.get_uuid());
    m_dry_stabilizer.stabilize(chassis, mgr);
    auto thermal_zone = discover_thermal_zone(chassis.get_uuid());
    m_dry_stabilizer.stabilize(thermal_zone, chassis);
    auto power_zone = discover_power_zone(chassis.get_uuid());
    m_dry_stabilizer.stabilize(power_zone, chassis);
    auto system = discover_system(mgr.get_uuid(), chassis.get_uuid());
    m_dry_stabilizer.stabilize(system);
    auto processors = discover_processors(system.get_uuid());
    auto memories = discover_memory(system.get_uuid());
    auto storage_subsystem = discover_storage_subsystem(system.get_uuid());
    m_dry_stabilizer.stabilize(storage_subsystem, system);
    auto drives = discover_drives(chassis.get_uuid());
    auto network_interfaces = discover_network_interfaces(system.get_uuid());
    auto network_device = discover_network_device(system.get_uuid());
    m_dry_stabilizer.stabilize(network_device, system);
    auto network_device_function = discover_network_device_function(network_device.get_uuid());
    m_dry_stabilizer.stabilize(network_device_function, system);
    NetworkDeviceBuilder::update_from_function(network_device, network_device_function);
    auto trusted_modules = discover_trusted_modules(system.get_uuid());

    agent_framework::eventing::EventDataVec events{};
    update_collection(events, std::move(processors), system);
    update_collection(events, std::move(memories), system);
    update_collection(events, std::move(drives));
    update_collection(events, std::move(network_interfaces));
    update_collection(events, std::move(trusted_modules), system);
    ManagerBuilder::update_chassis(mgr, chassis.get_uuid());
    add_or_update_with_removal(events, power_zone);
    add_or_update_with_removal(events, thermal_zone);
    add_or_update_with_removal(events, chassis);
    add_or_update_with_removal(events, system);
    add_or_update_with_removal(events, storage_subsystem);
    add_or_update_with_removal(events, network_device);
    add_or_update_with_removal(events, network_device_function);
    add_or_update_with_removal(events, mgr);

    agent_framework::eventing::EventsQueue::get_instance()->push_back(events);
    std::ostringstream msg{};
    for (const auto& event : events) {
        msg << " " << event.to_json().dump() << "\n";
    }
    log_info("compute-discovery", m_bmc.get_id() << " - Discovery events:\n" << msg.str());

    /* generate events for added metrics and definitions */
    if (auto telemetry_service = m_bmc.telemetry()) {
        telemetry_service->process_all_metrics();
    }

    log_info(GET_LOGGER("compute-discovery"), m_bmc.get_id() << " - Discovery finished");

    return mgr.get_uuid();
}


agent_framework::model::Manager DiscoveryManager::discover_manager() {
    auto manager = ManagerBuilder::build_default();

    log_debug(GET_LOGGER("compute-discovery"), "Starting discovery of a manger at " << connection_data_to_string());

    m_discoverer->discover_manager(manager);

    // todo: this should moved to networkinterface connected with given manger
    manager.set_connection_data(m_bmc.get_connection_data());
    manager.set_ipv4_address(m_bmc.get_connection_data().get_ip_address());

    log_debug(GET_LOGGER("compute-discovery"), "Finished discovery of a manager at " << connection_data_to_string());

    return manager;
}


agent_framework::model::Chassis
DiscoveryManager::discover_chassis(const std::string& parent_uuid) {
    auto chassis = ChassisBuilder::build_default(parent_uuid);

    log_debug("compute-discovery", "Starting discovery of a chassis at " << connection_data_to_string());

    m_discoverer->discover_chassis(chassis);

    auto platform = m_bmc.get_platform_id();
    if (platform == ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_PURLEY) {
        chassis.set_platform(enums::PlatformType::PURLEY);
    }
    else if (platform == ipmi::command::generic::PRODUCT_ID_INTEL_XEON_BDC_R) {
        chassis.set_platform(enums::PlatformType::GRANTLEY);
    }

    log_debug("compute-discovery", "Finished discovery of a chassis at " << connection_data_to_string());

    return chassis;
}


agent_framework::model::ThermalZone DiscoveryManager::discover_thermal_zone(const std::string& parent_uuid) {
    auto zone = ThermalZoneBuilder::build_default(parent_uuid);

    log_debug("compute-discovery", "Starting discovery of a thermal zone at " << connection_data_to_string());

    m_discoverer->discover_thermal_zone(zone);

    log_debug("compute-discovery", "Finished discovery of a thermal zone at " << connection_data_to_string());

    return zone;
}


agent_framework::model::PowerZone DiscoveryManager::discover_power_zone(const std::string& parent_uuid) {
    auto zone = PowerZoneBuilder::build_default(parent_uuid);

    log_debug("compute-discovery", "Starting discovery of a thermal zone at " << connection_data_to_string());

    m_discoverer->discover_power_zone(zone);

    log_debug("compute-discovery", "Finished discovery of a thermal zone at " << connection_data_to_string());

    return zone;
}


agent_framework::model::System DiscoveryManager::discover_system(const std::string& parent_uuid,
                                                                 const std::string& chassis_uuid) {
    auto system = SystemBuilder::build_default(parent_uuid, chassis_uuid);

    log_debug(GET_LOGGER("compute-discovery"), "Starting discovery of a system at " << connection_data_to_string());

    m_discoverer->discover_system(system);
    m_discoverer->discover_bios(system);
    m_discoverer->discover_cable_id(system);
    m_discoverer->discover_user_mode(system);
    m_discoverer->set_rackscale_mode(system);

    log_debug(GET_LOGGER("compute-discovery"), "Finished discovery of a system at " << connection_data_to_string());

    return system;
}


std::vector<agent_framework::model::Processor> DiscoveryManager::discover_processors(const std::string& parent_uuid) {
    std::vector<Processor> processors{};

    log_debug(GET_LOGGER("compute-discovery"), "Starting discovery of processors at " << connection_data_to_string());

    m_discoverer->discover_processors(processors, parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"), "Finished discovery of processors at " << connection_data_to_string());

    return processors;
}


std::vector<agent_framework::model::Memory> DiscoveryManager::discover_memory(const std::string& parent_uuid) {
    std::vector<Memory> memories{};

    log_debug(GET_LOGGER("compute-discovery"), "Starting discovery of memory at " << connection_data_to_string());

    m_discoverer->discover_memory(memories, parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"), "Finished discovery of memory at " << connection_data_to_string());

    return memories;
}


StorageSubsystem DiscoveryManager::discover_storage_subsystem(const std::string& parent_uuid) {
    auto storage = StorageSubsystemBuilder::build_default(parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"),
              "Starting discovery of a storage subsystem at " << connection_data_to_string());

    m_discoverer->discover_storage_subsystem(storage);

    log_debug(GET_LOGGER("compute-discovery"),
              "Finished discovery of a storage subsystem at " << connection_data_to_string());

    return storage;
}


std::vector<Drive> DiscoveryManager::discover_drives(const std::string& parent_uuid) {
    std::vector<Drive> drives{};

    log_debug(GET_LOGGER("compute-discovery"), "Starting discovery of drives at " << connection_data_to_string());

    m_discoverer->discover_drives(drives, parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"), "Finished discovery of drives at " << connection_data_to_string());

    return drives;
}


std::vector<agent_framework::model::NetworkInterface>
DiscoveryManager::discover_network_interfaces(const std::string& parent_uuid) {
    std::vector<NetworkInterface> network_interfaces{};

    log_debug(GET_LOGGER("compute-discovery"),
              "Starting discovery of network interfaces at " << connection_data_to_string());

    m_discoverer->discover_network_interfaces(network_interfaces, parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"),
              "Finished discovery of network interfaces at " << connection_data_to_string());

    return network_interfaces;
}


agent_framework::model::NetworkDevice DiscoveryManager::discover_network_device(const std::string& parent_uuid) {
    auto network_device = NetworkDeviceBuilder::build_default(parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"),
              "Starting discovery of a network device at " << connection_data_to_string());

    m_discoverer->discover_network_device(network_device);

    log_debug(GET_LOGGER("compute-discovery"),
              "Finished discovery of a network device at " << connection_data_to_string());

    return network_device;
}


agent_framework::model::NetworkDeviceFunction
DiscoveryManager::discover_network_device_function(const std::string& parent_uuid) {
    auto network_device_function = NetworkDeviceFunctionBuilder::build_default(parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"),
              "Starting discovery of a network device function at " << connection_data_to_string());

    m_discoverer->discover_network_device_function(network_device_function);

    log_debug(GET_LOGGER("compute-discovery"),
              "Finished discovery of a network device function at " << connection_data_to_string());

    return network_device_function;
}


std::vector<agent_framework::model::TrustedModule>
DiscoveryManager::discover_trusted_modules(const std::string& parent_uuid) {
    std::vector<TrustedModule> trusted_modules{};

    log_debug(GET_LOGGER("compute-discovery"),
              "Starting discovery of trusted modules at " << connection_data_to_string());

    m_discoverer->discover_trusted_modules(trusted_modules, parent_uuid);

    log_debug(GET_LOGGER("compute-discovery"),
              "Finished discovery of trusted modules at " << connection_data_to_string());

    return trusted_modules;
}
