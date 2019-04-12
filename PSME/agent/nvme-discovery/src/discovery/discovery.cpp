/*!
 * @brief Implementation of discovery.
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
 * @file discovery.cpp
 */

#include "discovery/discovery.hpp"
#include "tree_stability/nvme_stabilizer.hpp"
#include "tools/nvme_context.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"
#include "loader/config.hpp"
#include "logger/logger_factory.hpp"

#include "agent-framework/module/common_components.hpp"

#include <stdexcept>

#include <net/if.h>

using namespace agent::nvme::discovery;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;
using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::enums;

Discovery::Discovery(std::shared_ptr<NvmeAgentContext> context) :
    m_context(context) {}

Discovery::~Discovery() {}

namespace {

void update_manager(const Uuid& manager_uuid) {
    auto manager = get_manager<Manager>().get_entry_reference(manager_uuid);
    manager->add_collection({CollectionName::StorageServices,
                            CollectionType::StorageServices});
}

void add_endpoint_to_zone(const Uuid& zone, const Uuid& endpoint) {
    if (!get_manager<Endpoint>().entry_exists(endpoint)) {
        log_error("nvme-discovery-agent", "No endpoint found in manager with given UUID '" + endpoint + "'");
        return;
    }
    get_m2m_manager<Zone, Endpoint>().add_entry(zone, endpoint);
}

void load_endpoints_from_zone_database(const Uuid& zone) {
    ZoneDatabase zone_db(zone);
    auto endpoints{zone_db.get_multiple_values(NvmeDatabase::ENDPOINTS)};
    std::for_each(endpoints.begin(), endpoints.end(), [&zone](const std::string& endpoint) {add_endpoint_to_zone(zone, endpoint);});
}

void add_zone(const Uuid& fabric_uuid, const Uuid& zone_uuid) {
    Zone zone{fabric_uuid};
    zone.set_uuid(zone_uuid);

    zone.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    zone.add_collection(attribute::Collection(enums::CollectionName::Endpoints,
                                              enums::CollectionType::Endpoints));
    get_manager<Zone>().add_entry(zone);
    load_endpoints_from_zone_database(zone_uuid);
}

void discover_zones(const Uuid& fabric) {
    FabricDatabase fabric_db(fabric);
    auto zones{fabric_db.get_multiple_values(NvmeDatabase::ZONES)};
    std::for_each(zones.begin(), zones.end(), [&fabric](const std::string& zone) {::add_zone(fabric, zone);});
}

void add_initiator(const Uuid& fabric, const Uuid& uuid) {
    Endpoint endpoint{fabric};
    try {
        EndpointDatabase db{uuid};
        endpoint.add_identifier({literals::Endpoint::NQN_FORMAT + db.get(NvmeDatabase::NQN),
            enums::IdentifierType::NQN});
    }
    catch (const std::exception& e) {
        log_error("nvme-discovery-agent", "Unable to read initiator endpoint NQN from database");
        return;
    }
    endpoint.add_identifier({uuid, enums::IdentifierType::UUID});
    endpoint.set_uuid(uuid);
    endpoint.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    attribute::ConnectedEntity ce{};
    ce.set_entity_role(enums::EntityRole::Initiator);
    endpoint.add_connected_entity(ce);
    endpoint.set_protocol(enums::TransportProtocol::NVMeOverFabrics);
    get_manager<Endpoint>().add_entry(endpoint);
}

void add_target(const Uuid& fabric, const Uuid& endpoint_uuid) {
    Endpoint endpoint{fabric};

    attribute::IpTransportDetail ip_transport_detail{};

    try {
        EndpointDatabase db{endpoint_uuid};
        endpoint.add_identifier({literals::Endpoint::NQN_FORMAT + db.get(NvmeDatabase::NQN),
            enums::IdentifierType::NQN});
        ip_transport_detail.set_ipv4_address({db.get(NvmeDatabase::IPV4)});
        ip_transport_detail.set_port(std::stoi(db.get(NvmeDatabase::RDMA_PORT)));
        ip_transport_detail.set_transport_protocol(enums::TransportProtocol::RoCEv2);
        endpoint.add_ip_transport_detail(ip_transport_detail);
    }
    catch (const std::exception& e) {
        log_error("nvme-discovery-agent", "Unable to read target endpoint form database");
        return;
    }


    endpoint.add_identifier({endpoint_uuid, enums::IdentifierType::UUID});
    endpoint.set_uuid(endpoint_uuid);
    attribute::ConnectedEntity ce{};
    ce.set_entity_role(enums::EntityRole::Target);
    endpoint.add_connected_entity(ce);
    endpoint.set_protocol(enums::TransportProtocol::NVMeOverFabrics);
    endpoint.set_status(attribute::Status(enums::State::Enabled, enums::Health::OK));
    get_manager<Endpoint>().add_entry(endpoint);
}

void discover_endpoints(const Uuid& fabric) {
    FabricDatabase fabric_db{fabric};
    auto endpoints = fabric_db.get_multiple_values(NvmeDatabase::ENDPOINTS);
    for (auto endpoint_uuid : endpoints) {
        try {
            EndpointDatabase db{endpoint_uuid};
            if (literals::Endpoint::TARGET == db.get(NvmeDatabase::ROLE)) {
                add_target(fabric, endpoint_uuid);
            }
            else { // INITIATOR
                add_initiator(fabric, endpoint_uuid);
            }
        }
        catch (const std::exception& e) {
            log_error("nvme-discovery-agent", "Unable to read endpoint role from database");
        }
    }
}

}

void Discovery::discover(const Uuid& manager_uuid, const Uuid& fabric_uuid) {
    ::update_manager(manager_uuid);
    ::discover_endpoints(fabric_uuid);
    ::discover_zones(fabric_uuid);
}
