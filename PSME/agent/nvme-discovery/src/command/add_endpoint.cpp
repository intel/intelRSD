/*!
 * @brief Implementation of AddEndpoint command.
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
 * @file add_endpoint.cpp
 */

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "loader/config.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"
#include "tree_stability/nvme_stabilizer.hpp"

#include <cstdint>
#include <string>
#include <stdexcept>

using namespace agent::nvme;
using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;
using namespace std;

namespace {

void throw_if_endpoint_exist(const std::string& endpoint_uuid) {
    if (get_manager<Endpoint>().entry_exists(endpoint_uuid)) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "This NQN is already in use");
    }
}

void throw_if_wrong_transport_protocol(const attribute::IpTransportDetail& single_transport) {
    if (enums::TransportProtocol::RoCEv2 != single_transport.get_transport_protocol()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Transport protocol should be set on RoCEv2");
    }
}

// Valid NQN format
// nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff
void throw_if_durable_name_is_in_wrong_format(const attribute::Identifier& single_identifier) {
    if (0 != single_identifier.get_durable_name().value().find(model::literals::Endpoint::NQN_FORMAT)) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Durable name is not in NQN format");
    }
}

void throw_if_wrong_durable_format(const attribute::Identifier& single_identifier) {
    if (enums::IdentifierType::NQN != single_identifier.get_durable_name_format()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Only NQN Identifier is supported");
    }
}

void throw_if_durable_name_empty(const attribute::Identifier& single_identifier) {
    if (!single_identifier.get_durable_name().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Durable name field is mandatory");
    }
}

void throw_if_connected_entity_empty(const AddEndpoint::Request& req) {
    if (req.get_connected_entities().empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Exactly one connected entity needs to be given");
    }
}

void throw_if_wrong_entity_role(const attribute::ConnectedEntity& single_connected_entity) {
    if (enums::EntityRole::Target != single_connected_entity.get_entity_role() &&
        enums::EntityRole::Initiator != single_connected_entity.get_entity_role()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Only Target and Initiator connected entity role are supported");
    }
}

void throw_if_identifier_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (!single_connected_entity.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Identifier in Connected Entity should be empty");
    }
}

void throw_if_identifiers_empty(const AddEndpoint::Request& req) {
    if (req.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Exactly one identifier needs to be given");
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

void verify_connected_entities(const AddEndpoint::Request &req) {
    throw_if_connected_entity_empty(req);
    for (const auto& single_connected_entity : req.get_connected_entities()) {
        throw_if_wrong_entity_role(single_connected_entity);
        throw_if_identifier_not_empty(single_connected_entity);
    }
}

void throw_if_wrong_rdma_port(std::uint32_t rdma_port) {
    if (rdma_port <= 0 || rdma_port > UINT16_MAX) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Rdma port is out of rage, should be in <1,65535>");
    }
}

void throw_if_entity_link_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (single_connected_entity.get_entity().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent",
              "Invalid value in Entity property, this field should be empty");
    }
}

void throw_if_ethernet_interface_not_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (single_ip_transport_detail.get_interface().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent",
              "Invalid value in Interface property, this field should be empty");
    }
}

std::string get_nqn(const AddEndpoint::Request& req) {
    std::string nqn{};
    nqn = req.get_identifiers().front().get_durable_name();
    return nqn;
}

void throw_if_ip_transport_details_empty(const AddEndpoint::Request& req) {
    if (req.get_ip_transport_details().empty()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Mandatory property is missing: IpTransportDetails");
    }
}

void throw_if_ipv4_address_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (!single_ip_transport_detail.get_ipv4_address().get_address().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Mandatory property is missing: IPv4Address");
    }
}

void throw_if_rdma_port_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (!single_ip_transport_detail.get_port().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-discovery-agent", "Mandatory property is missing: Port");
    }
}

void set_rdma_port(const attribute::IpTransportDetail& ip_transport_detail_req,
                   attribute::IpTransportDetail& ip_transport_detail) {
    auto rdma_port = ip_transport_detail_req.get_port().value();
    throw_if_wrong_rdma_port(rdma_port);
    ip_transport_detail.set_port(uint16_t(rdma_port));
}

void set_transport_protocol(const attribute::IpTransportDetail& ip_transport_detail_req,
                            attribute::IpTransportDetail& ip_transport_detail) {
    if (ip_transport_detail_req.get_transport_protocol().has_value()) {
        throw_if_wrong_transport_protocol(ip_transport_detail_req);
        ip_transport_detail.set_transport_protocol(ip_transport_detail_req.get_transport_protocol().value());
    } else {
        ip_transport_detail.set_transport_protocol(enums::TransportProtocol::RoCEv2);
    }
}

