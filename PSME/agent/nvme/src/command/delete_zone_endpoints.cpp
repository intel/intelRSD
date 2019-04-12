/*!
 * @brief Implementation of DeleteZoneEndpoints command.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file delete_zone_endpoints.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "loader/config.hpp"
#include "tools/tools.hpp"
#include "tools/databases.hpp"

#include <cstdint>
#include <string>
#include <stdexcept>
#include <tuple>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;
using namespace std;

namespace {

Uuid validate_and_get_initiator(const DeleteZoneEndpoints::Request& req) {
    Uuid initiator{};
    const auto& endpoints = req.get_endpoints();

    if (endpoints.empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "No endpoints in request");
    }

    for (const auto& endpoint : endpoints) {
        // check if endpoint exists in model
        auto endpoint_entry = get_manager<Endpoint>().get_entry(endpoint);
        // check if exists in M2M manager
        if (!get_m2m_manager<Zone, Endpoint>().entry_exists(req.get_zone(), endpoint)) {
            THROW(exceptions::InvalidValue, "nvme-agent", "Endpoint is not in the zone");
        }
        if (is_initiator(endpoint_entry)) {
            initiator = endpoint;
        }
    }
    return initiator;
}

std::tuple<bool, Endpoint> get_zone_initiator(const Uuid& zone) {
    for (const auto& endpoint : get_m2m_manager<Zone, Endpoint>().get_children(zone)) {
        auto endpoint_entry = get_manager<Endpoint>().get_entry(endpoint);
        if (is_initiator(endpoint_entry)) {
            return std::make_tuple(true, endpoint_entry);
        }
    }
    return std::make_tuple(false, Endpoint{});
}

void delete_zone_endpoints(DeleteZoneEndpoints::ContextPtr ctx, const DeleteZoneEndpoints::Request& req, DeleteZoneEndpoints::Response&) {
    const Uuid& zone = req.get_zone();
    bool initiator_in_zone{false};

    // check if zone exists
    get_manager<Zone>().get_entry(zone);

    // validate endpoints and get initiator if exists in the request
    Uuid initiator_to_remove = validate_and_get_initiator(req);

    Endpoint initiator{};
    // check if there is initiator in the zone so that we know that we need to disconnect from targets
    std::tie(initiator_in_zone, initiator) = get_zone_initiator(req.get_zone());

    if (!initiator_to_remove.empty()) {
        // removing initiator from the zone
        tools::clear_initiator_filter(ctx, req.get_zone());
    } else if (initiator_in_zone) {
        // removing endpoints from a zone with an initiator
        // it is necessary to include the initiator in the endpoints array to clear the filter
        auto endpoints_with_initiator = req.get_endpoints();
        endpoints_with_initiator.add_entry(initiator.get_uuid());
        tools::clear_initiator_filter(ctx, endpoints_with_initiator);
    }

    ZoneDatabase zone_db{zone};
    const auto& endpoints = req.get_endpoints();
    for (const auto& endpoint : endpoints) {
        log_debug("nvme-agent", "Removing endpoint '" + endpoint + "'");
        zone_db.del(endpoint);
        get_m2m_manager<Zone, Endpoint>().remove_entry(zone, endpoint);
    }

    log_debug("nvme-agent", "Removed endpoints from zone '" + zone + "'");
}
}

REGISTER_NVME_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);
