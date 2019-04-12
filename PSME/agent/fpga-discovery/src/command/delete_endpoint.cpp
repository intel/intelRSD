/*!
* @brief Implementation of DeleteEndpoint command.
*
* @copyright Copyright (c) 2019 Intel Corporation
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



#include "agent-framework/module/common_components.hpp"
#include "fpga_agent_commands.hpp"
#include "tools/databases.hpp"



using namespace agent::fpga_discovery;
using namespace agent::fpga_discovery::tools;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

void delete_endpoint(DeleteEndpoint::ContextPtr, const DeleteEndpoint::Request& req, DeleteEndpoint::Response&) {
    const auto& endpoint_uuid{req.get_endpoint()};
    // check if endpoint exists
    auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);

    // check if endpoint is in a zone
    for (const auto& zone : get_manager<Zone>().get_keys()) {
        if (get_m2m_manager<Zone, Endpoint>().entry_exists(zone, endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "fpga-discovery-agent",
                  "Can't delete endpoint. Remove it from a zone first.");
        }
    }

    FabricDatabase(get_manager<Fabric>().get_keys().front()).del(endpoint_uuid);
    // delete endpoint database
    EndpointDatabase(endpoint_uuid).remove();

    get_manager<Endpoint>().remove_entry(endpoint_uuid);
    log_info("fpga-discovery-agent", "Removed endpoint with UUID '" + endpoint_uuid + "'");
}

}

REGISTER_FPGA_DISCOVERY_COMMAND(DeleteEndpoint, ::delete_endpoint);