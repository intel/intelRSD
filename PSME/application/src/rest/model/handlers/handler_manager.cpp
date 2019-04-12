/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file handler_manager.cpp
 * @brief HandlerManager class  implementation
 * */

#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/model/handlers/drive_handler.hpp"
#include "psme/rest/model/handlers/volume_handler.hpp"
#include "psme/rest/model/handlers/endpoint_handler.hpp"
#include "psme/rest/model/handlers/ethernet_switch_port_handler.hpp"
#include "psme/rest/model/handlers/storage_subsystem_handler.hpp"
#include "psme/rest/model/handlers/zone_handler.hpp"
#include "psme/rest/model/handlers/pcie_function_handler.hpp"
#include "psme/rest/model/handlers/port_handler.hpp"
#include "psme/rest/model/handlers/manager_handler.hpp"
#include "psme/rest/model/handlers/task_handler.hpp"
#include "psme/rest/model/handlers/metric_definition_handler.hpp"
#include "psme/rest/model/handlers/metric_handler.hpp"
#include "psme/rest/model/handlers/acl_handler.hpp"
#include "psme/rest/model/handlers/storage_service_handler.hpp"
#include "agent-framework/module/requests/compute.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/requests/network.hpp"
#include "agent-framework/module/requests/pnc.hpp"
#include "agent-framework/module/requests/rmm.hpp"
#include "configuration/configuration.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

// custom aliases for readability
using namespace agent_framework::model;
using attribute::Array;
using Component = enums::Component;
using Collection = attribute::Collection;
using CollectionType = agent_framework::model::enums::CollectionType;
using Handlers = std::vector<HandlerInterface*>;
using HandlersWithCollectionName = std::vector<std::pair<std::string, HandlerInterface*>>;
using HandlerPtr = std::shared_ptr<HandlerInterface>;

// aliases for common handlers
using ChassisHandler = GenericHandler
    <requests::GetChassisInfo, Chassis,
        IdPolicy<agent_framework::model::enums::Component::Chassis, NumberingZone::SHARED>>;

// aliases for compute handlers
using SystemHandler = GenericHandler
    <requests::GetSystemInfo, System,
        IdPolicy<Component::System, NumberingZone::SHARED>>;
using ProcessorHandler = GenericHandler
    <requests::GetProcessorInfo, Processor,
        IdPolicy<Component::Processor, NumberingZone::PARENT_SPACE>>;
using MemoryHandler = GenericHandler
    <requests::GetMemoryInfo, Memory,
        IdPolicy<Component::Memory, NumberingZone::PARENT_SPACE>>;
using NetworkInterfaceHandler = GenericHandler
    <requests::GetNetworkInterfaceInfo, NetworkInterface,
        IdPolicy<Component::NetworkInterface, NumberingZone::PARENT_SPACE>>;
using StorageControllerHandler = GenericHandler
    <requests::GetStorageControllerInfo, StorageController,
        IdPolicy<Component::StorageController, NumberingZone::PARENT_SPACE>>;
// StorageSubsystemHandler is defined in storage_subsystem_handler.hpp
// DriveHandler is defined in drive_handler.hpp
using NetworkDeviceHandler = GenericHandler
    <requests::GetNetworkDeviceInfo, NetworkDevice,
        IdPolicy<Component::NetworkDevice, NumberingZone::PARENT_SPACE>>;
using NetworkDeviceFunctionHandler = GenericHandler
    <requests::GetNetworkDeviceFunctionInfo, NetworkDeviceFunction,
        IdPolicy<Component::NetworkDeviceFunction, NumberingZone::PARENT_SPACE>>;
using TrustedModuleHandler = GenericHandler
    <requests::GetTrustedModuleInfo, TrustedModule,
        IdPolicy<Component::TrustedModule, NumberingZone::PARENT_SPACE>>;

// aliases for network handlers
using EthernetSwitchHandler = GenericHandler
    <requests::GetEthernetSwitchInfo, EthernetSwitch,
        IdPolicy<Component::EthernetSwitch, NumberingZone::SHARED>>;
