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
 * @file command/delete_zone_endpoints.cpp
 * @brief DeleteZoneEndpoints implementation
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

void delete_persistent_attributes(const Uuid& zone_uuid, const DeleteZoneEndpoints::Request& req) {
    auto& pa = PersistentAttributes::get_instance();
    try {
        Aggregate<PersistentAttributes> aggr_zone{pa, zone_uuid, literals::Zone::ZONE};
        for (const Uuid& endpoint_uuid : req.get_endpoints()) {
            aggr_zone.del(endpoint_uuid);
        }
        aggr_zone.dump();
    }
    catch (const std::out_of_range& e) {
        log_warning("storage-agent", "Could not read delete endpoint from persistent attributes: " << e.what());
    }
}


void remove_iscsi_targets(const attribute::Array<Uuid>& endpoints_uuids, const Uuid& zone_uuid,
                          bool initiator_removed = false) {
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    try {
        if (initiator_removed) {
            agent::storage::iscsi::tgt::remove_iscsi_targets_from_endpoints(
                zone_endpoint_manager.get_children(zone_uuid));
        }
        else {
            agent::storage::iscsi::tgt::remove_iscsi_targets_from_endpoints(endpoints_uuids);
        }
    }
    catch (const exceptions::InvalidValue&) {
        log_debug("storage-agent",
                  "Error while removing iSCSI Targets zone: " + zone_uuid + ", verify TGT daemon status");
        for (const Uuid& endpoint_uuid : endpoints_uuids) {
            zone_endpoint_manager.add_entry(zone_uuid, endpoint_uuid);
        }
        throw;
    }
}


void check_endpoints_in_zone(const attribute::Array<Uuid>& endpoints_uuids, const Uuid& zone_uuid) {
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    for (const auto& endpoint_uuid : endpoints_uuids) {
        if (!zone_endpoint_manager.entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(exceptions::InvalidValue, "storage-agent", "Zone does not contain provided endpoint");
        }
    }
}


void delete_zone_endpoints(const DeleteZoneEndpoints::Request& req, DeleteZoneEndpoints::Response&) {
    log_info("storage-agent", "Deleting endpoints from zone.");

    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();

    // check if zone exists
    get_manager<Zone>().get_entry(req.get_zone());
    bool initiator_in_zone = agent::storage::iscsi::tgt::is_role_present(
        zone_endpoint_manager.get_children(req.get_zone()), enums::EntityRole::Initiator);
    bool initiator_in_request = agent::storage::iscsi::tgt::is_role_present(req.get_endpoints(), enums::EntityRole::Initiator);

    check_endpoints_in_zone(req.get_endpoints(), req.get_zone());

    // If there is no initiator endpoint, then do not remove any iSCSI Target
    if (initiator_in_request && initiator_in_zone) { // Remove iSCSI Targets from target endpoints from the entire zone
        remove_iscsi_targets(req.get_endpoints(), req.get_zone(), true);
    }
    else if (initiator_in_zone) { // Remove iSCSI Targets only from endpoints from the request
        remove_iscsi_targets(req.get_endpoints(), req.get_zone());
    }

    for (const Uuid& endpoint_uuid : req.get_endpoints()) {
        zone_endpoint_manager.remove_entry(req.get_zone(), endpoint_uuid);
    }
    delete_persistent_attributes(req.get_zone(), req);

    log_debug("storage-agent", "Deleted endpoints from zone with UUID '" + req.get_zone() + "'");
}
}

REGISTER_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);
