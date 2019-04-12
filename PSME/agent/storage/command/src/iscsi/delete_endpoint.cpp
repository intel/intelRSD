/*!
 * @brief Implementation of DeleteEndpoint command.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
#include "agent-framework/database/database_entity.hpp"
#include "storage_agent_commands.hpp"


using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::storage;

namespace {

void delete_persistent_attributes(const Uuid& endpoint_uuid) {
    auto& pa = database::PersistentAttributes::get_instance();

    try {
        database::Aggregate<database::PersistentAttributes> aggr{pa, endpoint_uuid, literals::Endpoint::ENDPOINT};
        for (const auto& entity_uuid : aggr.get_multiple_values(literals::ConnectedEntity::ENTITY)) {
            pa.remove_value(entity_uuid, literals::ConnectedEntity::ENTITY);
        }
    }
    catch (const std::out_of_range& e) {
        log_warning("storage-discovery",
            "Could not read Connected Entity for endpoint " << endpoint_uuid << ": " << e.what());
    }
    pa.remove_value(endpoint_uuid, literals::Endpoint::ENDPOINT);
}

void check_usage(const Uuid& endpoint_uuid) {
    if (Endpoint::is_bound_to_zone(endpoint_uuid)) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
            "Endpoint will not be deleted because it is in use. Remove the endpoint from zones first." );
    }
}

void delete_endpoint(DeleteEndpoint::ContextPtr, const DeleteEndpoint::Request& request, DeleteEndpoint::Response&) {
    log_info("storage-agent", "Deleting endpoint...");
    // Check if endpoint exists
    get_manager<Endpoint>().get_entry(request.get_endpoint());

    check_usage(request.get_endpoint());
    delete_persistent_attributes(request.get_endpoint());

    get_manager<Endpoint>().remove_entry(request.get_endpoint());
    log_info("storage-agent", "Removed endpoint with UUID '" + request.get_endpoint() + "'");
}
}

REGISTER_STORAGE_COMMAND(DeleteEndpoint, ::delete_endpoint);