using RemoteSwitchHandler = GenericHandler
    <requests::GetRemoteEthernetSwitchInfo, RemoteEthernetSwitch,
        IdPolicy<Component::RemoteEthernetSwitch, NumberingZone::SHARED>>;
using PortVlanHandler = GenericHandler
    <requests::GetPortVlanInfo, EthernetSwitchPortVlan,
        IdPolicy<Component::EthernetSwitchPortVlan, NumberingZone::PARENT_SPACE>>;
using VlanHandler = GenericHandler
    <requests::GetVlanInfo, Vlan,
        IdPolicy<Component::Vlan, NumberingZone::PARENT_SPACE>>;
using AclRuleHandler = GenericHandler
    <requests::GetAclRuleInfo, AclRule,
        IdPolicy<Component::AclRule, NumberingZone::PARENT_SPACE>>;
using StaticMacHandler = GenericHandler
    <requests::GetPortStaticMacInfo, StaticMac,
        IdPolicy<Component::StaticMac, NumberingZone::PARENT_SPACE>>;

// aliases for storage handlers
using StoragePoolHandler = GenericHandler
    <requests::GetStoragePoolInfo, StoragePool,
        IdPolicy<Component::StoragePool, NumberingZone::PARENT_SPACE>>;
// StorageServiceHandler is defined in storage_service_handler.hpp
// VolumeHandler is defined in volume_handler.hpp

// aliases for pnc handlers
using FabricHandler = GenericHandler
    <requests::GetFabricInfo, Fabric,
        IdPolicy<Component::Fabric, NumberingZone::SHARED>>;
using SwitchHandler = GenericHandler
    <requests::GetSwitchInfo, Switch,
        IdPolicy<Component::Switch, NumberingZone::PARENT_SPACE>>;
// EndpointHandler is defined in endpoint_handler.hpp
// PortHandler is defined in port_handler.hpp
// ZoneHandler is defined in zone_handler.hpp
using PcieDeviceHandler = GenericHandler
    <requests::GetPcieDeviceInfo, PcieDevice,
        IdPolicy<Component::PcieDevice, NumberingZone::PARENT_SPACE>>;
// PcieFunctionHandler is defined in pcie_function_handler.hpp

// aliases for rmm handlers
using PowerZoneHandler = GenericHandler
    <requests::GetPowerZoneInfo, PowerZone,
        IdPolicy<Component::PowerZone, NumberingZone::PARENT_SPACE>>;
using ThermalZoneHandler = GenericHandler
    <requests::GetThermalZoneInfo, ThermalZone,
        IdPolicy<Component::ThermalZone, NumberingZone::PARENT_SPACE>>;
using PsuHandler = GenericHandler
    <requests::GetPsuInfo, Psu,
        IdPolicy<Component::PSU, NumberingZone::PARENT_SPACE>>;
using FanHandler = GenericHandler
    <requests::GetFanInfo, Fan,
        IdPolicy<Component::Fan, NumberingZone::PARENT_SPACE>>;


HandlerManager::~HandlerManager() {}


