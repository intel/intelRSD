/*!
 * @brief Implementation of AddEndpoint command.
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
 * @file add_endpoint.cpp
 */


#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "discovery/builders/fpgaof_endpoint_builder.hpp"
#include "fpgaof_agent_commands.hpp"
#include "fpgaof_stabilizer.hpp"
#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "uuid/uuid.hpp"
#include "utils.hpp"



using namespace agent::fpgaof;
using namespace agent::fpgaof::loader;
using namespace agent::fpgaof::discovery;
using namespace agent_framework;
using namespace agent_framework::database;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::exceptions;
using namespace agent_framework::module;
using namespace opaepp;

namespace {

void throw_if_endpoint_exist(const std::string& endpoint_uuid) {
    if (get_manager<Endpoint>().entry_exists(endpoint_uuid)) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "This UUID is already in use");
    }
}


void throw_if_wrong_transport_protocol(const attribute::IpTransportDetail& single_transport) {
    if (enums::TransportProtocol::OEM != single_transport.get_transport_protocol()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Transport protocol should be set on OEM");
    }
}


// Valid UUID format ffffffff-ffff-ffff-ffff-ffffffffffff
void throw_if_durable_name_is_in_wrong_format(const attribute::Identifier& single_identifier) {
    Uuid uuid = single_identifier.get_durable_name().value();

    if (!is_valid_string_uuid(uuid)) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "Durable name is not in UUID format (ffffffff-ffff-ffff-ffff-ffffffffffff).");
    }
}


void throw_if_wrong_durable_format(const attribute::Identifier& single_identifier) {
    if (enums::IdentifierType::UUID != single_identifier.get_durable_name_format()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Only UUID Identifier is supported");
    }
}


void throw_if_durable_name_empty(const attribute::Identifier& single_identifier) {
    if (!single_identifier.get_durable_name().has_value()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Durable name field is mandatory");
    }
}


void throw_if_not_exactly_one_connected_entity(const AddEndpoint::Request& req) {
    if (req.get_connected_entities().size() != 1) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Exactly one connected entity needs to be given");
    }
}


void throw_if_wrong_entity_role(const attribute::ConnectedEntity& single_connected_entity) {
    if (enums::EntityRole::Target != single_connected_entity.get_entity_role() &&
        enums::EntityRole::Initiator != single_connected_entity.get_entity_role()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "Only Target and Initiator connected entity role are supported");
    }
}


void throw_if_identifier_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (!single_connected_entity.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Identifier in Connected Entity should be empty");
    }
}


void throw_if_identifiers_empty(const AddEndpoint::Request& req) {
    if (req.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Exactly one identifier needs to be given");
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


std::string get_ip_address_from_ethernet_interface_uuid(const std::string& uuid) {
    const auto& network_interface = get_manager<NetworkInterface>().get_entry(uuid);
    std::string ip_address{};
    if (!network_interface.get_ipv4_addresses().empty()) {
        if (network_interface.get_ipv4_addresses().front().get_address().has_value()) {
            ip_address = network_interface.get_ipv4_addresses().front().get_address().value();
        }
    }
    if (ip_address.empty()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "Provided ethernet interface does not have IP address.");
    }

    return ip_address;
}


// Find first ethernet interface that has ip address
attribute::Ipv4Address get_ip_address_from_ethernet_interfaces() {
    if (get_manager<NetworkInterface>().get_entries().empty()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "There is no ethernet interface on this host.");
    }

    attribute::Ipv4Address ip_address{};
    for (const auto& network_interface_uuid : get_manager<NetworkInterface>().get_keys()) {
        const auto& network_interface = get_manager<NetworkInterface>().get_entry(network_interface_uuid);
        if (!network_interface.get_ipv4_addresses().empty()) {
            if (network_interface.get_ipv4_addresses().front().get_address().has_value()) {
                ip_address = network_interface.get_ipv4_addresses().front();
                break;
            }
        }
    }
    if (!ip_address.get_address().has_value()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "There is no valid ethernet interface on this host.");
    }
    return ip_address;
}


