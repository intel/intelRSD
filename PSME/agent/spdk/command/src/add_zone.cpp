/*!
 * @brief Implementation of AddZone command.
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
 * @file add_zone.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/discovery/builders/zone_builder.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"

#include "spdk_agent_commands.hpp"
#include "spdk_stabilizer.hpp"
#include "spdk_converter.hpp"


using namespace agent::spdk;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

void check_endpoints_added_to_zone(const attribute::Array<Uuid>& endpoint_uuids) {
    bool initiator_found = false;

    for (const Uuid& endpoint_uuid : endpoint_uuids) {
        const auto& endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);

        std::string nqn{};
        try {
            nqn = attribute::Identifier::get_nqn(endpoint);
            if (nqn.empty()) {
                THROW(exceptions::SpdkError, "spdk-agent", "NQN identifier cannot be empty.");
            }
        }
        catch (const std::logic_error&) {
            THROW(exceptions::SpdkError, "spdk-agent", "Unable to read NQN from endpoint.");
        }

        if (!Endpoint::is_healthy(endpoint)) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Zone has Endpoint with incorrect status.");
        }

        if (endpoint.get_connected_entities().empty()) {
            THROW(exceptions::InvalidValue, "spdk-agent", "One of the endpoints have empty connected entities.");
        }

        if (Endpoint::is_initiator(endpoint)) {
            if (initiator_found) {
                THROW(exceptions::InvalidValue, "spdk-agent", "Zone has duplicated Initiator Endpoint!");
            }
            initiator_found = true;
        }
        else if (Endpoint::is_target(endpoint)) {
            if (endpoint.get_ip_transport_details().empty()) {
                THROW(exceptions::InvalidValue, "spdk-agent",
                      "No transport details found in provided target endpoint.");
            }
            for (const auto& transport : endpoint.get_ip_transport_details()) {
                auto ipv4_address = transport.get_ipv4_address().get_address();
                if (!ipv4_address.has_value() || ipv4_address.value().empty()) {
                    THROW(exceptions::InvalidValue, "spdk-agent", "Target endpoint has no IPv4 address set.");
                }
            }
        }

        if (Endpoint::is_bound_to_zone(endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "spdk-agent", "Another zone already contains requested endpoint!");
        }
    }
}


OptionalField<Uuid> find_initiator(const AddZone::Request& request) {
    for (const auto& endpoint_uuid : request.get_endpoints()) {
        if (Endpoint::is_initiator(endpoint_uuid)) {
            return endpoint_uuid;
        }
    }
    return OptionalField<Uuid>{};
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

void add_nvmf_hosts(AgentContext::SPtr context,
                    const attribute::Array<Uuid>& endpoint_uuids,
                    const Uuid& initiator_uuid) {

    auto initiator = get_manager<Endpoint>().get_entry(initiator_uuid);
    auto host = agent::spdk::utils::SpdkConverter::convert_initiator_to_host(initiator);

    auto transaction_handler = context->transaction_handler_factory->get_handler();

    for (const Uuid& uuid : endpoint_uuids) {
        auto endpoint = get_manager<Endpoint>().get_entry(uuid);
        if (Endpoint::is_target(endpoint)) {
            auto nqn = attribute::Identifier::get_nqn(endpoint);
            transaction_handler->add_handler_pair(add_host_transaction_handler(context, nqn, host));
        }
    }

    try {
        transaction_handler->commit();
    }
    catch (const std::exception&) {
        transaction_handler->rollback();
        throw;
    }
}


void save_zone_to_db(const Zone& zone) {
    agent_framework::database::ZoneEntity db(zone.get_uuid());
    for (const auto& endpoint_uuid : get_m2m_manager<Zone, Endpoint>().get_children(zone.get_uuid())) {
        db.append(agent_framework::database::ZONE_ENDPOINTS_PROPERTY, endpoint_uuid);
    }
}

void add_zone(AddZone::ContextPtr context, const AddZone::Request& request, AddZone::Response& response) {
    log_info("spdk-agent", "Adding zone...");
    get_manager<Fabric>().get_entry(request.get_fabric());
    auto zone = agent_framework::discovery::ZoneBuilder::build_default(request.get_fabric());

    if (!request.get_endpoints().empty()) {
        check_endpoints_added_to_zone(request.get_endpoints());

        OptionalField<Uuid> initiator = find_initiator(request);
        if (initiator.has_value()) {
            add_nvmf_hosts(context, request.get_endpoints(), initiator);
        }

        for (const auto& endpoint_uuid : request.get_endpoints()) {
            get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
        }
    }

    save_zone_to_db(zone);
    response.set_zone(zone.get_uuid());
    get_manager<Zone>().add_entry(zone);
    log_info("spdk-agent", "Added zone with UUID '" + zone.get_uuid() + "'");

}
}

REGISTER_SPDK_COMMAND(AddZone, ::add_zone);