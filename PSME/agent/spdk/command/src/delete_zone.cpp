/*!
* @brief Implementation of DeleteZone command.
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
* @file delete_zone.cpp
*/

#include "spdk_agent_commands.hpp"
#include "spdk_converter.hpp"

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entities.hpp"



using namespace agent::spdk;
using namespace agent_framework::model;
using namespace agent_framework;

namespace {

OptionalField<Endpoint> find_initiator(const std::vector<Uuid>& endpoints) {
    for (const auto& endpoint_uuid : endpoints) {
        auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);
        if (Endpoint::is_initiator(endpoint)) {
            return endpoint;
        }
    }
    return OptionalField<Endpoint>{};
}

void remove_endpoint_from_zone_db(const Uuid& zone, const Uuid& endpoint) {
    agent_framework::database::ZoneEntity zone_db{zone};
    log_debug("spdk-agent", "Removing endpoint '" + endpoint + "' from DB");
    zone_db.del(endpoint);
}


::utils::transaction::BaseTransactionHandler::FunctionPair
    remove_host_transaction_handler(AgentContext::SPtr context, const Uuid& zone_uuid, const Uuid& target_uuid,
        const std::string& subsystem_nqn, const ::spdk::model::Host& host) {

    auto to_commit = [context, subsystem_nqn, host, zone_uuid, target_uuid]() {
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

        remove_endpoint_from_zone_db(zone_uuid, target_uuid);
        module::get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, target_uuid);
    };

    auto to_rollback = [subsystem_nqn, host]() {
        log_debug("spdk-agent", "Rollback NVMf subsystem remove host operation for NQN("
           << subsystem_nqn << ") Host(" << host.get_nqn() << ")");
    };

    return std::make_pair(to_commit, to_rollback);
}


void delete_zone(DeleteZone::ContextPtr context, const DeleteZone::Request& request,
                 DeleteZone::Response&) {

    log_info("spdk-agent", "Deleting zone...");
    const auto& zone_uuid = request.get_zone();
    module::get_manager<Zone>().get_entry(zone_uuid);
    auto endpoints = module::get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid);

    {
        auto initiator = find_initiator(endpoints);
        auto transaction_handler = context->transaction_handler_factory->get_handler();

        std::lock_guard<std::mutex> lock{context->spdk_mutex};
        for (const auto& uuid : endpoints) {
            auto endpoint = module::get_manager<Endpoint>().get_entry(uuid);
            if (Endpoint::is_target(endpoint) && initiator.has_value()) {
                auto nqn = agent_framework::model::attribute::Identifier::get_nqn(endpoint);
                auto host = agent::spdk::utils::SpdkConverter::convert_initiator_to_host(initiator);
                transaction_handler->add_handler_pair(
                    remove_host_transaction_handler(context, zone_uuid, uuid, nqn, host));
            }
        }

        try {
            transaction_handler->commit();
        }
        catch (const std::exception&) {
            transaction_handler->rollback();
            throw;
        }

        // We assume that initiator is only one.
        // So we can remove initiator at the end when all of targets are disconnected.
        if (initiator.has_value()) {
            remove_endpoint_from_zone_db(zone_uuid, initiator->get_uuid());
            module::get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, initiator->get_uuid());
        }
    }

    agent_framework::database::ZoneEntity::remove(zone_uuid);
    module::get_m2m_manager<Zone, Endpoint>().remove_parent(zone_uuid);
    module::get_manager<Zone>().remove_entry(zone_uuid);
    log_info("spdk-agent", "Zone " << zone_uuid << " removed successfully");
}
}

REGISTER_SPDK_COMMAND(DeleteZone, ::delete_zone);

