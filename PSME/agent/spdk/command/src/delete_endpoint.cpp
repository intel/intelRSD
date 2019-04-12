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

#include "spdk_agent_commands.hpp"

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entities.hpp"


using namespace agent::spdk;
using namespace agent_framework::model;
using namespace agent_framework;

namespace {

void delete_endpoint(DeleteEndpoint::ContextPtr context, const DeleteEndpoint::Request& request,
                     DeleteEndpoint::Response&) {

    log_info("spdk-agent", "Deleting endpoint...");
    const auto& endpoint_uuid = request.get_endpoint();
    auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);

    if (Endpoint::is_bound_to_zone(endpoint_uuid)) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Endpoint will not be deleted because it is in use. Remove the endpoint from zone first.");
    }

    if (Endpoint::is_target(endpoint)) {
        std::lock_guard<std::mutex> lock{context->spdk_mutex};
        try {
            bool success = context->spdk_api->delete_nvmf_subsystem(attribute::Identifier::get_nqn(endpoint));
            log_debug("spdk-agent", "Delete NVMf subsystem call was successful");

            if (!success) {
                THROW(exceptions::SpdkError, "spdk-agent", "SPDK delete NVMf subsystem returned 'false'.");
            }
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                  "Failed to delete NVMf subsystem: " + std::string{exception.what()});
        }
    }
    else {
        try {
            agent_framework::database::EndpointEntity::remove(endpoint_uuid);
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                "Failed to remove initiator endpoint: " + std::string{exception.what()});
        }
    }

    module::get_manager<Endpoint>().remove_entry(endpoint_uuid);
    log_info("spdk-agent", "Endpoint " << endpoint_uuid << " removed successfully");
}
}

REGISTER_SPDK_COMMAND(DeleteEndpoint, ::delete_endpoint);