/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#include "agent-framework/command/helpers/get_collection.hpp"
#include "agent-framework/module/common_components.hpp"
#include "storage_agent_commands.hpp"


using namespace agent::storage;
using namespace agent_framework;

using agent_framework::command::process_resource_collections;
using agent_framework::command::Link;
using agent_framework::command::LinkType;
using agent_framework::command::CollectionType;

namespace {

void get_collection(GetCollection::ContextPtr, const GetCollection::Request& request,
                    GetCollection::Response& response) {
    const auto& uuid = request.get_uuid();
    const auto& collection_name = request.get_name();

    if (process_resource_collections<model::Manager,
        Link<LinkType::Strong, model::enums::CollectionType::Chassis, model::Chassis>,
        Link<LinkType::Strong, model::enums::CollectionType::Systems, model::System>,
        Link<LinkType::Strong, model::enums::CollectionType::StorageServices, model::StorageService>,
        Link<LinkType::Strong, model::enums::CollectionType::Fabrics, model::Fabric>
        >(uuid, collection_name, response)) {}
    else if (process_resource_collections<model::System,
        Link<LinkType::Strong, CollectionType::NetworkInterfaces, model::NetworkInterface>,
        Link<LinkType::Strong, CollectionType::Processors, model::Processor>,
        Link<LinkType::Weak, CollectionType::StorageServices, model::StorageService>
        >(uuid, collection_name, response)) {}
    else if (process_resource_collections<model::Chassis,
        Link<LinkType::Strong, model::enums::CollectionType::Drives, model::Drive>
        >(uuid, collection_name, response)) {}
    else if (process_resource_collections<model::Fabric,
        Link<LinkType::Strong, model::enums::CollectionType::Zones, model::Zone>,
        Link<LinkType::Strong, model::enums::CollectionType::Endpoints, model::Endpoint>
        >(uuid, collection_name, response)) {}
    else if (process_resource_collections<model::StorageService,
        Link<LinkType::Strong, model::enums::CollectionType::StoragePools, model::StoragePool>,
        Link<LinkType::Strong, model::enums::CollectionType::Volumes, model::Volume>,
        Link<LinkType::Weak, model::enums::CollectionType::Drives, model::Drive>
        >(uuid, collection_name, response)) {}
    else if (process_resource_collections<model::StoragePool,
        Link<LinkType::Weak, model::enums::CollectionType::Volumes, model::Volume>
        >(uuid, collection_name, response)) {}
    else if (process_resource_collections<model::Zone,
        Link<LinkType::Weak, model::enums::CollectionType::Endpoints, model::Endpoint>
        >(uuid, collection_name, response)) {}
    else {
        THROW(exceptions::InvalidUuid, "storage-agent", "Component not found. Invalid UUID: '" + uuid + "'");
    }

    log_debug("storage-agent", "Getting " << collection_name << " collection successful.");

}
}

REGISTER_STORAGE_COMMAND(GetCollection, ::get_collection);
