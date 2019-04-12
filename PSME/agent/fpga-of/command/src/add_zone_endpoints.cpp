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
#include "fpgaof_agent_commands.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "utils.hpp"



using namespace agent::fpgaof;
using namespace agent::fpgaof::utils;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::database;
using namespace opaepp;

namespace {

std::vector<Uuid> get_initiator_endpoints(const attribute::Array<Uuid>& endpoint_uuids) {
    std::vector<Uuid> initiator_endpoints_uuids;
    std::copy_if(std::begin(endpoint_uuids), std::end(endpoint_uuids),
                 std::back_inserter(initiator_endpoints_uuids),
                 [](const auto& uuid) { return Endpoint::is_initiator(uuid); });
    return initiator_endpoints_uuids;
}


void validate_endpoints_are_unbound(const attribute::Array<Uuid>& endpoint_uuids) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        auto state = get_manager<Endpoint>().get_entry(endpoint_uuid).get_status().get_state();
        if (state != enums::State::Enabled) {
            log_debug("fpgaof-gami", "Endpoint " + endpoint_uuid + " is in wrong state: " + state.to_string() + ".");
            THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami",
                  std::string("Endpoint is in wrong state: ") + state.to_string() + ".");
        }
        if (get_m2m_manager<Zone, Endpoint>().child_exists(endpoint_uuid)) {
            log_debug("fpgaof-gami", "Endpoint " + endpoint_uuid + " already assigned to a zone.");
            THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami",
                  "Endpoint already assigned to a zone.");
        }
    }
}


void validate_number_of_initiator_endpoints(const AddZoneEndpoints::Request& request) {
    auto initiator_endpoints_uuids = get_initiator_endpoints(request.get_endpoints());
    if (initiator_endpoints_uuids.size() > 1) {
        THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami",
              "Unable to add more than 1 Initiator Endpoint to the Zone");
    }
    else if (initiator_endpoints_uuids.size() == 1) {
        auto initiator_endpoints_in_zone_uuids = get_initiator_endpoints(
            get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone()));
        if (initiator_endpoints_in_zone_uuids.size() != 0) {
            THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami",
                  "Unable to add Initiator Endpoint to a Zone which already contains Initiator Endpoint");
        }
    }
}


void validate_duplicate_endpoints(const attribute::Array<Uuid>& endpoint_uuids) {
    std::set<Uuid> endpoints_set(endpoint_uuids.begin(), endpoint_uuids.end());
    if (endpoints_set.size() != endpoint_uuids.size()) {
        THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami", "Request contains duplicated endpoints.");
    }
}


void validate_request(const AddZoneEndpoints::Request& request) {
    validate_duplicate_endpoints(request.get_endpoints());
    validate_endpoints_are_unbound(request.get_endpoints());
    validate_number_of_initiator_endpoints(request);
}


void
bind_targets(DeleteZoneEndpoints::ContextPtr ctx, const attribute::Array<Uuid>& target_endpoint_uuids,
             const Uuid& host_uuid,
             const Uuid& zone_uuid) {
    log_debug("fpgaof-gami", "Binding " + std::to_string(int(target_endpoint_uuids.size())) +
                             " endpoints to zone: " << zone_uuid);

    for (const auto& endpoint_uuid : target_endpoint_uuids) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        for (const auto& connected_entity : endpoint.get_connected_entities()) {
            OpaeProxyHostApi::set_device_ownership(*ctx->opae_proxy_context, host_uuid,
                                                   connected_entity.get_entity());
            log_debug("opae-proxy", "opaepp : added endpoint " + endpoint.get_uuid() + " to opae proxy.");
        }
        log_debug("fpgaof-gami", "Endpoint attached to zone, endpoint uuid: " << endpoint_uuid);
    }
}


void add_endpoints_to_zone_model(const attribute::Array<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    log_debug("fpgaof-gami",
              "Add " + std::to_string(endpoint_uuids.size()) + " endpoints to zone model, zone uuid: "
                  << zone_uuid);
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);

    for (const auto& endpoint_uuid : endpoint_uuids) {

        get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, endpoint_uuid);

        log_debug("fpgaof-gami", "Endpoint added to zone model, endpoint uuid: " << endpoint_uuid);
    }
}


void add_endpoints_to_zone_db(const attribute::Array<Uuid>& endpoints_uuids,
                              const Uuid& zone_uuid) {

    log_debug("fpgaof-gami", "Store endpoints in zone db, zone uuid: " << zone_uuid);

    agent_framework::database::ZoneEntity zone_db{zone_uuid};

    std::for_each(endpoints_uuids.begin(), endpoints_uuids.end(),
                  [&](const Uuid& endpoint_uuid) {
                      zone_db.append(ZONE_ENDPOINTS_PROPERTY, endpoint_uuid);
                      log_debug("fpgaof-gami", "Endpoint stored in zone db, endpoint uuid: "
                          << endpoint_uuid);
                  });
}


void add_zone_endpoints(AddZoneEndpoints::ContextPtr ctx, const AddZoneEndpoints::Request& request,
                        AddZoneEndpoints::Response& /*response*/) {
    log_info("fpgaof-gami", "Adding endpoints to zone.");

    /* Check zone exists */
    Zone zone = get_manager<Zone>().get_entry(request.get_zone());

    validate_request(request);

    auto endpoints_to_add = request.get_endpoints().get_array();
    std::vector<Uuid> current_initiators, current_targets, initiators_to_add, targets_to_add, targets_to_bind;

    partition_target_and_initiator_endpoints(get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone()),
                                             current_targets, current_initiators);
    partition_target_and_initiator_endpoints(endpoints_to_add, targets_to_add, initiators_to_add);

    Uuid host_uuid;
    bool add_initiator = !initiators_to_add.empty();
    bool zone_has_initiator = !current_initiators.empty();

    if (add_initiator) {
        std::merge(std::begin(current_targets), std::end(current_targets), std::begin(targets_to_add),
                   std::end(targets_to_add), std::back_inserter(targets_to_bind));
        host_uuid = attribute::Identifier::get_uuid(get_manager<Endpoint>().get_entry(initiators_to_add.at(0)));
    }
    else if (zone_has_initiator) {
        targets_to_bind = targets_to_add;
        host_uuid = attribute::Identifier::get_uuid(get_manager<Endpoint>().get_entry(current_initiators.at(0)));
    }

    if (!targets_to_bind.empty() && !host_uuid.empty()) {
        bind_targets(ctx, targets_to_bind, host_uuid, zone.get_uuid());
    }
    add_endpoints_to_zone_model(request.get_endpoints(), zone.get_uuid());
    add_endpoints_to_zone_db(request.get_endpoints(), zone.get_uuid());

    log_info("fpgaof-agent", "addZoneEndpoints command completed.");
}

}
REGISTER_FPGAOF_COMMAND(AddZoneEndpoints, ::add_zone_endpoints);