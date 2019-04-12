/*!
 * @brief Implementation of endpoint reader class.
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
 * @file endpoint_reader.cpp
 */

#include "tools/endpoint_reader.hpp"
#include "loader/config.hpp"
#include "tree_stability/nvme_stabilizer.hpp"
#include "logger/logger_factory.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "tools/databases.hpp"
#include "sysfs/construct_dev_path.hpp"



using namespace agent::nvme::tools;
using namespace agent::nvme::loader;
using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace std;

const string EndpointReader::DATABASE_NVME_PORT = "nvme_port";
const string EndpointReader::DATABASE_RDMA_PORT = "rdma_port";
const string EndpointReader::DATABASE_NQN = "nqn";
const string EndpointReader::DATABASE_IPV4 = "ipv4";
const string EndpointReader::DATABASE_ROLE = "role";
const string EndpointReader::DATABASE_NVME_DEVICE_UUID = "nvme_device_uuid";

namespace {

const string TRANSPORT_TYPE_RDMA = "rdma";
const string ADDRESS_FAMILY_IPV4 = "ipv4";
const string ADDRESS_FAMILY_IPV6 = "ipv6";


template<typename T>
bool get_entity(const string& system_path, Uuid& entity) {
    for (const auto& uuid : get_manager<T>().get_keys()) {
        T entry = get_manager<T>().get_entry(uuid);
        if (entry.get_name().has_value()) {
            auto entry_path = sysfs::construct_dev_path(entry.get_name());
            if (system_path == entry_path) {
                entity = uuid;
                return true;
            }
        }
        else {
            log_warning("nvme-agent", "Entry '" << entry.get_uuid() << "' has no name.");
        }
    }
    return false;
}


void read_device(const std::string& device_path, Endpoint& endpoint) {
    log_debug("nvme-agent", "Found namespace with device path '" << device_path << "'");
    attribute::ConnectedEntity connected_entity{};
    Uuid entity{};
    if (!(get_entity<Drive>(device_path, entity) || get_entity<Volume>(device_path, entity))) {
        throw runtime_error("System path '" + device_path +
                            "' is not recognized as a drive nor a volume");
    }
    connected_entity.set_entity(entity);
    connected_entity.set_entity_role(enums::EntityRole::Target);
    endpoint.add_connected_entity(connected_entity);
}


void read_namespaces(std::shared_ptr<nvme_target::BaseNvmeTargetHandler> configurator,
                     const nvme_target::Id& subsystem, Endpoint& endpoint) {
    log_debug("nvme-agent", "Reading subsystem " + subsystem);
    bool no_namespaces{true};
    for (const auto& nspace : configurator->get_subsystem_namespaces(subsystem)) {
        try {
            log_debug("nvme-agent", "Reading namespace " + nspace);
            auto device_path = configurator->get_subsystem_namespace_device(subsystem, nspace);
            read_device(device_path, endpoint);
            no_namespaces = false;
        }
        catch (const exception& error) {
            log_error("nvme-agent", "Failed to read namespace " << nspace << ": " << error.what());
        }
    }
    if (no_namespaces) {
        throw runtime_error("No namespaces defined in subsystem " + subsystem);
    }
}


template<typename T>
void link_network_interface(const string& address, attribute::IpTransportDetail& transport_detail,
                            function<const T&(const NetworkInterface&)> get_addresses) {
    // find network interfaces with matching IP addresses
    const auto& network_interfaces = get_manager<NetworkInterface>().get_entries(
        [&address, get_addresses](const NetworkInterface& ni) {
            const auto& iterface_addresses = get_addresses(ni).get_array();
            // decay removes const & from iterface_addresses which may be vectors ipv4 or ipv6
            return std::any_of(std::begin(iterface_addresses), std::end(iterface_addresses),
                               [&address](
                                   typename std::decay<decltype(iterface_addresses)>::type::const_reference ip_addr) {
                                   auto addr = ip_addr.get_address();
                                   return addr.has_value() && (addr == address);
                               });
        });

    if (network_interfaces.size() != 1) {
        std::stringstream error{};
        error << "Found " << network_interfaces.size() << " interfaces, expected 1";
        throw runtime_error(error.str());
    }

    transport_detail.set_interface(network_interfaces.front().get_uuid());
}


void read_transport(const nvme_target::PortParams& port_params, Endpoint& endpoint) {
    // extract port parameters
    std::string address;
    std::string family;
    std::string type;
    uint16_t port{};
    std::tie(address, port, family, type) = port_params;

    attribute::IpTransportDetail transport_detail{};

    log_debug("nvme-agent", "Transport discovered with address " << address
                                                                 << " type " << type << " port " << port << " family "
                                                                 << family);
    if (TRANSPORT_TYPE_RDMA != type) {
        throw runtime_error("Non-RDMA transport protocol detected: " + type);
    }
    transport_detail.set_transport_protocol(enums::TransportProtocol::RoCEv2);
    if (ADDRESS_FAMILY_IPV4 == family) {
        transport_detail.set_ipv4_address({address});
        link_network_interface<NetworkInterface::Ipv4Addresses>(address, transport_detail,
                                                                [](const NetworkInterface& ni) -> const NetworkInterface::Ipv4Addresses& {
                                                                    return ni.get_ipv4_addresses();
                                                                });
    }
    else if (ADDRESS_FAMILY_IPV6 == family) {
        attribute::Ipv6Address ipv6_address{};
        ipv6_address.set_address(address);
        transport_detail.set_ipv6_address(ipv6_address);
        link_network_interface<NetworkInterface::Ipv6Addresses>(address, transport_detail,
                                                                [](const NetworkInterface& ni) -> const NetworkInterface::Ipv6Addresses& {
                                                                    return ni.get_ipv6_addresses();
                                                                });
    }
    else {
        throw runtime_error("Unknown address family detected: " + family);
    }

    transport_detail.set_port(port);
    endpoint.add_ip_transport_detail(transport_detail);
}


void read_persistent_attributes(Database::Ptr db, uint16_t& nvme_port) {
    auto port_str{db->get(EndpointReader::DATABASE_NVME_PORT)};
    auto number = stoi(port_str);
    if ((number > 0) && (number <= UINT16_MAX)) {
        nvme_port = uint16_t(number);
    }
    else {
        throw runtime_error("NVMe port is out of range: " + port_str);
    }
}


void read_endpoint(std::shared_ptr<NvmeAgentContext> ctx, const nvme_target::PortParams& port_params,
                   const nvme_target::Id& subsystem, Endpoint& endpoint) {
    NvmeStabilizer ns{};
    uint16_t nvme_port{};
    auto configurator = ctx->nvme_target_handler;

    log_debug("nvme-agent", "Reading endpoint " << subsystem);
    string nqn = agent_framework::model::literals::Endpoint::NQN_FORMAT + subsystem;
    read_namespaces(configurator, subsystem, endpoint);
    read_transport(port_params, endpoint);
    endpoint.set_status({enums::State::Enabled, enums::Health::OK});
    endpoint.set_protocol(enums::TransportProtocol::NVMeOverFabrics);
    endpoint.add_identifier({nqn, enums::IdentifierType::NQN});
    ns.stabilize(endpoint);
    auto db = ctx->db_factory->create_database(Database::Types::ENDPOINT, endpoint.get_uuid());
    read_persistent_attributes(db, nvme_port);
    endpoint.add_identifier({endpoint.get_uuid(), enums::IdentifierType::UUID});
    NvmeConfig::get_instance()->set_endpoint_nvme_port(endpoint.get_uuid(), nvme_port);
}
}


EndpointReader::~EndpointReader() {}


BaseEndpointReader::EndpointContainer
EndpointReader::read_endpoints(std::shared_ptr<NvmeAgentContext> ctx, const Uuid& fabric) {
    auto configurator = ctx->nvme_target_handler;
    BaseEndpointReader::EndpointContainer endpoints;
    // for each subsystem under each port read the existing endpoint
    for (const auto& port : configurator->get_ports()) {
        // read_port_parameters
        auto port_params = configurator->get_port_params(port);
        log_debug("nvme-agent", "Reading port " + port);
        for (const auto& subsystem : configurator->get_port_subsystems(port)) {
            try {
                Endpoint endpoint{fabric};
                read_endpoint(ctx, port_params, subsystem, endpoint);
                endpoints.push_back(endpoint);
            }
            catch (const exception& error) {
                log_error("nvme-agent", "Failed reading endpoint " << port << "/" << subsystem + ": " << error.what());
            }
        }
    }

    return endpoints;
}
