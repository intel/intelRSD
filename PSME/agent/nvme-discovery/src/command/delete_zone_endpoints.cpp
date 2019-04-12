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
#include <sstream>
#include <stdexcept>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;
using namespace std;

namespace {

void throw_if_duplicates(const attribute::Array<Uuid>& endpoints) {
    if (tools::has_duplicates(endpoints)) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent",
              "Request contains duplicate endpoints");
    }
}

void validate_request(const DeleteZoneEndpoints::Request& req) {
    Uuid initiator{};
    const auto& endpoints = req.get_endpoints();
    throw_if_duplicates(endpoints);

    if (endpoints.empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "No endpoints in request");
    }

    for (const auto& endpoint : endpoints) {
        // check if endpoint exists in model
        auto endpoint_entry = get_manager<Endpoint>().get_entry(endpoint);
        // check if exists in M2M manager
        if (!get_m2m_manager<Zone, Endpoint>().entry_exists(req.get_zone(), endpoint)) {
            THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Endpoint is not in the zone");
        }
    }
}

void delete_zone_endpoints(DeleteZoneEndpoints::ContextPtr, const DeleteZoneEndpoints::Request& req, DeleteZoneEndpoints::Response&) {
    const Uuid& zone = req.get_zone();
    ZoneDatabase zone_db{zone};
    stringstream endpoints_uuids{};

    // check if zone exists
    get_manager<Zone>().get_entry(zone);

    // validate endpoints and get initiator if exists in the request
    validate_request(req);

    for (const auto& endpoint : req.get_endpoints()) {
        zone_db.del(endpoint);
        get_m2m_manager<Zone, Endpoint>().remove_entry(zone, endpoint);
        endpoints_uuids << "\n " << endpoint;
    }

    log_info("nvme-discovery-agent", "Removed endpoints from zone '" << zone
             << "', removed endpoints uuids:" << endpoints_uuids.str());
}
}

REGISTER_NVME_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);
