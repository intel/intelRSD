/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/command/get_collection.cpp
 */

#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/rmm_commands.hpp"
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
            Link<LinkType::Strong, model::enums::CollectionType::NetworkInterfaces, model::NetworkInterface>
            >(uuid, name, response)) {}
    else if (process_resource_collections<model::Chassis,
                Link<LinkType::Strong, model::enums::CollectionType::PowerZones, model::PowerZone>,
                Link<LinkType::Strong, model::enums::CollectionType::ThermalZones, model::ThermalZone>
                >(uuid, name, response)) {}
    else if (process_resource_collections<model::NetworkInterface,
                Link<LinkType::Strong, model::enums::CollectionType::EthernetSwitchPortVlans, model::EthernetSwitchPortVlan>
                >(uuid, name, response)) {}
    else if (process_resource_collections<model::PowerZone,
                Link<LinkType::Strong, model::enums::CollectionType::PSUs, model::Psu>
                >(uuid, name, response)) {}
    else if (process_resource_collections<model::ThermalZone,
                Link<LinkType::Strong, model::enums::CollectionType::Fans, model::Fan>
                >(uuid, name, response)) {}
    else {
        THROW(agent_framework::exceptions::InvalidUuid, "rmm-gami",
            "Component not found - invalid UUID: " + uuid);
    }

    log_debug("rmm-agent", "Getting " << name << " collection successful.");
 }

 }

 REGISTER_COMMAND(GetCollection, ::get_collection);
