/*!
 * @brief FPGA-oF discoverer interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file discovery_manager.cpp
 * */

#include "opaepp_error.hpp"
#include "discovery/discovery_manager.hpp"
#include "discovery/discoverers/fpgaof_fabric_discoverer.hpp"
#include "discovery/discoverers/fpgaof_endpoint_discoverer.hpp"
#include "discovery/discoverers/fpgaof_zone_discoverer.hpp"
#include "discovery/discoverers/fpgaof_system_discoverer.hpp"
#include "discovery/discoverers/fpgaof_chassis_discoverer.hpp"
#include "discovery/discoverers/fpgaof_manager_discoverer.hpp"
#include "discovery/discoverers/fpgaof_processor_discoverer.hpp"
#include "discovery/discoverers/fpgaof_opaeproxy_interface_discoverer.hpp"

#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "utils.hpp"



using namespace agent::fpgaof::discovery;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::discovery;

namespace {

void create_proxy_context_mapping(agent::fpgaof::AgentContext& context) {
    auto uuids = get_manager<Processor>().get_keys();
    opaepp::ModelUuidToFpgaUniqueKeyMap proxy_mapping;
    for (const auto& uuid : uuids) {
        auto fpga = get_manager<Processor>().get_entry(uuid).get_fpga();
        proxy_mapping[uuid] = opaepp::Device::get_pci_address(fpga.get_bus(), fpga.get_device(), fpga.get_function());
    }
    context.opae_proxy_context->set_model_to_token_mapping(proxy_mapping);
    log_debug("fpgaof-discovery", "FPGA model uuid to opae token mapping created");
}

}

namespace agent {
namespace fpgaof {
namespace discovery {

DiscoveryManager::~DiscoveryManager() {}


Uuid DiscoveryManager::discover() {

    auto manager = discover_manager();
    auto chassis = discover_chassis(manager);
    auto fabric = discover_fabric(manager);
    auto system = discover_system(manager, chassis);
    auto processors = discover_processors(system.get_uuid());

    try {
        auto interfaces = discover_ethernet_interfaces(system);
        for (const auto& interface : interfaces) {
            get_manager<NetworkInterface>().add_entry(interface);
        }
    }
    catch (const agent_framework::exceptions::GamiException& ex) {
        log_error("fpgaof-discovery", ex.get_message());
    }

    for (const auto& processor: processors) {
        get_manager<Processor>().add_entry(processor);
    }
    create_proxy_context_mapping(*m_context);

    auto endpoints = discover_endpoints(fabric);
    for (auto& endpoint: endpoints) {
        get_manager<Endpoint>().add_entry(endpoint);
    }

    auto zones = discover_zones(fabric);
    for (auto& zone: zones) {
        get_manager<Zone>().add_entry(zone);
    }

    get_manager<System>().add_entry(system);
    get_manager<Chassis>().add_entry(chassis);
    get_manager<Fabric>().add_entry(fabric);

    get_manager<Manager>().add_entry(manager);

    return manager.get_uuid();
}


Manager DiscoveryManager::discover_manager() {
    log_debug("fpgaof-discovery", "Starting discovery of a manager.");
    auto manager = FpgaofManagerDiscoverer{}.discover();
    m_stabilizer.stabilize(manager);
    log_info("fpgaof-discovery", "Finished discovery of a manager: " << manager.get_uuid());
    return manager;
}


Chassis DiscoveryManager::discover_chassis(const Manager& manager) {
    log_debug("fpgaof-discovery", "Starting discovery of a chassis.");
    auto discoverer = FpgaofChassisDiscoverer{};
    auto chassis = discoverer.discover(manager.get_uuid());
    m_stabilizer.stabilize(chassis);
    discoverer.discover_database(chassis);
    log_info("fpgaof-discovery", "Finished discovery of a chassis: " << chassis.get_uuid());
    return chassis;
}


Fabric DiscoveryManager::discover_fabric(const Manager& manager) {
    log_debug("fpgaof-discovery", "Starting discovery of a fabric.");
    auto fabric = FpgaofFabricDiscoverer{}.discover(manager.get_uuid());
    m_stabilizer.stabilize(fabric);
    log_info("fpgaof-discovery", "Finished discovery of a fabric: " << fabric.get_uuid());
    return fabric;
}


System DiscoveryManager::discover_system(const Manager& manager, const Chassis& chassis) {
    log_debug("fpgaof-discovery", "Starting discovery of system.");
    auto system = FpgaofSystemDiscoverer{m_context->sysfs_interface}.discover(manager.get_uuid(), chassis.get_uuid());
    m_stabilizer.stabilize(system);
    log_info("fpgaof-discovery", "Finished discovery of system: " << system.get_uuid());
    return system;
}


std::vector<NetworkInterface> DiscoveryManager::discover_ethernet_interfaces(const System& system) {
    log_debug("fpgaof-discovery", "Starting discovery of interfaces.");
    auto interfaces = OpaeproxyNetworkInterfaceDiscoverer{m_context}.discover(system.get_uuid());
    for (auto& interface : interfaces) {
        m_stabilizer.stabilize(interface);
        log_info("fpgaof-discovery", "Finished discovery of interface: " << interface.get_uuid());
    }
    if(interfaces.empty()) {
        log_error("fpgaof-discovery", "No ethernet interfaces found.");
    }
    log_info("fpgaof-discovery", "Finished discovery of interfaces.");
    return interfaces;
}


std::vector<Processor> DiscoveryManager::discover_processors(const Uuid& system_uuid) {
    log_debug("fpgaof-discovery", "Starting discovery of FPGA devices.");

    std::vector<Processor> processors;
    try {
        processors = FpgaofProcessorDiscoverer{m_context}.discover(system_uuid);
        std::for_each(processors.begin(), processors.end(), [this](Processor& processor) {
            m_stabilizer.stabilize(processor);
            log_info("fpgaof-discovery", "Finished discovery of processor: " << processor.get_uuid());
        });
        utils::sync_processors_with_db(system_uuid, processors);
    }
    catch (const opaepp::OpaeppError& ex) {
        log_error("fpgaof-discovery", "processors discovering failed: " + std::string(ex.what()));
    }

    log_info("fpgaof-discovery", "Finished discovery of FPGA devices.");
    return processors;
}


std::vector<Endpoint> DiscoveryManager::discover_endpoints(const Fabric& fabric) {
    log_debug("fpgaof-discovery", "Starting discovery of endpoints.");
    auto endpoints = FpgaofEndpointDiscoverer{m_context}.discover(fabric.get_uuid());
    std::for_each(endpoints.begin(), endpoints.end(), [this](Endpoint& endpoint) {
        m_stabilizer.stabilize(endpoint);
        log_info("fpgaof-discovery", "Finished discovery of endpoint: " << endpoint.get_uuid());
    });
    log_info("fpgaof-discovery", "Finished discovery of endpoints.");
    return endpoints;
}


std::vector<Zone> DiscoveryManager::discover_zones(const Fabric& fabric) {
    log_debug("fpgaof-discovery", "Starting discovery of zones.");
    auto zones = FpgaofZoneDiscoverer{m_context}.discover(fabric.get_uuid());
    log_info("fpgaof-discovery", "Finished discovery of zones.");
    return zones;
}

}
}
}



