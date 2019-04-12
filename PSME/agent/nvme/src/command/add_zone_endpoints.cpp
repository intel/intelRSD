/*!
 * @brief Implementation of AddZoneEndpoints command.
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
 * @file add_zone_endpoints.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "loader/config.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"

#include <algorithm>
#include <set>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;

namespace {

void check_endpoint_in_zone(const Uuid& zone, const Uuid& endpoint) {
    auto endpoints = get_m2m_manager<Zone, Endpoint>().get_children(zone);
    if (find(endpoints.begin(), endpoints.end(), endpoint) != endpoints.end()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Endpoint is already in a zone");
    }
}

void check_endpoints(const AddZoneEndpoints::Request& req, bool& need_connect, bool& initiator_in_request) {
    unsigned initiators{0};
    for (const auto& uuid : req.get_endpoints()) {
        // check if endpoint exists
        auto endpoint = get_manager<Endpoint>().get_entry(uuid);
        // according to architects decisions an endpoint can only be in one zone
        // so here make sure endpoint is not already in a zone
        for (const auto& zone : get_manager<Zone>().get_keys()) {
            check_endpoint_in_zone(zone, uuid);
        }
        // increment number of initiators that will be added to the zone
        if (is_initiator(endpoint)) {
            initiator_in_request = true;
            initiators++;
        }
    }
    // check how many initiators are there in the zone already
    for (const auto& uuid : get_m2m_manager<Zone, Endpoint>().get_children(req.get_zone())) {
        auto endpoint = get_manager<Endpoint>().get_entry(uuid);
        if (is_initiator(endpoint)) {
            initiators++;
        }
    }
    if (initiators > 1) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Zone cannot have more then 1 initiator");
    }
    need_connect = (initiators == 1);
}

void add_endpoints_to_zone(const AddZoneEndpoints::Request& req) {
    auto& manager = get_m2m_manager<Zone, Endpoint>();
    for (const auto& uuid : req.get_endpoints()) {
        manager.add_entry(req.get_zone(), uuid);
    }
}

void set_filter(AddZoneEndpoints::ContextPtr ctx, const AddZoneEndpoints::Request& req, bool initiator_in_request) {
    const auto& req_endpoints{req.get_endpoints()};
    std::vector<Uuid> endpoints{req_endpoints.begin(), req_endpoints.end()};
    const auto& zone_endpoints{get_m2m_manager<Zone, Endpoint>().get_children(req.get_zone())};
    if (initiator_in_request) {
        // adding initiator from the request, need to also connect to targets that are already in the zone
        std::copy(std::begin(zone_endpoints), std::end(zone_endpoints), std::back_inserter(endpoints));
    } else {
        // find the initiator from the zone
        for (const auto& endpoint : zone_endpoints) {
            const auto& endpoint_entry = get_manager<Endpoint>().get_entry(endpoint);
            if (tools::is_initiator(endpoint_entry)) {
                endpoints.push_back(endpoint);
            }
        }
    }

    // remove duplicates
    std::sort(std::begin(endpoints), std::end(endpoints));
    auto last = std::unique(std::begin(endpoints), std::end(endpoints));
    endpoints.erase(last, std::end(endpoints));
    tools::set_initiator_filter(ctx, endpoints);
}

void add_zone_endpoints(AddZoneEndpoints::ContextPtr ctx, const AddZoneEndpoints::Request& req, AddZoneEndpoints::Response&) {
    log_debug("nvme-agent", "Adding zone endpoint.");
    get_manager<Zone>().get_entry(req.get_zone());
    bool need_connect{false};
    bool initiator_in_request{false};
    check_endpoints(req, need_connect, initiator_in_request);
    // add to zone database
    ZoneDatabase zone_db{req.get_zone()};
    const auto& endpoints{req.get_endpoints()};
    if (need_connect) {
        set_filter(ctx, req, initiator_in_request);
    }
    for_each(endpoints.begin(), endpoints.end(),
             [&zone_db](const Uuid& endpoint) {zone_db.append(NvmeDatabase::ENDPOINTS, endpoint);});

    add_endpoints_to_zone(req);
    log_debug("nvme-agent", "Added endpoints to zone");
}
}

REGISTER_NVME_COMMAND(AddZoneEndpoints, ::add_zone_endpoints);