void add_endpoint(AddEndpoint::ContextPtr /* ctx */, const AddEndpoint::Request& req, AddEndpoint::Response& rsp) {
    NvmeStabilizer ns{};
    const auto& fabric{req.get_fabric()};

    log_debug("nvme-discovery-agent", "Adding endpoint");

    auto a = get_manager<Fabric>().get_entry(fabric);
    Endpoint endpoint{fabric};

    verify_connected_entities(req);
    verify_identifiers(req);
    std::string nqn = get_nqn(req);

    endpoint.add_identifier({nqn, enums::IdentifierType::NQN});
    const auto endpoint_uuid = ns.stabilize(endpoint);

    throw_if_endpoint_exist(endpoint_uuid);

    endpoint.add_identifier({endpoint_uuid, enums::IdentifierType::UUID});
    endpoint.set_protocol(enums::TransportProtocol::NVMeOverFabrics);
    endpoint.set_connected_entities(req.get_connected_entities());
    endpoint.set_status({enums::State::Enabled, enums::Health::OK});
    endpoint.set_oem(req.get_oem());

    attribute::IpTransportDetail ip_transport_detail{};
    attribute::Ipv4Address ipv4_address{};
    tools::convert_to_subnqn(nqn);

    if (enums::EntityRole::Target == req.get_connected_entities().front().get_entity_role()) { // Target role
        throw_if_entity_link_not_empty(req.get_connected_entities().front());
        throw_if_ip_transport_details_empty(req);

        const auto ip_transport_detail_req = req.get_ip_transport_details().front();

        throw_if_rdma_port_empty(ip_transport_detail_req);
        set_rdma_port(ip_transport_detail_req, ip_transport_detail);

        throw_if_ipv4_address_empty(ip_transport_detail_req);
        ipv4_address.set_address(ip_transport_detail_req.get_ipv4_address().get_address().value());
        ip_transport_detail.set_ipv4_address(ipv4_address);
        set_transport_protocol(ip_transport_detail_req, ip_transport_detail);
        endpoint.add_ip_transport_detail(ip_transport_detail);

        // Adding to database
        FabricDatabase(fabric).append(NvmeDatabase::ENDPOINTS, endpoint_uuid);
        EndpointDatabase(endpoint_uuid).put(NvmeDatabase::ROLE, model::literals::Endpoint::TARGET);
        EndpointDatabase(endpoint_uuid).put(NvmeDatabase::NQN, nqn);
        EndpointDatabase(endpoint_uuid).put(NvmeDatabase::RDMA_PORT, std::to_string(ip_transport_detail.get_port().value()));
        EndpointDatabase(endpoint_uuid).put(NvmeDatabase::IPV4, ipv4_address.get_address().value());


    }
    else { // EntityRole::Initiator Endpoint
        throw_if_entity_link_not_empty(req.get_connected_entities().front());
        if (!req.get_ip_transport_details().empty()) {
            const auto ip_transport_detail_req = req.get_ip_transport_details().front();
            set_rdma_port(ip_transport_detail_req, ip_transport_detail);
            set_transport_protocol(ip_transport_detail_req, ip_transport_detail);

            if (ip_transport_detail_req.get_ipv4_address().get_address().has_value()) {
                ipv4_address.set_address(ip_transport_detail_req.get_ipv4_address().get_address().value());
            }
            ip_transport_detail.set_ipv4_address(ipv4_address);

            throw_if_ethernet_interface_not_empty(ip_transport_detail_req);
            endpoint.add_ip_transport_detail(ip_transport_detail);
        }

        // Adding to database
        FabricDatabase(fabric).append(NvmeDatabase::ENDPOINTS, endpoint_uuid);
        EndpointDatabase(endpoint_uuid).put(NvmeDatabase::ROLE, model::literals::Endpoint::INITIATOR);
        EndpointDatabase(endpoint_uuid).put(NvmeDatabase::NQN, nqn);
    }

    rsp.set_endpoint(endpoint_uuid);
    get_manager<Endpoint>().add_entry(endpoint);
    log_info("nvme-discovery-agent", "Added endpoint with UUID '" + endpoint_uuid + "'");
}
}

REGISTER_NVME_COMMAND(AddEndpoint, ::add_endpoint);
