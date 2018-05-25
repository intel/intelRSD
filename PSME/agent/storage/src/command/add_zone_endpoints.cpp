/*!
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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
 * @file command/add_zone_endpoints.cpp
 * @brief AddZoneEndpoints implementation
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "iscsi/utils.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework;
using namespace database;

namespace {

void store_persistent_attributes(const Uuid& zone_uuid, const AddZoneEndpoints::Request& req) {
    auto& pa = PersistentAttributes::get_instance();
    Aggregate<PersistentAttributes> aggr_zone{pa, zone_uuid, literals::Zone::ZONE};
    for (const Uuid& endpoint_uuid : req.get_endpoints()) {
        aggr_zone.append(literals::Endpoint::ENDPOINT, endpoint_uuid);
    }
    aggr_zone.dump();
}


void create_iscsi_targets(const attribute::Array<Uuid>& endpoints_uuids,
                          const Uuid& zone_uuid,
                          bool initiator_added = false) {
    auto& zone_endpoint_manager = get_m2m_manager<Zone, Endpoint>();
    try {
        if (initiator_added) {
            agent::storage::iscsi::tgt::create_iscsi_targets_from_endpoints(
                zone_endpoint_manager.get_children(zone_uuid));
        }
        else {
            agent::storage::iscsi::tgt::create_iscsi_targets_from_endpoints(endpoints_uuids);
        }
    }
    catch (const exceptions::InvalidValue&) {
        log_error("storage-agent", "Could not create iSCSI Target for zone: " + zone_uuid);
        if (initiator_added) {
            agent::storage::iscsi::tgt::remove_iscsi_targets_from_endpoints(
                zone_endpoint_manager.get_children(zone_uuid));
        }
        else {
            agent::storage::iscsi::tgt::remove_iscsi_targets_from_endpoints(endpoints_uuids);
        }
        for (const Uuid& endpoint_uuid : endpoints_uuids) {
            zone_endpoint_manager.remove_entry(zone_uuid, endpoint_uuid);
        }
        throw;
    }
}


void add_zone_endpoints(const AddZoneEndpoints::Request& req, AddZoneEndpoints::Response&) {
    log_info("storage-agent", "Adding endpoints to zone.");

    auto& zone_endpoint_manager = get_m2m_manager<Zone, Endpoint>();

    // check if zone exists
    get_manager<Zone>().get_entry(req.get_zone());
    bool initiator_in_zone = agent::storage::iscsi::tgt::is_role_present(
        zone_endpoint_manager.get_children(req.get_zone()), enums::EntityRole::Initiator);
    bool initiator_in_request = agent::storage::iscsi::tgt::is_role_present(req.get_endpoints(), enums::EntityRole::Initiator);

    if (initiator_in_request && initiator_in_zone) {
        THROW(exceptions::InvalidValue, "storage-agent", "Zone must not have multiple initiator endpoints");
    }

    agent::storage::iscsi::tgt::check_endpoints_added_to_zone(req.get_endpoints());
    for (const Uuid& endpoint_uuid : req.get_endpoints()) {
        zone_endpoint_manager.add_entry(req.get_zone(), endpoint_uuid);
    }

    // If there is no initiator endpoint, then do not create any iSCSI Target
    if (initiator_in_zone) { // Create iSCSI Targets only from endpoints from the request
        create_iscsi_targets(req.get_endpoints(), req.get_zone());
    }
    else if (initiator_in_request) { // Create iSCSI Targets from target endpoints from the entire zone
        create_iscsi_targets(req.get_endpoints(), req.get_zone(), true);
    }

    store_persistent_attributes(req.get_zone(), req);

    log_debug("storage-agent", "Added endpoints to zone with UUID '" + req.get_zone() + "'");
}
}

REGISTER_COMMAND(AddZoneEndpoints, ::add_zone_endpoints);
