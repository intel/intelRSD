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
#include "agent-framework/module/common_components.hpp"
#include "loader/config.hpp"
#include "tools/endpoint_reader.hpp"
#include "tools/databases.hpp"
#include "tools/tools.hpp"
#include "tree_stability/nvme_stabilizer.hpp"
#include "sysfs/construct_dev_path.hpp"



using namespace agent_framework;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::nvme;
using namespace agent::nvme::loader;
using namespace agent::nvme::tools;

namespace {

void throw_if_endpoint_exist(const std::string& endpoint_uuid) {
    if (get_manager<Endpoint>().entry_exists(endpoint_uuid)) {
        THROW(exceptions::InvalidValue, "nvme-agent", "This NQN is already in use");
    }
}


void throw_if_wrong_transport_protocol(const attribute::IpTransportDetail& single_transport) {
    if (enums::TransportProtocol::RoCEv2 != single_transport.get_transport_protocol()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Transport protocol should be set on RoCEv2");
    }
}


// Valid NQN format
// nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff
void throw_if_durable_name_is_in_wrong_format(const attribute::Identifier& single_identifier) {
    if (0 != single_identifier.get_durable_name().value().find(literals::Endpoint::NQN_FORMAT)) {
        THROW(exceptions::InvalidValue, "nvme-agent",
              "Durable name is not in NQN format (nqn.2014-08.org.nvmexpress:uuid:ffffffff-ffff-ffff-ffff-ffffffffffff).");
    }
}


void throw_if_wrong_durable_format(const attribute::Identifier& single_identifier) {
    if (enums::IdentifierType::NQN != single_identifier.get_durable_name_format()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Only NQN Identifier is supported");
    }
}


void throw_if_durable_name_empty(const attribute::Identifier& single_identifier) {
    if (!single_identifier.get_durable_name().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Durable name field is mandatory");
    }
}


void throw_if_connected_entity_empty(const AddEndpoint::Request& req) {
    if (req.get_connected_entities().empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Exactly one connected entity needs to be given");
    }
}


void throw_if_wrong_entity_role(const attribute::ConnectedEntity& single_connected_entity) {
    if (enums::EntityRole::Target != single_connected_entity.get_entity_role() &&
        enums::EntityRole::Initiator != single_connected_entity.get_entity_role()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Only Target and Initiator connected entity role are supported");
    }
}


void throw_if_identifier_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (!single_connected_entity.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Identifier in Connected Entity should be empty");
    }
}


void throw_if_identifiers_empty(const AddEndpoint::Request& req) {
    if (req.get_identifiers().empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Exactly one identifier needs to be given");
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
        THROW(exceptions::InvalidValue, "nvme-agent",
              "Provided ethernet interface does not have IP address.");
    }

    return ip_address;
}


