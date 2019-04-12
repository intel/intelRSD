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
* @file delete_zone_enpoints.cpp
*/


#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/module/common_components.hpp"
#include "fpgaof_agent_commands.hpp"
#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "utils.hpp"



using namespace agent::fpgaof;
using namespace agent::fpgaof::utils;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace opaepp;

namespace {

void validate_endpoints_are_in_zone(const std::vector<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (!get_m2m_manager<Zone, Endpoint>().entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami", "Endpoint not present in given zone.");
        }
    }
}


void validate_duplicate_endpoints(const std::vector<Uuid>& endpoint_uuids) {
    std::set<Uuid> endpoints_set(endpoint_uuids.begin(), endpoint_uuids.end());
    if (endpoints_set.size() != endpoint_uuids.size()) {
        THROW(agent_framework::exceptions::InvalidValue, "fpgaof-gami", "Request contains duplicated endpoints.");
    }
}


void validate_request(const DeleteZoneEndpoints::Request& request) {
    validate_duplicate_endpoints(request.get_endpoints().get_array());
    validate_endpoints_are_in_zone(request.get_endpoints().get_array(), request.get_zone());
}


void unbind_targets(DeleteZoneEndpoints::ContextPtr ctx, const std::vector<Uuid>& target_endpoint_uuids,
                    const Uuid& zone_uuid) {
    log_debug("fpgaof-gami", "Unbinding " + std::to_string(int(target_endpoint_uuids.size())) +
                             " endpoints from zone: " << zone_uuid);

    for (const auto& endpoint_uuid : target_endpoint_uuids) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        for (const auto& connected_entity : endpoint.get_connected_entities()) {
            OpaeProxyHostApi::remove_device_ownership(*ctx->opae_proxy_context, connected_entity.get_entity());
            log_debug("opae-proxy", "opaepp : removed endpoint " + endpoint.get_uuid() + " from opae proxy.");
        }
        log_debug("fpgaof-gami", "Endpoint detached from zone, endpoint uuid: " << endpoint_uuid);
    }
}


void remove_endpoints_from_zone_model(const std::vector<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    log_debug("fpgaof-gami",
              "Remove " + std::to_string(endpoint_uuids.size()) + " endpoints from zone model, zone uuid: "
                  << zone_uuid);
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);

    for (const auto& endpoint_uuid : endpoint_uuids) {

        get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);

        log_debug("fpgaof-gami", "Endpoint removed from zone model, endpoint uuid: " << endpoint_uuid);
    }
}


void remove_endpoints_from_zone_db(const std::vector<Uuid>& endpoints_uuids,
                                   const Uuid& zone_uuid) {

    log_debug("fpgaof-gami", "Remove endpoints from zone db, zone uuid: " << zone_uuid);

    agent_framework::database::ZoneEntity zone_db{zone_uuid};

    std::for_each(endpoints_uuids.begin(), endpoints_uuids.end(),
                  [&](const Uuid& endpoint_uuid) {
                      zone_db.del(endpoint_uuid);
                      log_debug("fpgaof-gami", "Endpoint removed from zone db, endpoint uuid: " << endpoint_uuid);
                  });
}


void delete_zone_endpoints(DeleteZoneEndpoints::ContextPtr ctx, const DeleteZoneEndpoints::Request& request,
                           DeleteZoneEndpoints::Response& /*resp*/) {
    log_info("fpgaof-gami", "Deleting endpoints from zone.");

    /* Check zone exists */
    Zone zone = get_manager<Zone>().get_entry(request.get_zone());

    validate_request(request);

    auto endpoints_to_delete = request.get_endpoints().get_array();
    std::vector<Uuid> current_initiators, current_targets, initiators_to_delete, targets_to_delete, targets_to_unbind;

    partition_target_and_initiator_endpoints(get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone()),
                                             current_targets, current_initiators);
    partition_target_and_initiator_endpoints(endpoints_to_delete, targets_to_delete, initiators_to_delete);

    bool delete_initiator = !initiators_to_delete.empty();
    bool zone_has_initiator = !current_initiators.empty();

    if (delete_initiator) { // unbind all targets in zone
        targets_to_unbind = current_targets;
    }
    else if (zone_has_initiator) {
        targets_to_unbind = targets_to_delete;
    }

    if (!targets_to_unbind.empty()) {
        unbind_targets(ctx, targets_to_unbind, request.get_zone());
    }
    remove_endpoints_from_zone_model(endpoints_to_delete, request.get_zone());
    remove_endpoints_from_zone_db(endpoints_to_delete, request.get_zone());

    log_info("fpgaof-agent", "deleteZoneEndpoints command completed.");
}
}

REGISTER_FPGAOF_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);