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

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "loader/config.hpp"
#include "tools/endpoint_reader.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"

#include <cstdint>
#include <string>
#include <stdexcept>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;

namespace {

void delete_target_endpoint(DeleteEndpoint::ContextPtr context, const Uuid& endpoint_uuid, const std::string& nqn) {
    auto& endpoint_creator = NvmeConfig::get_instance()->get_endpoint_creator();
    try {
        EndpointDatabase db{endpoint_uuid};
        auto nvme_port = db.get(EndpointReader::DATABASE_NVME_PORT);
        endpoint_creator.delete_target_endpoint(context, nqn, nvme_port);
    }
    catch (const std::runtime_error& error) {
        THROW(exceptions::NvmeError, "nvme-agent",
              std::string("Failed deleting target endpoint: ") + error.what());
    }
}

std::string get_nqn(const Endpoint& endpoint) {
    std::string nqn{};
    try {
        nqn = attribute::Identifier::get_nqn(endpoint);
        log_debug("nvme-agent", "NQN is " + nqn);
    }
    catch (const std::logic_error&) {
    }

    if (nqn.empty()) {
        THROW(exceptions::NvmeError, "nvme-agent", "NQN identifier was not found or is empty.");
    }
    return nqn;
}

void delete_endpoint(DeleteEndpoint::ContextPtr context, const DeleteEndpoint::Request& req, DeleteEndpoint::Response&) {
    const auto& uuid{req.get_endpoint()};
    log_info("nvme-agent", "Deleting endpoint '" << uuid << "'...");
    // check if endpoint exists
    auto endpoint = get_manager<Endpoint>().get_entry(uuid);

    // check if endpoint is in a zone
    for (const auto& zone : get_manager<Zone>().get_keys()) {
        if (get_m2m_manager<Zone, Endpoint>().entry_exists(zone, uuid)) {
            THROW(exceptions::InvalidValue, "nvme-agent",
                  "Can't delete endpoint. Remove it from a zone first.");
        }
    }

    if (is_target(endpoint)) {
        delete_target_endpoint(context, req.get_endpoint(), get_nqn(endpoint));
    }
    else {
        // add initiator nqn to hosts configuration
        auto nqn = agent_framework::model::attribute::Identifier::get_nqn(endpoint);
        tools::convert_to_subnqn(nqn);

        // Be sure that "allowed host" is unlinked from all subsystems
        for (const auto& subsystem : context->nvme_target_handler->get_subsystems()) {
            try {
                context->nvme_target_handler->remove_subsystem_host(subsystem, nqn);
                log_warning("nvme-agent", "Force delete host " << nqn << " from subsystem " << subsystem);
            }
            catch (const std::exception& exception) {
                log_debug("nvme-agent", "Failed to force remove subsystem host: " << exception.what());
            }
        }

        context->nvme_target_handler->remove_host(nqn);
    }

    // delete endpoint from fabric's database
    FabricDatabase(get_manager<Fabric>().get_keys().front()).del(uuid);
    // delete endpoint database
    EndpointDatabase(uuid).remove();

    get_manager<Endpoint>().remove_entry(req.get_endpoint());
    log_info("nvme-agent", "Removed endpoint with UUID '" + req.get_endpoint() + "'");
}
}

REGISTER_NVME_COMMAND(DeleteEndpoint, ::delete_endpoint);
