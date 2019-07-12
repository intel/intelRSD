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
 * @file command/get_collection.cpp
 * @brief GetCollection implementation
 * */

#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "agent-framework/command/helpers/get_collection.hpp"


using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model::attribute;

namespace {

void get_collection(const GetCollection::Request& request, GetCollection::Response& response) {
    const auto& uuid = request.get_uuid();
    const auto& name = request.get_name();

    if (process_resource_collections<model::Manager,
            Link<LinkType::Strong, model::enums::CollectionType::Chassis, model::Chassis>,
            Link<LinkType::Strong, model::enums::CollectionType::Systems, model::System>,
            Link<LinkType::Strong, model::enums::CollectionType::PCIeDevices, model::PcieDevice>,
            Link<LinkType::Strong, model::enums::CollectionType::Fabrics, model::Fabric>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::PcieDevice,
            Link<LinkType::Strong, model::enums::CollectionType::PCIeFunctions, model::PcieFunction>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Chassis,
            Link<LinkType::Strong, model::enums::CollectionType::Drives, model::Drive>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Drive,
            Link<LinkType::Weak, model::enums::CollectionType::PCIeFunctions, model::PcieFunction>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::System,
            Link<LinkType::Strong, model::enums::CollectionType::StorageSubsystems, model::StorageSubsystem>,
            Link<LinkType::Strong, model::enums::CollectionType::Processors, model::Processor>,
            Link<LinkType::Strong, model::enums::CollectionType::NetworkInterfaces, model::NetworkInterface>,
            Link<LinkType::Weak, model::enums::CollectionType::StorageServices, model::StorageService>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::StorageSubsystem,
            Link<LinkType::Weak, model::enums::CollectionType::Drives, model::Drive>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Fabric,
            Link<LinkType::Strong, model::enums::CollectionType::Endpoints, model::Endpoint>,
            Link<LinkType::Strong, model::enums::CollectionType::Switches, model::Switch>,
            Link<LinkType::Strong, model::enums::CollectionType::Zones, model::Zone>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Switch,
            Link<LinkType::Strong, model::enums::CollectionType::Ports, model::Port>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Zone,
            Link<LinkType::Weak, model::enums::CollectionType::Endpoints, model::Endpoint>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Endpoint,
            Link<LinkType::Weak, model::enums::CollectionType::Ports, model::Port>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Processor,
            Link<LinkType::Weak, model::enums::CollectionType::Processors, model::Processor>,
            Link<LinkType::Weak, model::enums::CollectionType::PCIeFunctions, model::PcieFunction>
            >(uuid, name, response)) {}
    else {
        THROW(::agent_framework::exceptions::InvalidUuid, "pnc-gami",
            "Component not found - invalid UUID: " + uuid);
    }

    log_debug("pnc-gami", "Getting collection of " << request.get_name() << " successful.");
}
}

REGISTER_COMMAND(GetCollection, ::get_collection);
