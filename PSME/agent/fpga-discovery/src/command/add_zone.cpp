/*!
 * @brief Implementation of AddZone command.
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
 * @file add_zone.cpp
 */

#include "fpga_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/discovery/builders/zone_builder.hpp"
#include "tools/databases.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::fpga_discovery;
using namespace agent::fpga_discovery::tools;

namespace {

void throw_if_duplicates(const attribute::Array<Uuid>& endpoints) {
    std::set<Uuid> endpoints_set(endpoints.begin(), endpoints.end());
    if (endpoints_set.size() != endpoints.size()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Cannot create zone with duplicate endpoints.");
    }
}


void throw_if_not_exactly_one_connected_entity(const Endpoint& endpoint) {
    if (endpoint.get_connected_entities().size() != 1) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Exactly one connected entity needs to be given in endpoint.");
    }

}


void throw_if_ip_transport_details_empty(const Endpoint& endpoint) {
    if (enums::EntityRole::Target == endpoint.get_connected_entities().front().get_entity_role()) {
        if (endpoint.get_ip_transport_details().empty()) {
            THROW(exceptions::InvalidValue, "fpga-discovery-agent",
                  "No transport details found in transport.");
        }
    }
}


void throw_if_more_than_one_initiator(const Endpoint& endpoint, uint16_t& initiator_counter) {
    if (enums::EntityRole::Initiator == endpoint.get_connected_entities().front().get_entity_role()) {
        initiator_counter++;
        if (1 < initiator_counter) {
            THROW(exceptions::InvalidValue, "fpga-discovery-agent",
                  "Zone cannot have more than one initiator endpoint.");
        }
    }
}


void throw_if_uuid_not_present(const Endpoint& endpoint) {
    std::string uuid{};

    try {
        uuid = attribute::Identifier::get_uuid(endpoint);
    }
    catch (const std::logic_error&) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Unable to read uuid from endpoint.");
    }

    if (uuid.empty()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "UUID identifier was not found or is empty.");
    }
}


void throw_if_endpoint_already_in_zone(const Endpoint& endpoint) {
    auto& manager = get_m2m_manager<Zone, Endpoint>();
    if (!manager.get_parents(endpoint.get_uuid()).empty()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Endpoint is already in a zone.");
    }
}


void validate_zone_endpoints(const attribute::Array<Uuid>& endpoints) {
    throw_if_duplicates(endpoints);
    uint16_t initiator_counter{0};
    for (const auto& endpoint_uuid : endpoints) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        throw_if_not_exactly_one_connected_entity(endpoint);
        throw_if_more_than_one_initiator(endpoint, initiator_counter);
        throw_if_uuid_not_present(endpoint);
        if (Endpoint::is_target(endpoint)) {
            throw_if_ip_transport_details_empty(endpoint);
        }
        throw_if_endpoint_already_in_zone(endpoint);
    }
}


void add_endpoints_to_zone(const AddZone::Request& req, const Uuid& zone_uuid) {
    std::stringstream endpoints_uuids{};
    auto& manager = get_m2m_manager<Zone, Endpoint>();
    for (const auto& uuid : req.get_endpoints()) {
        manager.add_entry(zone_uuid, uuid);
        endpoints_uuids << "\n " << uuid;
    }
    log_info("fpga-discovery-agent", "Added endpoints to zone '" << zone_uuid
                                                                 << "', added endpoints uuids:"
                                                                 << endpoints_uuids.str());
}


void store_to_database(const Uuid& zone, const AddZone::Request& req) {
    FabricDatabase(get_manager<Fabric>().get_keys().front()).append(FpgaDiscoveryDatabase::ZONES, zone);
    ZoneDatabase zone_db(zone);
    const auto& endpoints = req.get_endpoints();
    for_each(endpoints.begin(), endpoints.end(),
             [&zone_db](const Uuid& endpoint) { zone_db.append(FpgaDiscoveryDatabase::ENDPOINTS, endpoint); });
}


void add_zone(AddZone::ContextPtr /*ctx*/, const AddZone::Request& req, AddZone::Response& rsp) {

    get_manager<Fabric>().get_entry(req.get_fabric());
    Zone zone = agent_framework::discovery::ZoneBuilder::build_default(req.get_fabric());

    validate_zone_endpoints(req.get_endpoints());

    rsp.set_zone(zone.get_uuid());
    get_manager<Zone>().add_entry(zone);
    add_endpoints_to_zone(req, zone.get_uuid());
    store_to_database(zone.get_uuid(), req);
    log_info("fpga-discovery-agent", "Added zone with UUID '" + zone.get_uuid() + "'");
}
}

REGISTER_FPGA_DISCOVERY_COMMAND(AddZone, ::add_zone);