HandlerManager::HandlerManager() :
    m_component_handlers{
        {enums::Component::Manager,
            HandlerPtr(new ManagerHandler())},
        {enums::Component::Chassis,
            HandlerPtr(new ChassisHandler())},
        {enums::Component::System,
            HandlerPtr(new SystemHandler())},
        {enums::Component::Processor,
            HandlerPtr(new ProcessorHandler())},
        {enums::Component::Memory,
            HandlerPtr(new MemoryHandler())},
        {enums::Component::NetworkInterface,
            HandlerPtr(new NetworkInterfaceHandler())},
        {enums::Component::StorageController,
            HandlerPtr(new StorageControllerHandler())},
        {enums::Component::StorageSubsystem,
            HandlerPtr(new StorageSubsystemHandler())},
        {enums::Component::NetworkDevice,
            HandlerPtr(new NetworkDeviceHandler())},
        {enums::Component::NetworkDeviceFunction,
            HandlerPtr(new NetworkDeviceFunctionHandler())},
        {enums::Component::TrustedModule,
            HandlerPtr(new TrustedModuleHandler())},
        {enums::Component::Drive,
            HandlerPtr(new DriveHandler())},
        {enums::Component::EthernetSwitch,
            HandlerPtr(new EthernetSwitchHandler())},
        {enums::Component::EthernetSwitchPort,
            HandlerPtr(new EthernetSwitchPortHandler())},
        {enums::Component::EthernetSwitchPortVlan,
            HandlerPtr(new PortVlanHandler())},
        {enums::Component::Vlan,
            HandlerPtr(new VlanHandler())},
        {enums::Component::Acl,
            HandlerPtr(new AclHandler())},
        {enums::Component::AclRule,
            HandlerPtr(new AclRuleHandler())},
        {enums::Component::StaticMac,
            HandlerPtr(new StaticMacHandler())},
        {enums::Component::RemoteEthernetSwitch,
            HandlerPtr(new RemoteSwitchHandler())},
        {enums::Component::NeighborSwitch,
            HandlerPtr(new RemoteSwitchHandler())},
        {enums::Component::StorageService,
            HandlerPtr(new StorageServiceHandler())},
        {enums::Component::StoragePool,
            HandlerPtr(new StoragePoolHandler())},
        {enums::Component::Volume,
            HandlerPtr(new VolumeHandler())},
        {enums::Component::Switch,
            HandlerPtr(new SwitchHandler())},
        {enums::Component::Zone,
            HandlerPtr(new ZoneHandler())},
        {enums::Component::Port,
            HandlerPtr(new PortHandler())},
        {enums::Component::Endpoint,
            HandlerPtr(new EndpointHandler())},
        {enums::Component::Fabric,
            HandlerPtr(new FabricHandler())},
        {enums::Component::PcieDevice,
            HandlerPtr(new PcieDeviceHandler())},
        {enums::Component::PcieFunction,
            HandlerPtr(new PcieFunctionHandler())},
        {enums::Component::Task,
            HandlerPtr(new TaskHandler())},
        {enums::Component::MetricDefinition,
            HandlerPtr(new MetricDefinitionHandler())},
        {enums::Component::Metric,
            HandlerPtr(new MetricHandler())},
        {enums::Component::PowerZone,
            HandlerPtr(new PowerZoneHandler())},
        {enums::Component::ThermalZone,
            HandlerPtr(new ThermalZoneHandler())},
        {enums::Component::PSU,
            HandlerPtr(new PsuHandler())},
        {enums::Component::Fan,
            HandlerPtr(new FanHandler())}
    },
    m_collection_handlers{
        {enums::CollectionType::Systems,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::System)->second)},
        {enums::CollectionType::Processors,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Processor)->second)},
        {enums::CollectionType::Memory,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Memory)->second)},
        {enums::CollectionType::NetworkInterfaces,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::NetworkInterface)->second)},
        {enums::CollectionType::EthernetSwitchPortVlans,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::EthernetSwitchPortVlan)->second)},
        {enums::CollectionType::StorageControllers,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::StorageController)->second)},
        {enums::CollectionType::StorageSubsystems,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::StorageSubsystem)->second)},
        {enums::CollectionType::NetworkDevices,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::NetworkDevice)->second)},
        {enums::CollectionType::NetworkDeviceFunctions,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::NetworkDeviceFunction)->second)},
        {enums::CollectionType::TrustedModules,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::TrustedModule)->second)},
        {enums::CollectionType::Drives,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Drive)->second)},
        {enums::CollectionType::EthernetSwitches,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::EthernetSwitch)->second)},
        {enums::CollectionType::NeighborSwitches,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::NeighborSwitch)->second)},
        {enums::CollectionType::EthernetSwitchPorts,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::EthernetSwitchPort)->second)},
        {enums::CollectionType::PortMembers,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::EthernetSwitchPort)->second)},
        {enums::CollectionType::Vlans,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Vlan)->second)},
        {enums::CollectionType::Acls,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Acl)->second)},
        {enums::CollectionType::Rules,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::AclRule)->second)},
        {enums::CollectionType::StaticMacs,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::StaticMac)->second)},
        {enums::CollectionType::StorageServices,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::StorageService)->second)},
        {enums::CollectionType::StoragePools,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::StoragePool)->second)},
        {enums::CollectionType::Volumes,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Volume)->second)},
        {enums::CollectionType::Managers,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Manager)->second)},
        {enums::CollectionType::Fabrics,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Fabric)->second)},
        {enums::CollectionType::Chassis,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Chassis)->second)},
        {enums::CollectionType::Switches,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Switch)->second)},
        {enums::CollectionType::Zones,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Zone)->second)},
        {enums::CollectionType::Ports,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Port)->second)},
        {enums::CollectionType::Endpoints,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Endpoint)->second)},
        {enums::CollectionType::PCIeDevices,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::PcieDevice)->second)},
        {enums::CollectionType::PCIeFunctions,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::PcieFunction)->second)},
        {enums::CollectionType::Tasks,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Task)->second)},
        {enums::CollectionType::MetricDefinitions,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::MetricDefinition)->second)},
        {enums::CollectionType::Metrics,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Metric)->second)},
        {enums::CollectionType::PowerZones,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::PowerZone)->second)},
        {enums::CollectionType::ThermalZones,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::ThermalZone)->second)},
        {enums::CollectionType::PSUs,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::PSU)->second)},
        {enums::CollectionType::Fans,
            HandlerPtr(m_component_handlers
                           .find(enums::Component::Fan)->second)}
    } {}