void verify_connected_entities(const AddEndpoint::Request& req) {
    throw_if_not_exactly_one_connected_entity(req);
    for (const auto& single_connected_entity : req.get_connected_entities()) {
        throw_if_wrong_entity_role(single_connected_entity);
        throw_if_identifier_not_empty(single_connected_entity);
    }
}


void set_ip_transport_detail_with_default(FpgaofConfiguration& configuration,
                                          attribute::IpTransportDetail& ip_transport_detail) {
    auto first_opae_proxy_transport = configuration.get_opae_proxy_transports().front();

    attribute::Ipv4Address ipv4_address{};
    ipv4_address.set_address(first_opae_proxy_transport.get_ipv4_address());
    ip_transport_detail.set_ipv4_address(ipv4_address);
    ip_transport_detail.set_port(first_opae_proxy_transport.get_port());
    auto interface = agent::fpgaof::utils::get_ethernet_interface_uuid_from_ip_address(ipv4_address);
    if (interface.has_value()) {
        ip_transport_detail.set_interface(interface.value());
    }
    ip_transport_detail.set_transport_protocol(enums::TransportProtocol::OEM);
}


void throw_if_wrong_port(std::uint32_t port) {
    if (port <= 0 || port > UINT16_MAX) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Port is out of range, should be in <1,65535>.");
    }
}


// Entity link field - throw if empty or invalid
void throw_if_wrong_target_entity(const attribute::ConnectedEntity& single_connected_entity) {
    if (!single_connected_entity.get_entity().has_value()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Missing entity in connected entities collection.");
    }
    get_manager<Processor>().get_entry(single_connected_entity.get_entity().value());
    log_debug("fpgaof-agent", "Connected entity Processor " + single_connected_entity.get_entity().value());
}


void throw_if_entity_link_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (single_connected_entity.get_entity().has_value()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "Invalid value in Entity property, this field should be empty.");
    }
}


void throw_if_ethernet_interface_not_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (single_ip_transport_detail.get_interface().has_value()) {
        THROW(exceptions::InvalidValue, "fpgaof-agent",
              "Invalid value in Interface property, this field should be empty.");
    }
}


void throw_if_ip_transport_details_more_than_one(const AddEndpoint::Request& requset) {
    if (requset.get_ip_transport_details().size() > 1) {
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Only one IP transport detail is supported.");
    }
}


void throw_if_processor_in_use(const Processor& processor) {
    if (Endpoint::is_resource_in_endpoint(processor.get_uuid())) {
        log_error("fpgaof-agent", "Processor " << processor.get_uuid() << " is already used by an endpoint.");
        THROW(exceptions::InvalidValue, "fpgaof-agent", "Processor is already used by an endpoint.");
    }
}


void set_port(FpgaofConfiguration& configuration,
              const attribute::IpTransportDetail& ip_transport_detail_req,
              attribute::IpTransportDetail& ip_transport_detail) {
    if (ip_transport_detail_req.get_port().has_value()) {
        throw_if_wrong_port(ip_transport_detail_req.get_port().value());
        ip_transport_detail.set_port(std::uint16_t(ip_transport_detail_req.get_port().value()));
    }
    else {
        // TODO: Add a way to specify default transport details
        auto port = configuration.get_opae_proxy_transports().front().get_port();
        throw_if_wrong_port(port);
        ip_transport_detail.set_port(std::uint16_t(port));
    }
}


