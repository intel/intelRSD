/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * */

#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "agent-framework/command/helpers/get_collection.hpp"


using namespace agent_framework;
using namespace agent_framework::command;

namespace {

void get_collection(const GetCollection::Request& request, GetCollection::Response& response) {
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
        THROW(exceptions::InvalidUuid, "storage-gami",
              "Component not found - invalid UUID: " + uuid);
    }

    log_debug("storage-gami", "Getting " << collection_name << " collection successful.");
}

}

REGISTER_COMMAND(GetCollection, ::get_collection);
