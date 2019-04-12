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
 * @file command/iscis/add_zone_endpoints.cpp
 * @brief AddZoneEndpoints implementation
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/database/database_entity.hpp"
#include "storage_agent_commands.hpp"
#include "agent/utils/iscsi/utils.hpp"


using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::storage;

namespace {

void store_persistent_attributes(const Uuid& zone_uuid, const AddZoneEndpoints::Request& req) {
    auto& pa = database::PersistentAttributes::get_instance();
    database::Aggregate<database::PersistentAttributes> aggr_zone{pa, zone_uuid, literals::Zone::ZONE};
    for (const Uuid& endpoint_uuid : req.get_endpoints()) {
        aggr_zone.append(literals::Endpoint::ENDPOINT, endpoint_uuid);
    }
    aggr_zone.dump();
}


void create_iscsi_targets(AgentContext::SPtr context,
                          const attribute::Array<Uuid>& endpoints_uuids,
                          const Uuid& zone_uuid,
                          bool initiator_added = false) {

    try {
        if (initiator_added) {
            agent::storage::utils::create_iscsi_targets_from_endpoints(
                context, get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid));
        }
        else {
            agent::storage::utils::create_iscsi_targets_from_endpoints(context, endpoints_uuids);
        }
    }
    catch (const agent_framework::exceptions::InvalidValue&) {
        log_error("storage-agent", "Could not create iSCSI Target for zone: " + zone_uuid);
        if (initiator_added) {
            agent::storage::utils::remove_iscsi_targets_from_endpoints(
                context, get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid));
        }
        else {
            agent::storage::utils::remove_iscsi_targets_from_endpoints(context, endpoints_uuids);
        }
        for (const Uuid& endpoint_uuid : endpoints_uuids) {
            get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);
        }
        throw;
    }
}


void add_zone_endpoints(AddZoneEndpoints::ContextPtr context,
                        const AddZoneEndpoints::Request& request,
                        AddZoneEndpoints::Response&) {
    log_info("storage-agent", "Adding endpoints to zone.");

    auto& zone_endpoint_manager = get_m2m_manager<Zone, Endpoint>();

    // check if zone exists
    get_manager<Zone>().get_entry(request.get_zone());
    bool initiator_in_zone = agent::storage::utils::is_role_present(
        zone_endpoint_manager.get_children(request.get_zone()), enums::EntityRole::Initiator);
    bool initiator_in_request = agent::storage::utils::is_role_present(
        request.get_endpoints(), enums::EntityRole::Initiator);

    if (initiator_in_request && initiator_in_zone) {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "Zone must not have multiple initiator endpoints");
    }

    agent::storage::utils::check_endpoints_added_to_zone(context, request.get_endpoints());
    for (const Uuid& endpoint_uuid : request.get_endpoints()) {
        zone_endpoint_manager.add_entry(request.get_zone(), endpoint_uuid);
    }

    // If there is no initiator endpoint, then do not create any iSCSI Target
    if (initiator_in_zone) { // Create iSCSI Targets only from endpoints from the request
        create_iscsi_targets(context, request.get_endpoints(), request.get_zone());
    }
    else if (initiator_in_request) { // Create iSCSI Targets from target endpoints from the entire zone
        create_iscsi_targets(context, request.get_endpoints(), request.get_zone(), true);
    }

    store_persistent_attributes(request.get_zone(), request);

    log_info("storage-agent", "Added endpoints to zone with UUID '" + request.get_zone() + "'");
}
}

REGISTER_STORAGE_COMMAND(AddZoneEndpoints, ::add_zone_endpoints);