void set_ipv4_address(const attribute::IpTransportDetail& ip_transport_detail_req,
                      attribute::IpTransportDetail& ip_transport_detail) {

    std::string address{};
    if (ip_transport_detail_req.get_interface().has_value()) {
        address = get_ip_address_from_ethernet_interface_uuid(ip_transport_detail_req.get_interface().value());
        log_debug("fpgaof-agent", "Setting IP address: " << address
                                                         << " from requested interface "
                                                         << ip_transport_detail_req.get_interface());
    }
    else if (ip_transport_detail_req.get_ipv4_address().get_address().has_value()) {
        address = ip_transport_detail_req.get_ipv4_address().get_address().value();
        log_debug("fpgaof-agent", "Setting IP address: " << address << " from request");
    }
    else {
        address = get_ip_address_from_ethernet_interfaces().get_address();
        log_debug("fpgaof-agent", "Setting default IP address: " << address);
    }
    attribute::Ipv4Address ipv4_address{};
    ipv4_address.set_address(address);
    ip_transport_detail.set_ipv4_address(std::move(ipv4_address));
}


void set_interface(const attribute::IpTransportDetail& ip_transport_detail_req,
                   attribute::IpTransportDetail& ip_transport_detail) {
    if (ip_transport_detail_req.get_interface().has_value()) {
        // if line below throws exception it means that this interface is unable to use
        get_ip_address_from_ethernet_interface_uuid(ip_transport_detail_req.get_interface().value());
        ip_transport_detail.set_interface(ip_transport_detail_req.get_interface().value());
        log_debug("fpgaof-agent",
                  "Setting interface " << ip_transport_detail_req.get_interface().value() << " from request");
        return;
    }

    if (ip_transport_detail_req.get_ipv4_address().get_address().has_value()) {
        auto interface = agent::fpgaof::utils::get_ethernet_interface_uuid_from_ip_address(
            ip_transport_detail_req.get_ipv4_address());

        if (interface.has_value()) {
            log_debug("fpgaof-agent", "Setting interface " << interface.value()
                                                           << " based on IP address "
                                                           << ip_transport_detail_req.get_ipv4_address().get_address().value());
            ip_transport_detail.set_interface(interface.value());
            return;
        }
    }

    log_debug("fpgaof-agent", "Setting default interface...");
    auto interface = agent::fpgaof::utils::get_ethernet_interface_uuid_from_ip_address(
        get_ip_address_from_ethernet_interfaces());
    if (interface.has_value()) {
        ip_transport_detail.set_interface(interface.value());
    }
}


