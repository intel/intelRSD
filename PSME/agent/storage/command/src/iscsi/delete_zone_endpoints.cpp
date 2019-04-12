/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file command/iscsi/delete_zone_endpoints.cpp
 * @brief DeleteZoneEndpoints implementation
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/database/database_entity.hpp"
#include "storage_agent_commands.hpp"
#include "agent/utils/iscsi/utils.hpp"



using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::storage;

namespace {

void delete_persistent_attributes(const Uuid& zone_uuid, const DeleteZoneEndpoints::Request& req) {
    auto& pa = database::PersistentAttributes::get_instance();
    try {
        database::Aggregate<database::PersistentAttributes> aggr_zone{pa, zone_uuid, literals::Zone::ZONE};
        for (const Uuid& endpoint_uuid : req.get_endpoints()) {
            aggr_zone.del(endpoint_uuid);
        }
        aggr_zone.dump();
    }
    catch (const std::out_of_range& e) {
        log_warning("storage-agent", "Could not read delete endpoint from persistent attributes: " << e.what());
    }
}


void remove_iscsi_targets(AgentContext::SPtr context,
                          const attribute::Array<Uuid>& endpoints_uuids,
                          const Uuid& zone_uuid,
                          bool initiator_removed = false) {
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    try {
        if (initiator_removed) {
            agent::storage::utils::remove_iscsi_targets_from_endpoints(
                context, zone_endpoint_manager.get_children(zone_uuid));
        }
        else {
            agent::storage::utils::remove_iscsi_targets_from_endpoints(context, endpoints_uuids);
        }
    }
    catch (const agent_framework::exceptions::InvalidValue&) {
        log_debug("storage-agent",
                  "Error while removing iSCSI Targets zone: " + zone_uuid + ", verify TGT daemon status");
        for (const Uuid& endpoint_uuid : endpoints_uuids) {
            zone_endpoint_manager.add_entry(zone_uuid, endpoint_uuid);
        }
        throw;
    }
}


void check_endpoints_in_zone(const attribute::Array<Uuid>& endpoints_uuids, const Uuid& zone_uuid) {
    for (const auto& endpoint_uuid : endpoints_uuids) {
        if (!get_m2m_manager<Zone, Endpoint>().entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "Zone does not contain provided endpoint");
        }
    }
}


void delete_zone_endpoints(DeleteZoneEndpoints::ContextPtr context,
                           const DeleteZoneEndpoints::Request& request,
                           DeleteZoneEndpoints::Response&) {
    log_info("storage-agent", "Deleting endpoints from zone...");

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    // check if zone exists
    get_manager<Zone>().get_entry(request.get_zone());
    bool initiator_in_zone = agent::storage::utils::is_role_present(
        zone_endpoint_manager.get_children(request.get_zone()), enums::EntityRole::Initiator);
    bool initiator_in_request = agent::storage::utils::is_role_present(
        request.get_endpoints(), enums::EntityRole::Initiator);

    check_endpoints_in_zone(request.get_endpoints(), request.get_zone());

    // If there is no initiator endpoint, then do not remove any iSCSI Target
    if (initiator_in_request && initiator_in_zone) { // Remove iSCSI Targets from target endpoints from the entire zone
        remove_iscsi_targets(context, request.get_endpoints(), request.get_zone(), true);
    }
    else if (initiator_in_zone) { // Remove iSCSI Targets only from endpoints from the request
        remove_iscsi_targets(context, request.get_endpoints(), request.get_zone());
    }

    for (const Uuid& endpoint_uuid : request.get_endpoints()) {
        zone_endpoint_manager.remove_entry(request.get_zone(), endpoint_uuid);
    }

    delete_persistent_attributes(request.get_zone(), request);
    log_info("storage-agent", "Deleted endpoints from zone with UUID '" + request.get_zone() + "'");
}
}

REGISTER_STORAGE_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);
