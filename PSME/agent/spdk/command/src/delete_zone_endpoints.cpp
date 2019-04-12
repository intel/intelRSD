/*!
* @brief Implementation of DeleteZoneEndpoints command.
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
* @file delete_zone_endpoints.cpp
*/

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entities.hpp"

#include "spdk_agent_commands.hpp"
#include "spdk_converter.hpp"

using namespace agent::spdk;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;

namespace {

OptionalField<Endpoint> get_initiator(const std::vector<Uuid>& endpoints_uuids) {
    for (const auto& endpoint_uuid : endpoints_uuids) {
        if (Endpoint::is_initiator(endpoint_uuid)) {
            return get_manager<Endpoint>().get_entry(endpoint_uuid);
        }
    }
    return OptionalField<Endpoint>{};
}

::utils::transaction::BaseTransactionHandler::FunctionPair
    remove_host_transaction_handler(AgentContext::SPtr context,
        const std::string& subsystem_nqn, const ::spdk::model::Host& host) {

    auto to_commit = [context, subsystem_nqn, host]() {
        try {
            bool success = context->spdk_api->nvmf_subsystem_remove_host(subsystem_nqn, host);
            log_debug("spdk-agent", "NVMf subsystem remove host call was successful");
            if (!success) {
                throw std::runtime_error("SPDK NVMf subsystem remove host returned 'false'.");
            }
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                  "Failed to remove new host to NVMf subsystem: " + std::string{exception.what()});
        }
    };

    auto to_rollback = [subsystem_nqn, host]() {
        log_debug("spdk-agent", "Rollback NVMf subsystem remove host operation for NQN("
            << subsystem_nqn << ") Host(" << host.get_nqn() << ")");
    };

    return std::make_pair(to_commit, to_rollback);
}

void remove_host_from_nvmf(AgentContext::SPtr context, const std::vector<Uuid>& endpoint_uuids, const Endpoint& initiator) {
    auto transaction_handler = context->transaction_handler_factory->get_handler();
    auto host = agent::spdk::utils::SpdkConverter::convert_initiator_to_host(initiator);
    for (const auto& endpoint_uuid : endpoint_uuids) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (Endpoint::is_target(endpoint)) {
            auto nqn = attribute::Identifier::get_nqn(endpoint);
            transaction_handler->add_handler_pair(remove_host_transaction_handler(context, nqn, host));
        }
    }

    try {
        transaction_handler->commit();
    }
    catch (...) {
        transaction_handler->rollback();
        throw;
    }
}

void remove_endpoints_from_model(const Uuid& zone, const std::vector<Uuid>& endpoints) {
    for (const auto& uuid : endpoints) {
        get_m2m_manager<Zone, Endpoint>().remove_entry(zone, uuid);
    }
}

void remove_endpoints_from_db(const Uuid& zone, const std::vector<Uuid>& endpoints) {
    agent_framework::database::ZoneEntity zone_db{zone};
    std::for_each(endpoints.begin(), endpoints.end(), [&zone_db](const Uuid& endpoint) {
        zone_db.del(endpoint);
    });
}


void validate_endpoints(const Uuid& zone_uuid, const attribute::Array<Uuid>& endpoints_uuids) {
    for (const auto& endpoint_uuid : endpoints_uuids) {
        if (!agent_framework::module::get_m2m_manager<Zone, Endpoint>().entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Zone does not contain provided endpoint.");
        }
    }
}


void delete_zone_endpoints(DeleteZoneEndpoints::ContextPtr context,
                           const DeleteZoneEndpoints::Request& request,
                           DeleteZoneEndpoints::Response&) {

    log_info("spdk-agent", "Deleting endpoints from zone.");

    validate_endpoints(request.get_zone(), request.get_endpoints());

    auto endpoints_in_zone = get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone());
    auto requested_endpoints = request.get_endpoints().get_array();

    OptionalField<Endpoint> requested_initiator = get_initiator(requested_endpoints);

    if (requested_initiator.has_value()) {
        // Remove initiator from all (removed and in-zone) target endpoints
        std::lock_guard<std::mutex> lock{context->spdk_mutex};

        remove_host_from_nvmf(context, endpoints_in_zone, requested_initiator);
    }
    else {
        log_debug("spdk-agent", "Initiator still in zone so no hardware changes.");
    }

    remove_endpoints_from_db(request.get_zone(), requested_endpoints);
    remove_endpoints_from_model(request.get_zone(), requested_endpoints);
    log_info("spdk-agent", "Removed endpoints from zone '" + request.get_zone() + "'");
}


}
REGISTER_SPDK_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);