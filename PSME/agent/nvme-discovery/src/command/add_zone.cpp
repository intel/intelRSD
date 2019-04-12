/*!
 * @brief Implementation of AddZone command.
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
 * @file add_zone.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/pnc_components.hpp"

#include "loader/config.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <set>

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::nvme::tools;
using namespace agent::nvme::loader;

namespace {

void throw_if_duplicates(const attribute::Array<Uuid>& endpoints) {
    if (tools::has_duplicates(endpoints)) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent",
              "Cannot create zone with duplicate endpoints.");
    }
}

void throw_if_connected_entities_empty(const Endpoint& endpoint) {
    if (endpoint.get_connected_entities().empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent",
              "No connected entity found in endpoint.");
    }

}

void throw_if_ip_transport_details_empty(const Endpoint& endpoint) {
    if (enums::EntityRole::Target == endpoint.get_connected_entities().front().get_entity_role()) {
        if (endpoint.get_ip_transport_details().empty()) {
            THROW(exceptions::InvalidValue, "nvme-discovery-agent",
                  "No transport details found in transport.");
        }
    }
}

void throw_if_more_than_one_initiator(const Endpoint& endpoint, uint16_t& initiator_counter) {
    if (enums::EntityRole::Initiator == endpoint.get_connected_entities().front().get_entity_role()) {
        initiator_counter++;
        if (1 < initiator_counter) {
            THROW(exceptions::InvalidValue, "nvme-discovery-agent",
                  "Zone cannot have more than one initiator endpoint.");
        }
    }
}

void throw_if_nqn_not_present(const Endpoint& endpoint) {
    std::string nqn{};

    try {
        nqn = attribute::Identifier::get_nqn(endpoint);
    }
    catch (const std::logic_error&) {
        THROW(exceptions::NvmeError, "nvme-discovery-agent",
              "Unable to read NQN from endpoint.");
    }

    if (nqn.empty()) {
        THROW(exceptions::NvmeError, "nvme-discovery-agent",
              "NQN identifier was not found or is empty.");
    }
}

void throw_if_endpoint_already_in_zone(const Endpoint& endpoint) {
    auto &manager = get_m2m_manager<Zone, Endpoint>();
    if (!manager.get_parents(endpoint.get_uuid()).empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent",
              "Endpoint is already in a zone.");
    }
}

void validate_zone_endpoints(const attribute::Array<Uuid>& endpoints) {
    throw_if_duplicates(endpoints);
    uint16_t initiator_counter{0};
    for (const auto& endpoint_uuid : endpoints) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        throw_if_connected_entities_empty(endpoint);
        throw_if_more_than_one_initiator(endpoint, initiator_counter);
        throw_if_nqn_not_present(endpoint);
        if (enums::EntityRole::Target == endpoint.get_connected_entities().front().get_entity_role().value()) {
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
    log_info("nvme-discovery-agent", "Added endpoints to zone '" << zone_uuid
             << "', added endpoints uuids:" << endpoints_uuids.str());
}

void store_to_database(const Uuid& zone, const AddZone::Request& req) {
    FabricDatabase fabric_db{get_manager<Fabric>().get_keys().front()};
    fabric_db.append(NvmeDatabase::ZONES, zone);
    ZoneDatabase zone_db(zone);
    const auto& endpoints = req.get_endpoints();
    for_each(endpoints.begin(), endpoints.end(),
             [&zone_db](const Uuid& endpoint) { zone_db.append(NvmeDatabase::ENDPOINTS, endpoint); });
}

void add_zone(AddZone::ContextPtr, const AddZone::Request& req, AddZone::Response& rsp) {

    get_manager<Fabric>().get_entry(req.get_fabric());
    Zone zone{req.get_fabric()};

    validate_zone_endpoints(req.get_endpoints());

    zone.set_status({enums::State::Enabled, enums::Health::OK});
    store_to_database(zone.get_uuid(), req);

    zone.add_collection(model::attribute::Collection(
            model::enums::CollectionName::Endpoints,
            model::enums::CollectionType::Endpoints
    ));

    rsp.set_zone(zone.get_uuid());
    get_manager<Zone>().add_entry(zone);
    log_info("nvme-discovery-agent", "Added zone with UUID '" + zone.get_uuid() + "'");
    add_endpoints_to_zone(req, zone.get_uuid());
}
}

REGISTER_NVME_COMMAND(AddZone, ::add_zone);
