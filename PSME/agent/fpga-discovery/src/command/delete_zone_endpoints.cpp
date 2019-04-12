/*!
* @brief Implementation of DeleteZoneEndpoints command.
*
* @copyright Copyright (c) 2019 Intel Corporation
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



#include "agent-framework/module/common_components.hpp"
#include "fpga_agent_commands.hpp"
#include "tools/databases.hpp"



using namespace agent::fpga_discovery;
using namespace agent::fpga_discovery::tools;
using namespace agent_framework::model;
using namespace agent_framework::module;

namespace {

void validate_endpoints_are_in_zone(const std::vector<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (!get_m2m_manager<Zone, Endpoint>().entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "fpga-discovery-agent",
                  "Endpoint not present in given zone.");
        }
    }
}


void validate_duplicate_endpoints(const std::vector<Uuid>& endpoint_uuids) {
    std::set<Uuid> endpoints_set(endpoint_uuids.begin(), endpoint_uuids.end());
    if (endpoints_set.size() != endpoint_uuids.size()) {
        THROW(agent_framework::exceptions::InvalidValue, "fpga-discovery-agent",
              "Request contains duplicated endpoints.");
    }
}


void validate_request(const DeleteZoneEndpoints::Request& request) {
    validate_duplicate_endpoints(request.get_endpoints().get_array());
    validate_endpoints_are_in_zone(request.get_endpoints().get_array(), request.get_zone());
}


void remove_endpoints_from_zone_model(const std::vector<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    log_debug("fpga-discovery-agent",
              "Remove " + std::to_string(endpoint_uuids.size()) + " endpoints from zone model, zone uuid: "
                  << zone_uuid);
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);

    for (const auto& endpoint_uuid : endpoint_uuids) {

        get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);

        log_debug("fpga-discovery-agent", "Endpoint removed from zone model, endpoint uuid: " << endpoint_uuid);
    }
}


void remove_endpoints_from_zone_db(const std::vector<Uuid>& endpoints_uuids,
                                   const Uuid& zone_uuid) {

    log_debug("fpga-discovery-agent", "Remove endpoints from zone db, zone uuid: " << zone_uuid);

    ZoneDatabase zone_db{zone_uuid};

    std::for_each(endpoints_uuids.begin(), endpoints_uuids.end(),
                  [&](const Uuid& endpoint_uuid) {
                      zone_db.del(endpoint_uuid);
                      log_debug("fpga-discovery-agent",
                                "Endpoint removed from zone db, endpoint uuid: " << endpoint_uuid);
                  });
}


void delete_zone_endpoints(DeleteZoneEndpoints::ContextPtr, const DeleteZoneEndpoints::Request& request,
                           DeleteZoneEndpoints::Response&) {
    log_info("fpga-discovery-agent", "Deleting endpoints from zone.");

    /* Check zone exists */
    Zone zone = get_manager<Zone>().get_entry(request.get_zone());

    validate_request(request);

    remove_endpoints_from_zone_model(request.get_endpoints().get_array(), request.get_zone());
    remove_endpoints_from_zone_db(request.get_endpoints().get_array(), request.get_zone());

    log_info("fpga-discovery-agent", "deleteZoneEndpoints command completed.");
}
}

REGISTER_FPGA_DISCOVERY_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints
);