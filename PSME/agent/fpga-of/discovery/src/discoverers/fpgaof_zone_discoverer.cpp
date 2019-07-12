/*!
 * @brief Zone discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file fpgaof_zone_discoverer.cpp
 */

#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/discovery/builders/zone_builder.hpp"
#include "discovery/discoverers/fpgaof_zone_discoverer.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "utils.hpp"



using namespace agent::fpgaof::discovery;
using namespace agent::fpgaof::utils;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent_framework::database;
using namespace agent::fpgaof;
using namespace opaepp;

namespace {

void add_endpoint_to_zone(const Uuid& zone, const Uuid& endpoint) {
    if (!get_manager<Endpoint>().entry_exists(endpoint)) {
        log_error("fpgaof-discovery", "No endpoint found in manager with given UUID '" + endpoint + "'");
        return;
    }
    get_m2m_manager<Zone, Endpoint>().add_entry(zone, endpoint);
}


void load_endpoints_from_zone_database(const Uuid& zone) {
    ZoneEntity zone_db(zone);
    auto endpoints{zone_db.get_multiple_values(ZONE_ENDPOINTS_PROPERTY)};
    std::for_each(endpoints.begin(), endpoints.end(),
                  [&zone](const std::string& endpoint) { add_endpoint_to_zone(zone, endpoint); });
}


void add_zone(Zone& zone, const Uuid& zone_uuid) {
    zone.set_uuid(zone_uuid);
    zone.set_status(attribute::Status(State::Enabled, Health::OK));
    zone.add_collection(attribute::Collection(CollectionName::Endpoints, CollectionType::Endpoints));
    load_endpoints_from_zone_database(zone_uuid);
}


void bind_endpoints_in_zone(AgentContext::SPtr ctx, const Zone& zone) {
    auto endpoint_uuids = get_m2m_manager<Zone, Endpoint>().get_children(zone.get_uuid());
    std::vector<Uuid> target_endpoint_uuids, initiator_endpoint_uuids;
    partition_target_and_initiator_endpoints(endpoint_uuids, target_endpoint_uuids, initiator_endpoint_uuids);

    if (!target_endpoint_uuids.empty() && !initiator_endpoint_uuids.empty()) {
        auto host_uuid = attribute::Identifier::get_uuid(
            get_manager<Endpoint>().get_entry(initiator_endpoint_uuids.front()));
        for (const auto& target : target_endpoint_uuids) {
            auto device_uuid = get_manager<Endpoint>().get_entry(
                target).get_connected_entities().front().get_entity().value();
            OpaeProxyHostApi::set_device_ownership(*ctx->opae_proxy_context, host_uuid, device_uuid);
            log_info("opae-proxy", "Host '" + host_uuid + "' became the owner of device '" + device_uuid + "'");
        }
    }
}

}


std::vector<agent_framework::model::Zone> FpgaofZoneDiscoverer::discover(const Uuid& fabric_uuid) {
    std::vector<Zone> zones{};

    auto zone_uuids = FabricEntity(fabric_uuid).get_multiple_values(ZONES_PROPERTY);

    for (const auto& zone_uuid : zone_uuids) {
        Zone zone{fabric_uuid};
        log_info("fpgaof-discovery", "Discovering zone " + zone_uuid + " from database");
        try {
            add_zone(zone, zone_uuid);
            bind_endpoints_in_zone(m_context, zone);
            zones.push_back(zone);
        }
        catch (const std::exception& e) {
            log_error("fpgaof-discovery", "Unable to read zone " + zone_uuid + " from database. " + e.what());
        }
    }
    return zones;
}