// Find first ethernet interface that has ip address
attribute::Ipv4Address get_ip_address_from_ethernet_interfaces() {
    if (get_manager<NetworkInterface>().get_entries().empty()) {
        THROW(exceptions::InvalidValue, "nvme-agent",
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
        THROW(exceptions::InvalidValue, "nvme-agent",
              "There is no valid ethernet interface on this host.");
    }
    return ip_address;
}


// Find ethernet interface with selected IP address
std::tuple<bool, Uuid> get_ethernet_interface_uuid_from_ip_address(const attribute::Ipv4Address& address_from_request) {
    auto interfaces = get_manager<NetworkInterface>()
        .get_keys([&address_from_request](const NetworkInterface& interface) {
            for (const auto& ipv4 : interface.get_ipv4_addresses()) {
                if (ipv4.get_address().has_value() && ipv4.get_address() == address_from_request.get_address()) {
                    return true;
                }
            }
            return false;
        });

    return interfaces.empty() ?
           std::make_tuple(false, Uuid{}) :
           std::make_tuple(true, interfaces.front());
}


void verify_connected_entities(const AddEndpoint::Request& req) {
    throw_if_connected_entity_empty(req);
    for (const auto& single_connected_entity : req.get_connected_entities()) {
        throw_if_wrong_entity_role(single_connected_entity);
        throw_if_identifier_not_empty(single_connected_entity);
    }
}


void set_ip_transport_detail_with_default(attribute::IpTransportDetail& ip_transport_detail) {
    attribute::Ipv4Address ipv4_address{};
    ipv4_address.set_address(get_ip_address_from_ethernet_interfaces().get_address());
    ip_transport_detail.set_ipv4_address(ipv4_address);
    ip_transport_detail.set_port(NvmeConfig::get_instance()->get_rdma_port());
    auto interface = get_ethernet_interface_uuid_from_ip_address(ipv4_address);
    if (std::get<0>(interface) == true) {
        ip_transport_detail.set_interface(std::get<1>(interface));
    }
    ip_transport_detail.set_transport_protocol(enums::TransportProtocol::RoCEv2);
}


void throw_if_wrong_rdma_port(std::uint32_t rdma_port) {
    if (rdma_port <= 0 || rdma_port > UINT16_MAX) {
        THROW(exceptions::InvalidValue, "nvme-agent", "RDMA port is out of rage, should be in <1,65535>.");
    }
}


// Entity link field - throw if empty or invalid
void throw_if_wrong_target_entity(const attribute::ConnectedEntity& single_connected_entity) {
    if (!single_connected_entity.get_entity().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Missing entity in connected entities collection.");
    }
    get_manager<Volume>().get_entry(single_connected_entity.get_entity().value());
    log_debug("nvme-agent", "Connected entity Volume " + single_connected_entity.get_entity().value());
}


void throw_if_entity_link_not_empty(const attribute::ConnectedEntity& single_connected_entity) {
    if (single_connected_entity.get_entity().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-agent",
              "Invalid value in Entity property, this field should be empty.");
    }
}


void throw_if_ethernet_interface_not_empty(const attribute::IpTransportDetail& single_ip_transport_detail) {
    if (single_ip_transport_detail.get_interface().has_value()) {
        THROW(exceptions::InvalidValue, "nvme-agent",
              "Invalid value in Interface property, this field should be empty.");
    }
}


std::string get_nqn(const AddEndpoint::Request& req) {
    std::string nqn{};
    nqn = req.get_identifiers().front().get_durable_name();
    return nqn;
}


void throw_if_ip_transport_details_more_than_one(const AddEndpoint::Request& requset) {
    if (requset.get_ip_transport_details().size() > 1) {
        THROW(exceptions::InvalidValue, "nvme-agent", "Only one IP transport detail is supported.");
    }
}


void throw_if_volume_in_use(const Volume& volume) {
    if (tools::is_volume_in_endpoint(volume)) {
        log_error("nvme-agent", "Volume " << volume.get_uuid() << " is already used by an endpoint.");
        THROW(exceptions::InvalidValue, "nvme-agent", "Volume is already used by an endpoint.");
    }
}


void set_rdma_port(const attribute::IpTransportDetail& ip_transport_detail_req,
                   attribute::IpTransportDetail& ip_transport_detail) {
    if (ip_transport_detail_req.get_port().has_value()) {
        throw_if_wrong_rdma_port(ip_transport_detail_req.get_port().value());
        ip_transport_detail.set_port(std::uint16_t(ip_transport_detail_req.get_port().value()));
    }
    else {
        unsigned int rdma_port = NvmeConfig::get_instance()->get_rdma_port();
        throw_if_wrong_rdma_port(rdma_port);
        ip_transport_detail.set_port(std::uint16_t(rdma_port));
    }
}


