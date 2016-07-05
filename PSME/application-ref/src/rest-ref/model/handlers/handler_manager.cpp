/*!
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
 * @file handler_manager.cpp
 * @brief HandlerManager class  implementation
 * */
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/generic_handler_deps.hpp"
#include "psme/rest-ref/model/handlers/generic_handler.hpp"
#include "psme/rest-ref/model/handlers/logical_drive_handler.hpp"
#include "psme/rest-ref/model/handlers/physical_drive_handler.hpp"
#include "psme/rest-ref/model/handlers/switch_port_handler.hpp"
#include "psme/rest-ref/model/handlers/acl_handler.hpp"
#include "psme/rest-ref/model/handlers/manager_handler.hpp"
#include "agent-framework/module-ref/requests/compute.hpp"
#include "agent-framework/module-ref/requests/storage.hpp"
#include "agent-framework/module-ref/requests/network.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

    using namespace agent_framework::model;

    // aliases for common handlers
    using ChassisHandler = GenericHandler
                <agent_framework::model::requests::GetChassisInfo,
                agent_framework::model::Chassis, ChassisIdPolicy>;

    // aliases for compute handlers
    using SystemHandler = GenericHandler
                <agent_framework::model::requests::GetSystemInfo,
                agent_framework::model::System, GlobalIdPolicy>;
    using ProcessorHandler = GenericHandler
                <agent_framework::model::requests::GetProcessorInfo,
                agent_framework::model::Processor, LocalIdPolicy>;
    using DimmHandler = GenericHandler
                <agent_framework::model::requests::GetDimmInfo,
                agent_framework::model::Dimm, LocalIdPolicy>;
    using MemoryChunkHandler = GenericHandler
                <agent_framework::model::requests::GetMemoryChunkInfo,
                agent_framework::model::MemoryChunk, LocalIdPolicy>;
    using NetworkInterfaceHandler = GenericHandler
                <agent_framework::model::requests::GetNetworkInterfaceInfo,
                agent_framework::model::NetworkInterface, LocalIdPolicy>;
    using StorageControllerHandler = GenericHandler
                <agent_framework::model::requests::GetStorageControllerInfo,
                agent_framework::model::StorageController, LocalIdPolicy>;
    using DriveHandler = GenericHandler
                <agent_framework::model::requests::GetDriveInfo,
                agent_framework::model::Drive, LocalIdPolicy>;

    // aliases for network handlers
    using EthernetSwitchHandler = GenericHandler
                <agent_framework::model::requests::GetEthernetSwitchInfo,
                agent_framework::model::Switch, GlobalIdPolicy>;
    using RemoteSwitchHandler = GenericHandler
                <agent_framework::model::requests::GetRemoteEthernetSwitchInfo,
                agent_framework::model::RemoteSwitch, GlobalIdPolicy>;
    using PortVlanHandler = GenericHandler
                <agent_framework::model::requests::GetPortVlanInfo,
                agent_framework::model::PortVlan, LocalIdPolicy>;
    using VlanHandler = GenericHandler
                <agent_framework::model::requests::GetVlanInfo,
                agent_framework::model::Vlan, LocalIdPolicy>;
    using AclRuleHandler = GenericHandler
                <agent_framework::model::requests::GetAclRuleInfo,
                agent_framework::model::AclRule, LocalIdPolicy>;
    using StaticMacHandler = GenericHandler
                <agent_framework::model::requests::GetPortStaticMacInfo,
                agent_framework::model::StaticMac, LocalIdPolicy>;

    // aliases for storage handlers
    using StorageServiceHandler = GenericHandler
                <agent_framework::model::requests::GetStorageServicesInfo,
                agent_framework::model::StorageServices, GlobalIdPolicy>;
    using IscsiTargetHandler = GenericHandler
                <agent_framework::model::requests::GetIscsiTargetInfo,
                agent_framework::model::IscsiTarget, LocalIdPolicy>;

    // custom aliases for readability
    using agent_framework::model::attribute::Array;
    using Collection = agent_framework::model::attribute::Collection;
    using CollectionType = agent_framework::model::enums::CollectionType;
    using Component = agent_framework::model::enums::Component;
    using Handlers = std::vector<HandlerInterface*>;
    using HandlersWithCollectionName =
            std::vector<std::pair<std::string, HandlerInterface*>>;
    using HandlerPtr = std::shared_ptr<HandlerInterface>;

    HandlerManager::~HandlerManager() {}

    HandlerManager::HandlerManager() :
    m_component_handlers{
        { enums::Component::Manager,
                                HandlerPtr(new ManagerHandler()) },
        { enums::Component::Chassis,
                                HandlerPtr(new ChassisHandler()) },
        { enums::Component::System,
                                HandlerPtr(new SystemHandler()) },
        { enums::Component::Processor,
                                HandlerPtr(new ProcessorHandler()) },
        { enums::Component::Dimm,
                                HandlerPtr(new DimmHandler()) },
        { enums::Component::MemoryChunk,
                                HandlerPtr(new MemoryChunkHandler()) },
        { enums::Component::NetworkInterface,
                                HandlerPtr(new NetworkInterfaceHandler()) },
        { enums::Component::StorageController,
                                HandlerPtr(new StorageControllerHandler()) },
        { enums::Component::Drive,
                                HandlerPtr(new DriveHandler()) },
        { enums::Component::Switch,
                                HandlerPtr(new EthernetSwitchHandler()) },
        { enums::Component::SwitchPort,
                                HandlerPtr(new SwitchPortHandler()) },
        { enums::Component::Acl,
                                HandlerPtr(new AclHandler()) },
        { enums::Component::AclRule,
                                HandlerPtr(new AclRuleHandler()) },
        { enums::Component::StaticMac,
                                HandlerPtr(new StaticMacHandler()) },
        { enums::Component::PortVlan,
                                HandlerPtr(new PortVlanHandler()) },
        { enums::Component::Vlan,
                                HandlerPtr(new VlanHandler()) },
        { enums::Component::RemoteSwitch,
                                HandlerPtr(new RemoteSwitchHandler()) },
        { enums::Component::NeighborSwitch,
                                HandlerPtr(new RemoteSwitchHandler()) },
        { enums::Component::StorageServices,
                                HandlerPtr(new StorageServiceHandler()) },
        { enums::Component::IscsiTarget,
                                HandlerPtr(new IscsiTargetHandler()) },
        { enums::Component::LogicalDrive,
                                HandlerPtr(new LogicalDriveHandler()) },
        { enums::Component::PhysicalDrive,
                                HandlerPtr(new PhysicalDriveHandler()) }
    },
    m_collection_handlers{
        { enums::CollectionType::Systems,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::System)->second) },
        { enums::CollectionType::Processors,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Processor)->second) },
        { enums::CollectionType::Dimms,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Dimm)->second) },
        { enums::CollectionType::MemoryChunks,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::MemoryChunk)->second) },
        { enums::CollectionType::NetworkInterfaces,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::NetworkInterface)->second) },
        { enums::CollectionType::PortVlans,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::PortVlan)->second) },
        { enums::CollectionType::StorageControllers,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::StorageController)->second) },
        { enums::CollectionType::Drives,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Drive)->second) },
        { enums::CollectionType::EthernetSwitches,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Switch)->second) },
        { enums::CollectionType::NeighborSwitches,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::NeighborSwitch)->second) },
        { enums::CollectionType::Ports,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::SwitchPort)->second) },
        { enums::CollectionType::PortMembers,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::SwitchPort)->second) },
        { enums::CollectionType::Acls,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Acl)->second) },
        { enums::CollectionType::Rules,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::AclRule)->second) },
        { enums::CollectionType::StaticMacs,
                HandlerPtr(m_component_handlers
                        .find(enums::Component::StaticMac)->second) },
        { enums::CollectionType::Vlans,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Vlan)->second) },
        { enums::CollectionType::StorageServices,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::StorageServices)->second) },
        { enums::CollectionType::LogicalDrives,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::LogicalDrive)->second) },
        { enums::CollectionType::PhysicalDrives,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::PhysicalDrive)->second) },
        { enums::CollectionType::iSCSITargets,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::IscsiTarget)->second) },
        { enums::CollectionType::Managers,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Manager)->second) },
        { enums::CollectionType::Chassis,
                        HandlerPtr(m_component_handlers
                        .find(enums::Component::Chassis)->second) }
    }
    {}

    HandlerInterface* HandlerManager::get_handler(Component component) {
        auto handler = m_component_handlers.find(component);
        if (m_component_handlers.end() == handler) {
            THROW(::agent_framework::exceptions::InvalidParameters, "rest",
                "Handler for component \"" +
                std::string(component.to_string()) + "\" not defined");
        }
        return handler->second.get();
    }

    HandlerInterface* HandlerManager::get_handler(CollectionType collection_type) {
        auto handler = m_collection_handlers.find(collection_type);
        if (m_component_handlers.end() == handler) {
            THROW(::agent_framework::exceptions::InvalidParameters, "rest",
                  "Handler for collection type \"" +
                  std::string(collection_type.to_string()) + "\" not defined");
        }
        return handler->second.get();
    }

    HandlersWithCollectionName HandlerManager::get_handlers(Array<Collection> collections) {
        HandlersWithCollectionName handlers;
        for (const auto& collection : collections) {
            auto handler = m_collection_handlers.find(collection.get_type());
            if(m_collection_handlers.end() == handler) {
                log_error(GET_LOGGER("rest"),
                    "Handler for collection type: \"" +
                    std::string(collection.get_type().to_string()) +
                    "\" not defined, proceeding with get_handlers()");
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
        for(const auto& handler : m_component_handlers) {
            handlers.push_back(handler.second.get());
        }
        return handlers;
    }

    void HandlerManager::remove_agent_data(const std::string& in_gami_id) {
        Handlers handlers = get_handlers();
        for(const auto& handler : handlers) {
            handler->remove_agent_data(in_gami_id);
        }
    }

} /* handler */
} /* model */
} /* rest */
} /* psme */
