/*!
 * @brief Endpoint discoverer implementation.
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
 * @file fpgaof_endpoint_discoverer.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "discovery/discoverers/fpgaof_endpoint_discoverer.hpp"
#include "discovery/builders/fpgaof_endpoint_builder.hpp"
#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;
using namespace agent_framework::database;
using namespace agent::fpgaof;
using namespace opaepp;

namespace {

void add_initiator_attributes(Endpoint& endpoint) {
    attribute::ConnectedEntity ce{};
    ce.set_entity_role(EntityRole::Initiator);
    endpoint.add_connected_entity(ce);
}


void add_target_attributes(Endpoint& endpoint, const Uuid& endpoint_uuid) {
    attribute::IpTransportDetail ip_transport_detail{};

    try {
        agent_framework::database::EndpointEntity db{endpoint_uuid};
        ip_transport_detail.set_ipv4_address({db.get(agent_framework::database::IPV4_PROPERTY)});
        ip_transport_detail.set_port(std::stoi(db.get(agent_framework::database::PORT_PROPERTY)));
        ip_transport_detail.set_transport_protocol(TransportProtocol::OEM);
        ip_transport_detail.set_interface(
            agent::fpgaof::utils::get_ethernet_interface_uuid_from_ip_address(ip_transport_detail.get_ipv4_address()));
        endpoint.add_ip_transport_detail(ip_transport_detail);
        attribute::ConnectedEntity ce{};
        ce.set_entity_role(EntityRole::Target);
        ce.set_entity(db.get(agent_framework::database::FPGA_DEVICE_UUID_PROPERTY));
        endpoint.add_connected_entity(ce);
    }
    catch (const std::exception& e) {
        log_error("fpgaof-discovery", "Unable to read target endpoint from database: " << e.what());
        throw std::runtime_error(std::string("Unable to read target endpoint from database: ") + e.what());
    }
}


void set_common_attributes(Endpoint& endpoint, const Uuid& endpoint_uuid) {
    try {
        agent_framework::database::EndpointEntity db{endpoint_uuid};
        endpoint.add_identifier({db.get(agent_framework::database::UUID_PROPERTY), enums::IdentifierType::UUID});
    }
    catch (const std::exception& e) {
        log_error("fpgaof-discovery", "Unable to read initiator endpoint UUID identifier from database: " << e.what());
        throw std::runtime_error(
            std::string("Unable to read initiator endpoint UUID identifier from database: ") + e.what());
    }
    endpoint.set_uuid(endpoint_uuid);
    endpoint.set_status(attribute::Status(State::Enabled, Health::OK));
    endpoint.set_protocol(enums::TransportProtocol::OEM);
    endpoint.set_oem_protocol(enums::OemProtocol::FPGAoF);
}

}


std::vector<agent_framework::model::Endpoint> discovery::FpgaofEndpointDiscoverer::discover(const Uuid& fabric_uuid) {
    std::vector<Endpoint> endpoints{};

    log_info("fpgaof-discovery", "Discovering endpoints");

    auto endpoint_uuids = agent_framework::database::FabricEntity(fabric_uuid).get_multiple_values(
        agent_framework::database::ZONE_ENDPOINTS_PROPERTY);

    for (const auto& endpoint_uuid : endpoint_uuids) {
        Endpoint endpoint{fabric_uuid};
        log_info("fpgaof-discovery", "Discovering endpoint " + endpoint_uuid + " from database");
        try {
            set_common_attributes(endpoint, endpoint_uuid);

            agent_framework::database::EndpointEntity db{endpoint_uuid};
            auto role = db.get(agent_framework::database::ENDPOINT_ROLE_PROPERTY);

            if (literals::Endpoint::TARGET == role) {
                add_target_attributes(endpoint, endpoint_uuid);
            }
            else if (literals::Endpoint::INITIATOR == role) {
                add_initiator_attributes(endpoint);
                OpaeProxyHostApi::add_initiator_host(*m_context->opae_proxy_context,
                                                     attribute::Identifier::get_uuid(endpoint));
                log_info("fpgaof-discovery",
                         "opaepp : added host " + attribute::Identifier::get_uuid(endpoint) + " to opae proxy.");
            }
            else {
                log_error("fpgaof-discovery", "Unrecognized endpoint role read from database " << role);
            }
            endpoints.push_back(endpoint);
        }
        catch (const std::exception&) {
            log_error("fpgaof-discovery", "Unable to read endpoint " + endpoint_uuid + " from database");
        }
    }
    return endpoints;
}