void set_ipv4_address(const attribute::IpTransportDetail& ip_transport_detail_req,
                      attribute::IpTransportDetail& ip_transport_detail) {

    std::string address{};
    if (ip_transport_detail_req.get_interface().has_value()) {
        address = get_ip_address_from_ethernet_interface_uuid(ip_transport_detail_req.get_interface().value());
        log_debug("nvme-agent", "Setting IP address: " << address
                                                       << " from requested interface "
                                                       << ip_transport_detail_req.get_interface());
    }
    else if (ip_transport_detail_req.get_ipv4_address().get_address().has_value()) {
        address = ip_transport_detail_req.get_ipv4_address().get_address().value();
        log_debug("nvme-agent", "Setting IP address: " << address << " from request");
    }
    else {
        address = get_ip_address_from_ethernet_interfaces().get_address();
        log_debug("nvme-agent", "Setting default IP address: " << address);
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
        log_debug("nvme-agent",
                  "Setting interface " << ip_transport_detail_req.get_interface().value() << " from request");
        return;
    }

    if (ip_transport_detail_req.get_ipv4_address().get_address().has_value()) {
        bool result{};
        Uuid found_interface{};

        std::tie(result, found_interface) =
            get_ethernet_interface_uuid_from_ip_address(ip_transport_detail_req.get_ipv4_address());

        if (result == true) {
            log_debug("nvme-agent", "Setting interface " << found_interface
                                                         << " based on IP address "
                                                         << ip_transport_detail_req.get_ipv4_address().get_address().value());
            ip_transport_detail.set_interface(found_interface);
            return;
        }
    }

    log_debug("nvme-agent", "Setting default interface...");
    auto interface = get_ethernet_interface_uuid_from_ip_address(get_ip_address_from_ethernet_interfaces());
    if (std::get<0>(interface) == true) {
        ip_transport_detail.set_interface(std::get<1>(interface));
    }
}


void set_ipv4_address_and_interface_with_verification(const attribute::IpTransportDetail& ip_transport_detail_req,
                                                      attribute::IpTransportDetail& ip_transport_detail) {

    const auto& ipv4_address = ip_transport_detail_req.get_ipv4_address();
    const auto& interface_from_request = ip_transport_detail_req.get_interface();

    if (ipv4_address.get_address().has_value()) {
        bool result{};
        Uuid found_interface{};
        std::tie(result, found_interface) = get_ethernet_interface_uuid_from_ip_address(ipv4_address);
        if (result == false) {
            THROW(exceptions::InvalidValue, "nvme-agent",
                  "There is no ethernet interface with requested IPv4 address.");
        }

        if (interface_from_request.has_value() && interface_from_request.value() != found_interface) {
            THROW(exceptions::InvalidValue, "nvme-agent",
                  "Requested ethernet interface does not match ethernet interface with IP from the request.");
        }

        log_debug("nvme-agent",
                  "Interface " << found_interface << " found with IP " << ipv4_address.get_address().value());
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
        ip_transport_detail.set_transport_protocol(enums::TransportProtocol::RoCEv2);
    }
}


void add_target_endpoint(AddEndpoint::ContextPtr ctx, const std::string& ip_address, std::uint16_t port,
                         const std::string& nqn, const Uuid& endpoint, const Volume& volume) {
    auto& endpoint_creator = NvmeConfig::get_instance()->get_endpoint_creator();
    try {

        std::string volume_path{};
        if (!volume.get_name().has_value()) {
            THROW(exceptions::NvmeError, "nvme-agent", "Cannot use Volume with unknown name.");
        }
        volume_path = sysfs::construct_dev_path(volume.get_name());

        auto nvme_port = endpoint_creator.create_target_endpoint(ctx, ip_address, port, nqn, volume_path);
        NvmeConfig::get_instance()->set_endpoint_nvme_port(endpoint, nvme_port);

        // Adding to database
        EndpointDatabase db{endpoint};
        db.put(EndpointReader::DATABASE_NVME_PORT, std::to_string(int(nvme_port)));
        db.put(EndpointReader::DATABASE_RDMA_PORT, std::to_string(static_cast<unsigned int>(port)));
        db.put(EndpointReader::DATABASE_ROLE, model::literals::Endpoint::TARGET);
        db.put(EndpointReader::DATABASE_NQN, nqn);
        db.put(EndpointReader::DATABASE_IPV4, ip_address);
        db.put(EndpointReader::DATABASE_NVME_DEVICE_UUID, volume.get_uuid());
    }
    catch (const std::logic_error&) {
        THROW(exceptions::NvmeError, "nvme-agent",
              "Unknown system path for Volume " + volume.get_uuid());
    }
    catch (const std::exception& error) {
        THROW(exceptions::NvmeError, "nvme-agent",
              std::string("Failed adding target endpoint: ") + error.what());
    }
}


