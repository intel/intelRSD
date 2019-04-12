/*!
 * @brief DeleteEndpoint command implementation
 *
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
 * @file command/delete_endpoint.cpp
 */


#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "agent-framework/exceptions/gami_exception.hpp"
#include "tools/toolset.hpp"
#include "tools/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"


using namespace agent::pnc::tools;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {

void throw_if_endpoint_in_zone(const requests::DeleteEndpoint& request) {
    if (Endpoint::is_bound_to_zone(request.get_endpoint())) {
        THROW(InvalidValue, "pnc-agent",
              "Can't delete endpoint. Remove it from a zone first.");
    }
}


void validate_request(const requests::DeleteEndpoint& request) {
    throw_if_endpoint_in_zone(request);
}

void remove_target_endpoint_association_with_port(const Endpoint& endpoint) {

    Uuid connected_device_uuid  = endpoint.get_connected_entities()[0].get_entity().value();

    auto ports_uuids = Toolset::get().model_tool->get_ports_by_device_uuid(connected_device_uuid);

    for (const auto& port_uuid: ports_uuids) {

        log_debug("pnc-agent", "Remove target endpoint - port association, endpoint uuid: " << endpoint.get_uuid());

        get_m2m_manager<Endpoint, Port>().remove_entry(endpoint.get_uuid(), port_uuid);
    }
}

void remove_initiator_endpoint_association_with_port(const Endpoint& endpoint) {

    auto ports_uuids = Toolset::get().model_tool->get_ports_uuids_by_endpoint_uuid(endpoint.get_uuid());

    for (const auto& port_uuid: ports_uuids) {

        log_debug("pnc-agent", "Remove initiator endpoint - port association, endpoint uuid: " << endpoint.get_uuid());

        get_m2m_manager<Endpoint, Port>().remove_entry(endpoint.get_uuid(), port_uuid);
    }
}

void delete_endpoint(const DeleteEndpoint::Request& request, DeleteEndpoint::Response&) {
    Uuid endpoint_uuid = request.get_endpoint();

    Endpoint endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);   // Throws if Endpoint UUID is invalid

    validate_request(request);

    if (Endpoint::is_target(endpoint_uuid)) {
        remove_target_endpoint_association_with_port(endpoint);
    }
    else {
        remove_initiator_endpoint_association_with_port(endpoint);
    }

    // delete endpoint from fabric's database
    agent_framework::database::FabricEntity(get_manager<Fabric>().get_keys().front()).del(endpoint_uuid);
    // delete endpoint database
    agent_framework::database::EndpointEntity::remove(endpoint_uuid);

    get_manager<Endpoint>().remove_entry(endpoint_uuid);
    log_info("pnc-agent", "Removed endpoint with UUID '" + endpoint_uuid + "'");
}
}

REGISTER_COMMAND(DeleteEndpoint, ::delete_endpoint);
