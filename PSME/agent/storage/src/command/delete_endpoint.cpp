/*!
 * @brief Implementation of DeleteEndpoint command.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file delete_endpoint.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"


using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace database;

namespace {

void delete_persistent_attributes(const Uuid& endpoint_uuid) {

    auto& pa = PersistentAttributes::get_instance();

    try {
        Aggregate<PersistentAttributes> aggr{pa, endpoint_uuid, literals::Endpoint::ENDPOINT};
        for (const auto& entity_uuid : aggr.get_multiple_values(literals::ConnectedEntity::ENTITY)) {
            pa.remove_value(entity_uuid, literals::ConnectedEntity::ENTITY);
        }
    }
    catch (const std::out_of_range& e) {
        log_debug("storage-discovery",
                  "Could not read Connected Entity for endpoint " << endpoint_uuid << ": " << e.what());
    }
    pa.remove_value(endpoint_uuid, literals::Endpoint::ENDPOINT);
}

void check_usage(const Uuid& endpoint_uuid) {
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    auto parents = zone_endpoint_manager.get_parents(endpoint_uuid);
    if (parents.size() > 0) {
        THROW(exceptions::InvalidValue, "storage-agent",
        "Endpoint will not be deleted, because it is in use - first remove the endpoint from zones" );
    }
}

void delete_endpoint(const DeleteEndpoint::Request& req, DeleteEndpoint::Response&) {
    log_debug("storage-agent", "Deleting endpoint.");
    // check if endpoint exists
    get_manager<Endpoint>().get_entry(req.get_endpoint());

    check_usage(req.get_endpoint());
    delete_persistent_attributes(req.get_endpoint());

    get_manager<Endpoint>().remove_entry(req.get_endpoint());
    log_debug("storage-agent", "Removed endpoint with UUID '" + req.get_endpoint() + "'");
}
}

REGISTER_COMMAND(DeleteEndpoint, ::delete_endpoint);