void add_endpoint(AddEndpoint::ContextPtr ctx, const AddEndpoint::Request& req, AddEndpoint::Response& rsp) {
    log_info("nvme-agent", "Adding endpoint...");
    NvmeStabilizer ns{};
    const auto& fabric{req.get_fabric()};

    get_manager<Fabric>().get_entry(fabric);
    Endpoint endpoint{fabric};

    verify_connected_entities(req);
    verify_identifiers(req);
    std::string nqn = get_nqn(req);

    // tree stability requires NQN to be already set
    endpoint.add_identifier({nqn, enums::IdentifierType::NQN});
    Uuid uuid = ns.stabilize(endpoint);

    throw_if_endpoint_exist(uuid);

    endpoint.add_identifier({uuid, enums::IdentifierType::UUID});
    endpoint.set_protocol(enums::TransportProtocol::NVMeOverFabrics);
    endpoint.set_connected_entities(req.get_connected_entities());
    endpoint.set_status({enums::State::Enabled, enums::Health::OK});
    endpoint.set_oem(req.get_oem());
    tools::convert_to_subnqn(nqn);


    if (Endpoint::is_target(endpoint)) { // Target role

        throw_if_wrong_target_entity(req.get_connected_entities().front());

        attribute::IpTransportDetail ip_transport_detail{};

        if (req.get_ip_transport_details().empty()) { // IP transport details is not provided
            log_debug("nvme-agent", "No IP transport details provided - using default interface");
            set_ip_transport_detail_with_default(ip_transport_detail);
            endpoint.add_ip_transport_detail(ip_transport_detail);
        }
        else { // IP transport details is provided

            throw_if_ip_transport_details_more_than_one(req);
            const auto ip_transport_detail_req = req.get_ip_transport_details().front();

            set_rdma_port(ip_transport_detail_req, ip_transport_detail);
            set_ipv4_address_and_interface_with_verification(ip_transport_detail_req, ip_transport_detail);
            set_transport_protocol(ip_transport_detail_req, ip_transport_detail);

            endpoint.add_ip_transport_detail(ip_transport_detail);
        }

        if (endpoint.get_connected_entities().front().get_lun().has_value()) {
            THROW(exceptions::InvalidValue, "nvme-agent", "Specifying Logical unit number is not supported for NVMeoF storage.");
        }
        const Volume volume = get_manager<Volume>()
            .get_entry(endpoint.get_connected_entities().front().get_entity().value());
        throw_if_volume_in_use(volume);

        add_target_endpoint(ctx,
                            ip_transport_detail.get_ipv4_address().get_address(),
                            uint16_t(ip_transport_detail.get_port()),
                            nqn,
                            endpoint.get_uuid(),
                            volume);
        FabricDatabase(fabric).append(NvmeDatabase::ENDPOINTS, uuid);
    }
    else { // EntityRole::Initiator Endpoint
        throw_if_entity_link_not_empty(req.get_connected_entities().front());
        if (!req.get_ip_transport_details().empty()) {
            throw_if_ip_transport_details_more_than_one(req);
            throw_if_ethernet_interface_not_empty(req.get_ip_transport_details().front());
        }

        // add initiator nqn to hosts configuration
        ctx->nvme_target_handler->add_host(nqn);
        FabricDatabase(fabric).append(NvmeDatabase::ENDPOINTS, uuid);
        EndpointDatabase(uuid).put(EndpointReader::DATABASE_ROLE, model::literals::Endpoint::INITIATOR);
        EndpointDatabase(uuid).put(EndpointReader::DATABASE_NQN, nqn);
    }

    rsp.set_endpoint(uuid);
    get_manager<Endpoint>().add_entry(endpoint);
    log_info("nvme-agent", "Added endpoint with UUID '" + uuid + "'");
}
}

REGISTER_NVME_COMMAND(AddEndpoint, ::add_endpoint);
