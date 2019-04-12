/*!
 * @brief Implementation of AddZoneEndpoints command.
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
 * @file add_zone_endpoints.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "spdk_converter.hpp"
#include "spdk_agent_commands.hpp"



using namespace agent::spdk;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

bool check_if_zone_contains_initiator(const AddZoneEndpoints::Request& request) {
    for (const auto& endpoint_uuid :
            agent_framework::module::get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone())) {

        if (Endpoint::is_initiator(endpoint_uuid)) {
            return true;
        }
    }
    return false;
}


void validate_requested_endpoints(const AddZoneEndpoints::Request& request) {
    auto has_initiator = check_if_zone_contains_initiator(request);

    for (const auto& endpoint_uuid : request.get_endpoints()) {
        if (!get_manager<Endpoint>().entry_exists(endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Given Endpoint doesn't exist.");
        }

        auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (!Endpoint::is_healthy(endpoint)) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Requested endpoint has incorrect status.");
        }
        if (Endpoint::is_bound_to_zone(endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Endpoint is already in a zone.");
        }
        if (Endpoint::is_initiator(endpoint)) {
            if (has_initiator) {
                THROW(exceptions::InvalidValue, "spdk-agent", "Zone contains more than one initiator endpoint.");
            }
            else {
                has_initiator = true;
            }
        }
    }
}


void save_endpoints_in_model(const Uuid& zone, const std::vector<Uuid>& endpoints) {
    for (const auto& uuid : endpoints) {
        get_m2m_manager<Zone, Endpoint>().add_entry(zone, uuid);
    }
}

void save_endpoints_in_db(const Uuid& zone, const std::vector<Uuid>& endpoints) {
    agent_framework::database::ZoneEntity zone_db{zone};
    std::for_each(endpoints.begin(), endpoints.end(), [&zone_db](const Uuid& endpoint) {
        zone_db.append(agent_framework::database::ZONE_ENDPOINTS_PROPERTY, endpoint);
    });
}

::utils::transaction::BaseTransactionHandler::FunctionPair
    add_host_transaction_handler(AgentContext::SPtr context,
        const std::string& subsystem_nqn, const ::spdk::model::Host& host) {

    auto to_commit = [context, subsystem_nqn, host]() {
        try {
            bool success = context->spdk_api->nvmf_subsystem_add_host(subsystem_nqn, host);
            log_debug("spdk-agent",
                "NVMf subsystem add host call was successful NQN(" << subsystem_nqn << ") Host(" << host.get_nqn() << ")");
            if (!success) {
                throw std::runtime_error("SPDK NVMf subsystem add host returned 'false'.");
            }
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                  "Failed to apply new host to NVMf subsystem: " + std::string{exception.what()});
        }
    };

    auto to_rollback = [context, subsystem_nqn, host]() {
        try {
            bool success = context->spdk_api->nvmf_subsystem_remove_host(subsystem_nqn, host);
            if (!success) {
                throw std::runtime_error("SPDK NVMf subsystem remove host returned 'false'.");
            }
        }
        catch (const std::exception& exception) {
            // If rollback failed, we can do nothing
            log_warning("spdk-agent", "Could not rollback NVMf subsystem add host operation: " << exception.what());
        }
    };

    return std::make_pair(to_commit, to_rollback);
}

void add_host_to_nvmf(AgentContext::SPtr context, const std::vector<Uuid>& endpoint_uuids, const Endpoint& initiator) {
    auto transaction_handler = context->transaction_handler_factory->get_handler();
    auto host = agent::spdk::utils::SpdkConverter::convert_initiator_to_host(initiator);
    for (const auto& endpoint_uuid : endpoint_uuids) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (Endpoint::is_target(endpoint)) {
            auto nqn = attribute::Identifier::get_nqn(endpoint);
            transaction_handler->add_handler_pair(add_host_transaction_handler(context, nqn, host));
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

OptionalField<Endpoint> get_initiator(const std::vector<Uuid>& endpoints_uuids) {
    for (const auto& endpoint_uuid : endpoints_uuids) {
        if (Endpoint::is_initiator(endpoint_uuid)) {
            return get_manager<Endpoint>().get_entry(endpoint_uuid);
        }
    }
    return OptionalField<Endpoint>{};
}


void add_zone_endpoints(AddZoneEndpoints::ContextPtr context,
                        const AddZoneEndpoints::Request& request,
                        AddZoneEndpoints::Response&) {

    log_info("spdk-agent", "Request to add endpoints to zone: " << request.get_zone());
    get_manager<Zone>().get_entry(request.get_zone());

    validate_requested_endpoints(request);

    auto endpoints_in_zone = get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone());
    auto requested_endpoints = request.get_endpoints().get_array();

    OptionalField<Endpoint> requested_initiator = get_initiator(requested_endpoints);
    OptionalField<Endpoint> zone_initiator = get_initiator(endpoints_in_zone);

    if (requested_initiator.has_value()) {
        // Update all (new and in-zone) target endpoints with initiator
        std::lock_guard<std::mutex> lock{context->spdk_mutex};

        std::vector<Uuid> endpoints_to_update{};
        endpoints_to_update.insert(endpoints_to_update.end(), endpoints_in_zone.begin(), endpoints_in_zone.end());
        endpoints_to_update.insert(endpoints_to_update.end(), requested_endpoints.begin(), requested_endpoints.end());

        add_host_to_nvmf(context, endpoints_to_update, requested_initiator);
    }
    else if (zone_initiator.has_value()) {
        // Update only new targets
        std::lock_guard<std::mutex> lock{context->spdk_mutex};

        add_host_to_nvmf(context, requested_endpoints, zone_initiator);
    }
    else {
        log_debug("spdk-agent", "No initiator in zone so no hardware changes.");
    }

    save_endpoints_in_db(request.get_zone(), requested_endpoints);
    save_endpoints_in_model(request.get_zone(), requested_endpoints);
    log_info("spdk-agent", "Added endpoints to zone '" + request.get_zone() + "'");

}
}

REGISTER_SPDK_COMMAND(AddZoneEndpoints, ::add_zone_endpoints);