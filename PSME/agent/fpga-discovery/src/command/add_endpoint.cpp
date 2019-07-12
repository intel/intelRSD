/*!
 * @brief Implementation of AddEndpoint command.
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
 * @file add_endpoint.cpp
 */


#include "agent-framework/module/common_components.hpp"
#include "tools/databases.hpp"
#include "fpga_agent_commands.hpp"
#include "tree_stability/fpga_stabilizer.hpp"
#include "uuid/uuid.hpp"



using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::exceptions;
using namespace agent_framework::module;
using namespace agent::fpga_discovery;
using namespace agent::fpga_discovery::tools;

namespace {

void throw_if_endpoint_exist(const std::string& endpoint_uuid) {
    if (get_manager<Endpoint>().entry_exists(endpoint_uuid)) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "This UUID is already in use");
    }
}


void throw_if_wrong_transport_protocol(const attribute::IpTransportDetail& single_transport) {
    if (enums::TransportProtocol::RoCEv2 != single_transport.get_transport_protocol()
        && enums::TransportProtocol::OEM != single_transport.get_transport_protocol()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Transport protocol should be set on RoCEv2 or OEM");
    }
}


// Valid UUID format ffffffff-ffff-ffff-ffff-ffffffffffff
void throw_if_durable_name_is_in_wrong_format(const attribute::Identifier& single_identifier) {
    Uuid uuid = single_identifier.get_durable_name().value();

    if (!is_valid_string_uuid(uuid)) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Durable name is not in UUID format (ffffffff-ffff-ffff-ffff-ffffffffffff).");
    }
}


void throw_if_wrong_durable_format(const attribute::Identifier& single_identifier) {
    if (enums::IdentifierType::UUID != single_identifier.get_durable_name_format()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Only UUID Identifier is supported");
    }
}


void throw_if_durable_name_empty(const attribute::Identifier& single_identifier) {
    if (!single_identifier.get_durable_name().has_value()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Durable name field is mandatory");
    }
}


void throw_if_not_exactly_one_connected_entity(const AddEndpoint::Request& req) {
    if (req.get_connected_entities().size() != 1) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Exactly one connected entity needs to be given");
    }
}


void throw_if_wrong_entity_role(const attribute::ConnectedEntity& single_connected_entity) {
    if (enums::EntityRole::Target != single_connected_entity.get_entity_role() &&
        enums::EntityRole::Initiator != single_connected_entity.get_entity_role()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Only Target and Initiator connected entity role are supported");
    }
}


void throw_if_identifier_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (!single_connected_entity.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Identifier in Connected Entity should be empty");
    }
}


void throw_if_identifiers_empty(const AddEndpoint::Request& req) {
    if (req.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Exactly one identifier needs to be given");
    }
}


void verify_identifiers(const AddEndpoint::Request& req) {
    throw_if_identifiers_empty(req);
    for (const auto& single_identifier : req.get_identifiers()) {
        throw_if_wrong_durable_format(single_identifier);
        throw_if_durable_name_empty(single_identifier);
        throw_if_durable_name_is_in_wrong_format(single_identifier);
    }
}


void verify_connected_entities(const AddEndpoint::Request& req) {
    throw_if_not_exactly_one_connected_entity(req);
    for (const auto& single_connected_entity : req.get_connected_entities()) {
        throw_if_wrong_entity_role(single_connected_entity);
        throw_if_identifier_not_empty(single_connected_entity);
    }
}


void throw_if_ip_transport_details_empty(const AddEndpoint::Request& req) {
    if (req.get_ip_transport_details().empty()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Mandatory property is missing: IpTransportDetails");
    }
}


void throw_if_ipv4_address_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (!single_ip_transport_detail.get_ipv4_address().get_address().has_value()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Mandatory property is missing: IPv4Address");
    }
}


void throw_if_port_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (!single_ip_transport_detail.get_port().has_value()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Mandatory property is missing: Port");
    }
}


void throw_if_wrong_port(std::uint32_t port) {
    if (port <= 0 || port > UINT16_MAX) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Port is out of range, should be in <1,65535>.");
    }
}


void throw_if_entity_link_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (single_connected_entity.get_entity().has_value()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Invalid value in Entity property, this field should be empty.");
    }
}


void throw_if_ethernet_interface_not_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (single_ip_transport_detail.get_interface().has_value()) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent",
              "Invalid value in Interface property, this field should be empty.");
    }
}


void throw_if_ip_transport_details_more_than_one(const AddEndpoint::Request& requset) {
    if (requset.get_ip_transport_details().size() > 1) {
        THROW(exceptions::InvalidValue, "fpga-discovery-agent", "Only one IP transport detail is supported.");
    }
}


