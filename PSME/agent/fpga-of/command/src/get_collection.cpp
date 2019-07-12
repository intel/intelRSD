/*!
 * @brief Implementation of GetCollection command.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file get_collection.cpp
 */

#include "fpgaof_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/helpers/get_collection.hpp"



using namespace agent::fpgaof;
using namespace agent_framework::model;
using namespace agent_framework::command;

namespace {

void get_collection(GetCollection::ContextPtr, const GetCollection::Request& request,
                    GetCollection::Response& response) {
    auto uuid = request.get_uuid();
    auto name = request.get_name();
    log_debug("fpgaof-agent", "Getting collection of " << name);
    if (process_resource_collections<Manager,
        Link<LinkType::Strong, CollectionType::Systems, System>,
        Link<LinkType::Strong, CollectionType::Chassis, Chassis>,
        Link<LinkType::Strong, CollectionType::Fabrics, Fabric>
    >(uuid, name, response)) {}
    else if (process_resource_collections<System,
        Link<LinkType::Strong, CollectionType::NetworkInterfaces, NetworkInterface>,
        Link<LinkType::Strong, CollectionType::Processors, Processor>
    >(uuid, name, response)) {}
    else if (process_resource_collections<Fabric,
        Link<LinkType::Strong, CollectionType::Endpoints, Endpoint>,
        Link<LinkType::Strong, CollectionType::Zones, Zone>
    >(uuid, name, response)) {}
    else if (process_resource_collections<Zone,
        Link<LinkType::Weak, CollectionType::Endpoints, Endpoint>
    >(uuid, name, response)) {}
    else if (process_resource_collections<Processor,
        Link<LinkType::Weak, CollectionType::Processors, Processor>,
        Link<LinkType::Weak, CollectionType::PCIeFunctions, PcieFunction>
    >(uuid, name, response)) {}
    else {
        THROW(agent_framework::exceptions::InvalidUuid, "fpgaof-agent",
              "Component not found - invalid UUID: " + uuid);
    }

    log_debug("fpgaof-agent", "Getting " << name << " collection successful.");
}

}

REGISTER_FPGAOF_COMMAND(GetCollection, ::get_collection);