void set_ipv4_address_and_interface_with_verification(const attribute::IpTransportDetail& ip_transport_detail_req,
                                                      attribute::IpTransportDetail& ip_transport_detail) {

    const auto& ipv4_address = ip_transport_detail_req.get_ipv4_address();
    const auto& interface_from_request = ip_transport_detail_req.get_interface();

    if (ipv4_address.get_address().has_value()) {
        auto interface = agent::fpgaof::utils::get_ethernet_interface_uuid_from_ip_address(ipv4_address);
        if (!interface.has_value()) {
            THROW(exceptions::InvalidValue, "fpgaof-agent",
                  "There is no ethernet interface with requested IPv4 address.");
        }

        if (interface_from_request.has_value() && interface_from_request.value() != interface.value()) {
            THROW(exceptions::InvalidValue, "fpgaof-agent",
                  "Requested ethernet interface does not match ethernet interface with IP from the request.");
        }

        log_debug("fpgaof-agent",
                  "Interface " << interface.value() << " found with IP " << ipv4_address.get_address().value());
    }

    set_ipv4_address(ip_transport_detail_req, ip_transport_detail);
    set_interface(ip_transport_detail_req, ip_transport_detail);
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


void add_endpoint(AddEndpoint::ContextPtr ctx, const AddEndpoint::Request& req, AddEndpoint::Response& rsp) {
    FpgaofStabilizer fpgaof_stabilizer{};
    const auto& fabric{req.get_fabric()};

    log_info("fpgaof-agent", "Adding endpoint");

    get_manager<Fabric>().get_entry(fabric);
    auto endpoint = FpgaofEndpointBuilder::build_default(fabric);

    verify_connected_entities(req);
    verify_identifiers(req);
    std::string endpoint_uuid = req.get_identifiers().front().get_durable_name();

    // tree stability requires UUID to be already set
    endpoint.add_identifier({endpoint_uuid, enums::IdentifierType::UUID});

    Uuid uuid = fpgaof_stabilizer.stabilize(endpoint);

    throw_if_endpoint_exist(uuid);

    endpoint.set_protocol(enums::TransportProtocol::OEM);
    endpoint.set_oem_protocol(enums::OemProtocol::FPGAoF);
    endpoint.set_connected_entities(req.get_connected_entities());
    endpoint.set_status({enums::State::Enabled, enums::Health::OK});
    endpoint.set_oem(req.get_oem());

    if (enums::EntityRole::Target == req.get_connected_entities().front().get_entity_role()) { // Target role

        throw_if_wrong_target_entity(req.get_connected_entities().front());
        attribute::IpTransportDetail ip_transport_detail{};

        if (req.get_ip_transport_details().empty()) { // IP transport details is not provided
            log_debug("fpgaof-agent", "No IP transport details provided - using default interface");
            set_ip_transport_detail_with_default(*ctx->configuration, ip_transport_detail);
            endpoint.add_ip_transport_detail(ip_transport_detail);
        }
        else { // IP transport details is provided
            throw_if_ip_transport_details_more_than_one(req);
            const auto ip_transport_detail_req = req.get_ip_transport_details().front();

            set_port(*ctx->configuration, ip_transport_detail_req, ip_transport_detail);
            set_ipv4_address_and_interface_with_verification(ip_transport_detail_req, ip_transport_detail);
            set_transport_protocol(ip_transport_detail_req, ip_transport_detail);

            endpoint.add_ip_transport_detail(ip_transport_detail);
        }

        const Processor processor = get_manager<Processor>()
            .get_entry(endpoint.get_connected_entities().front().get_entity().value());
        throw_if_processor_in_use(processor);

        agent_framework::database::EndpointEntity db{uuid};
        db.put(agent_framework::database::PORT_PROPERTY,
               std::to_string(static_cast<unsigned int>(ip_transport_detail.get_port())));
        db.put(agent_framework::database::ENDPOINT_ROLE_PROPERTY, model::literals::Endpoint::TARGET);
        db.put(agent_framework::database::UUID_PROPERTY, endpoint_uuid);
        db.put(agent_framework::database::IPV4_PROPERTY, ip_transport_detail.get_ipv4_address().get_address());
        db.put(agent_framework::database::FPGA_DEVICE_UUID_PROPERTY, processor.get_uuid());

        agent_framework::database::FabricEntity(fabric).append(agent_framework::database::ZONE_ENDPOINTS_PROPERTY,
                                                               uuid);
    }
    else { // EntityRole::Initiator Endpoint
        throw_if_entity_link_not_empty(req.get_connected_entities().front());
        if (!req.get_ip_transport_details().empty()) {
            throw_if_ip_transport_details_more_than_one(req);
            throw_if_ethernet_interface_not_empty(req.get_ip_transport_details().front());
        }

        agent_framework::database::FabricEntity(fabric).append(agent_framework::database::ZONE_ENDPOINTS_PROPERTY,
                                                               uuid);
        EndpointEntity(uuid).put(agent_framework::database::ENDPOINT_ROLE_PROPERTY,
                                 model::literals::Endpoint::INITIATOR);
        EndpointEntity(uuid).put(agent_framework::database::UUID_PROPERTY, endpoint_uuid);
        OpaeProxyHostApi::add_initiator_host(*ctx->opae_proxy_context, endpoint_uuid);
        log_debug("fpgaof-agent", "opaepp : added endpoint " + uuid + " to opae proxy.");
    }

    rsp.set_endpoint(uuid);
    get_manager<Endpoint>().add_entry(endpoint);
    log_info("fpgaof-agent", "Added endpoint, request UUID: '" + endpoint_uuid + "'" + ", stable UUID:" + uuid + ".");
}
}

REGISTER_FPGAOF_COMMAND(AddEndpoint, ::add_endpoint);