/*!
* @brief Implementation of DeleteEndpoint command.
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
* @file delete_endpoint.cpp
*/

#include "fpgaof_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"



using namespace agent::fpgaof;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::fpgaof::loader;
using namespace opaepp;

namespace {

void delete_initiator_endpoint(DeleteEndpoint::ContextPtr ctx, const Endpoint& endpoint) {
    auto host_uuid = attribute::Identifier::get_uuid(endpoint);
    OpaeProxyHostApi::remove_initiator_host(*ctx->opae_proxy_context, host_uuid);
    log_debug("opae-proxy", "opaepp : removed endpoint " + endpoint.get_uuid() + " from opae proxy.");
}


void delete_endpoint(DeleteEndpoint::ContextPtr ctx, const DeleteEndpoint::Request& req, DeleteEndpoint::Response&) {
    const auto& endpoint_uuid{req.get_endpoint()};
    // check if endpoint exists
    auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);

    // check if endpoint is in a zone
    for (const auto& zone_uuid : get_manager<Zone>().get_keys()) {
        if (get_m2m_manager<Zone, Endpoint>().entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "fpgaof-agent",
                  "Can't delete endpoint. Remove it from a zone first.");
        }
    }

    if (Endpoint::is_initiator(endpoint)) {
        delete_initiator_endpoint(ctx, endpoint);
    }

    agent_framework::database::EndpointEntity::remove(endpoint_uuid);
    agent_framework::database::FabricEntity(get_manager<Fabric>().get_keys().front()).del(endpoint_uuid);

    get_manager<Endpoint>().remove_entry(req.get_endpoint());
    log_info("fpgaof-agent", "Removed endpoint with UUID '" + req.get_endpoint() + "'");
}

}

REGISTER_FPGAOF_COMMAND(DeleteEndpoint, ::delete_endpoint);