HandlerInterface* HandlerManager::get_handler(Component component) {
    auto handler = m_component_handlers.find(component);
    if (m_component_handlers.end() == handler) {
        THROW(agent_framework::exceptions::InvalidValue, "rest",
              "Handler for component \"" + std::string(component.to_string()) + "\" not defined.");
    }
    return handler->second.get();
}


HandlerInterface* HandlerManager::get_handler(CollectionType collection_type) {
    auto handler = m_collection_handlers.find(collection_type);
    if (m_component_handlers.end() == handler) {
        THROW(agent_framework::exceptions::InvalidValue, "rest",
              "Handler for collection type \"" + std::string(collection_type.to_string()) + "\" not defined.");
    }
    return handler->second.get();
}


HandlersWithCollectionName HandlerManager::get_handlers(Array<Collection> collections) {
    HandlersWithCollectionName handlers;
    for (const auto& collection : collections) {
        auto handler = m_collection_handlers.find(collection.get_type());
        if (m_collection_handlers.end() == handler) {
            log_error("rest",
                      "Handler for collection type: \"" +
                      std::string(collection.get_type().to_string()) +
                      "\" not defined, proceeding with get_handlers().");
        }
        else {
            handlers.push_back(std::make_pair(
                collection.get_name(), handler->second.get()));
        }
    }
    return handlers;
}


Handlers HandlerManager::get_handlers() const {
    Handlers handlers{};
    for (const auto& handler : m_component_handlers) {
        handlers.push_back(handler.second.get());
    }
    return handlers;
}


void HandlerManager::remove_agent_data(const std::string& in_gami_id) {
    log_info("rest", "remove all resources for agent " << in_gami_id);
    HandlerInterface::Context ctx;
    ctx.mode = HandlerInterface::Context::Mode::AGENT_DISAPPEARED;
    Handlers handlers = get_handlers();
    for (const auto& handler : handlers) {
        handler->remove_agent_data(ctx, in_gami_id);
    }
    psme::rest::eventing::manager::SubscriptionManager::get_instance()->notify(ctx.events);
}

} /* handler */
} /* model */
} /* rest */
} /* psme */