void set_port(const attribute::IpTransportDetail& ip_transport_detail_req,
              attribute::IpTransportDetail& ip_transport_detail) {
    throw_if_wrong_port(ip_transport_detail_req.get_port().value());
    ip_transport_detail.set_port(std::uint16_t(ip_transport_detail_req.get_port().value()));
}


void set_transport_protocol(const attribute::IpTransportDetail& ip_transport_detail_req,
                            attribute::IpTransportDetail& ip_transport_detail) {
    if (ip_transport_detail_req.get_transport_protocol().has_value()) {
        throw_if_wrong_transport_protocol(ip_transport_detail_req);
        ip_transport_detail.set_transport_protocol(ip_transport_detail_req.get_transport_protocol().value());
    }
    else {
        ip_transport_detail.set_transport_protocol(enums::TransportProtocol::OEM);
    }
}


void add_endpoint(AddEndpoint::ContextPtr /*ctx*/, const AddEndpoint::Request& req, AddEndpoint::Response& rsp) {
    const auto& fabric{req.get_fabric()};

    log_info("fpga-discovery-agent", "Adding endpoint");

    get_manager<Fabric>().get_entry(fabric);

    model::Endpoint endpoint{fabric};
    endpoint.set_parent_type(model::enums::Component::Fabric);

    verify_connected_entities(req);
    verify_identifiers(req);
    std::string endpoint_uuid = req.get_identifiers().front().get_durable_name();

    endpoint.add_identifier({endpoint_uuid, enums::IdentifierType::UUID});

    FpgaStabilizer stabilizer;
    Uuid uuid = stabilizer.stabilize(endpoint);

    throw_if_endpoint_exist(uuid);

    endpoint.set_protocol(enums::TransportProtocol::OEM);
    endpoint.set_oem_protocol(enums::OemProtocol::FPGAoF);
    endpoint.set_connected_entities(req.get_connected_entities());
    endpoint.set_status({enums::State::Enabled, enums::Health::OK});
    endpoint.set_oem(req.get_oem());

    attribute::IpTransportDetail ip_transport_detail{};
    attribute::Ipv4Address ipv4_address{};
    if (enums::EntityRole::Target == req.get_connected_entities().front().get_entity_role()) { // Target role

        throw_if_entity_link_not_empty(req.get_connected_entities().front());
        throw_if_ip_transport_details_empty(req);

        const auto ip_transport_detail_req = req.get_ip_transport_details().front();

        throw_if_port_empty(ip_transport_detail_req);
        throw_if_ipv4_address_empty(ip_transport_detail_req);
        throw_if_ip_transport_details_more_than_one(req);

        set_port(ip_transport_detail_req, ip_transport_detail);
        ipv4_address.set_address(ip_transport_detail_req.get_ipv4_address().get_address().value());
        ip_transport_detail.set_ipv4_address(ipv4_address);
        set_transport_protocol(ip_transport_detail_req, ip_transport_detail);
        endpoint.add_ip_transport_detail(ip_transport_detail);

        EndpointDatabase db{uuid};
        db.put(FpgaDiscoveryDatabase::PORT,
               std::to_string(static_cast<unsigned int>(ip_transport_detail.get_port())));
        db.put(FpgaDiscoveryDatabase::ROLE, model::literals::Endpoint::TARGET);
        db.put(FpgaDiscoveryDatabase::UUID, endpoint_uuid);
        db.put(FpgaDiscoveryDatabase::IPV4, ip_transport_detail.get_ipv4_address().get_address());
        db.put(FpgaDiscoveryDatabase::TRANSPORT_PROTOCOL,
               ip_transport_detail.get_transport_protocol().value().to_string());

        FabricDatabase(fabric).append(FpgaDiscoveryDatabase::ENDPOINTS,
                                      uuid);
    }
    else { // EntityRole::Initiator Endpoint
        throw_if_entity_link_not_empty(req.get_connected_entities().front());
        if (!req.get_ip_transport_details().empty()) {
            throw_if_ip_transport_details_more_than_one(req);
            throw_if_ethernet_interface_not_empty(req.get_ip_transport_details().front());
        }

        FabricDatabase(fabric).append(FpgaDiscoveryDatabase::ENDPOINTS,
                                      uuid);
        EndpointDatabase(uuid).put(FpgaDiscoveryDatabase::ROLE,
                                   model::literals::Endpoint::INITIATOR);
        EndpointDatabase(uuid).put(FpgaDiscoveryDatabase::UUID, endpoint_uuid);
    }

    rsp.set_endpoint(uuid);
    get_manager<Endpoint>().add_entry(endpoint);
    log_info("fpga-discovery-agent", "Added endpoint with UUID '" + uuid + "'");
}
}

REGISTER_FPGA_DISCOVERY_COMMAND(AddEndpoint, ::add_endpoint);