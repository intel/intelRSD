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
#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"

#include "discovery/builders/spdk_endpoint_builder.hpp"
#include "spdk_stabilizer.hpp"
#include "spdk_converter.hpp"
#include "spdk_agent_commands.hpp"



using namespace agent::spdk;
using namespace agent_framework;
using namespace agent_framework::database;
using namespace agent_framework::model;

namespace {

bool check_if_endpoint_exists(const std::string& nqn) {
    auto endpoints = module::get_manager<Endpoint>().get_entries([nqn](const Endpoint& endpoint) -> bool {
        return attribute::Identifier::get_nqn(endpoint) == nqn;
    });
    return !endpoints.empty();
}


void throw_if_durable_name_is_in_wrong_format(const std::string& identifier) {
    if (0 != identifier.find(literals::Endpoint::NQN_FORMAT)) {
        THROW(exceptions::InvalidValue, "spdk-agent", "Durable name is not in NQN format. Missing prefix '" +
                                                      std::string{literals::Endpoint::NQN_FORMAT} + "'.");
    }
}


std::vector<attribute::IpTransportDetail> make_transport_details_from_interface(AddEndpoint::ContextPtr context, const Uuid& interface_uuid) {
    std::vector<attribute::IpTransportDetail> ip_transport_details{};

    auto interface = module::get_manager<NetworkInterface>().get_entry(interface_uuid);
    for (const auto& ipv4 : interface.get_ipv4_addresses()) {
        attribute::IpTransportDetail transport{};

        transport.set_ipv4_address(ipv4.get_address());
        transport.set_transport_protocol(enums::TransportProtocol::RoCEv2);
        transport.set_interface(interface_uuid);

        transport.set_port(context->configuration->get_service_port());

        ip_transport_details.emplace_back(std::move(transport));
    }
    return ip_transport_details;
}


NetworkInterface find_interface(const attribute::Ipv4Address& ipv4_address) {
    auto interfaces = module::get_manager<NetworkInterface>().get_entries(
        [&ipv4_address](const NetworkInterface& network_interface) -> bool {
            for (const auto& ipv4 : network_interface.get_ipv4_addresses()) {
                if (ipv4.get_address() == ipv4_address.get_address()) {
                    return true;
                }
            }
            return false;
        });

    if (interfaces.empty()) {
        THROW(agent_framework::exceptions::InvalidValue, "spdk-agent",
              "Network Interface with given IPv4 address not found.");
    }
    else {
        return interfaces.front();
    }
}

void save_initiator_to_db(const Endpoint& initiator) {
    try {
        EndpointEntity db{initiator.get_uuid()};
        db.put(ENDPOINT_NQN_PROPERTY, attribute::Identifier::get_nqn(initiator));
        db.put(ENDPOINT_ROLE_PROPERTY, model::literals::Endpoint::INITIATOR);
    }
    catch (const std::exception& exception) {
        THROW(exceptions::SpdkError, "spdk-agent",
              "Could not save initiator endpoint to database: " + std::string{exception.what()});
    }
}


::utils::transaction::BaseTransactionHandler::FunctionPair
construct_transaction_handler(AgentContext::SPtr context, const std::string& subsystem_nqn) {

    auto to_commit = [context, subsystem_nqn]() {
        try {
            log_debug("spdk-agent", "Calling nvmf subsystem create method (SPDK version 18.10.1 or higher)...");
            bool success = context->spdk_api->nvmf_subsystem_create(subsystem_nqn,
                                                                    agent::spdk::utils::SpdkConverter::RSD_SERIAL_NUMBER,
                                                                    false);
            log_debug("spdk-agent", "NVMf subsystem create call was successful");
            if (!success) {
                throw std::runtime_error("SPDK Construct NVMf subsystem returned 'false'.");
            }
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                  "Failed to construct NVMf subsystem: " + std::string{exception.what()});
        }
    };

    auto to_rollback = [context, subsystem_nqn]() {
        try {
            bool success = context->spdk_api->delete_nvmf_subsystem(subsystem_nqn);
            if (!success) {
                throw std::runtime_error("SPDK delete NVMf subsystem returned 'false'.");
            }
            log_debug("spdk-agent", "[Rollback] Delete NVMf subsystem call was successful");
        }
        catch (const std::exception& exception) {
            // If rollback failed, we can do nothing
            log_warning("spdk-agent", "Could not rollback construct NVMf subsystem operation: " << exception.what());
        }
    };

    return std::make_pair(to_commit, to_rollback);
}

::utils::transaction::BaseTransactionHandler::FunctionPair
add_listener_transaction_handler(AgentContext::SPtr context,
                                 const std::string& subsystem_nqn, const attribute::IpTransportDetail& transport) {

    auto to_commit = [context, subsystem_nqn, transport]() {
        try {
            auto listen_address = agent::spdk::utils::SpdkConverter::convert_to_listen_address(transport);
            bool success = context->spdk_api->nvmf_subsystem_add_listener(subsystem_nqn, listen_address);
            log_debug("spdk-agent", "NVMf subsystem add listener call was successful");
            if (!success) {
                throw std::runtime_error("SPDK NVMf subsystem add listener returned 'false'.");
            }
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                  "Failed to apply transport details to NVMf subsystem: " + std::string{exception.what()});
        }
    };

    auto to_rollback = [transport]() {
        // Nothing to do as subsystem will be removed during last rollback
        log_debug("spdk-agent", "Rollback of 'add listener' method with IP: "
            << transport.get_ipv4_address().get_address().value_or(std::string{}));
    };

    return std::make_pair(to_commit, to_rollback);
}

::utils::transaction::BaseTransactionHandler::FunctionPair
add_namespace_transaction_handler(AgentContext::SPtr context,
                                  const std::string& subsystem_nqn, const attribute::ConnectedEntity& entity) {

    auto volume = module::get_manager<Volume>().get_entry(entity.get_entity());
    auto to_commit = [context, subsystem_nqn, volume]() {
        auto nvmf_namespace = agent::spdk::utils::SpdkConverter::convert_volume_to_namespace(volume);

        try {
            context->spdk_api->nvmf_subsystem_add_ns(subsystem_nqn, nvmf_namespace);
            log_debug("spdk-agent", "NVMf subsystem add namespace call was successful");
        }
        catch (const std::exception& exception) {
            THROW(exceptions::SpdkError, "spdk-agent",
                  "Failed to apply new namespace to NVMf subsystem: " + std::string{exception.what()});
        }
    };

    auto to_rollback = [volume]() {
        auto nvmf_namespace = agent::spdk::utils::SpdkConverter::convert_volume_to_namespace(volume);
        // Nothing to do as subsystem will be removed during last rollback
        log_debug("spdk-agent", "Rollback of 'add namespace' method for " << nvmf_namespace.get_bdev_name());
    };

    return std::make_pair(to_commit, to_rollback);
}


void construct_nvmf_subsystem(AgentContext::SPtr context, Endpoint& target) {
    std::string nvmf_subsystem_nqn = attribute::Identifier::get_nqn(target);

    std::lock_guard<std::mutex> lock{context->spdk_mutex};
    auto transaction_handler = context->transaction_handler_factory->get_handler();

    transaction_handler->add_handler_pair(construct_transaction_handler(context, nvmf_subsystem_nqn));

    for (const auto& transport_detail : target.get_ip_transport_details()) {
        transaction_handler->add_handler_pair(
            add_listener_transaction_handler(context, nvmf_subsystem_nqn, transport_detail));
    }

    for (auto& entity : target.get_connected_entities()) {
        transaction_handler->add_handler_pair(
            add_namespace_transaction_handler(context, nvmf_subsystem_nqn, entity));
    }

    try {
        transaction_handler->commit();
    }
    catch (const std::exception&) {
        transaction_handler->rollback();
        throw;
    }
}


void add_endpoint(AddEndpoint::ContextPtr context, const AddEndpoint::Request& request, AddEndpoint::Response& response) {
    log_info("spdk-agent", "Adding endpoint...");
    const auto& fabric_uuid = request.get_fabric();
    module::get_manager<Fabric>().get_entry(fabric_uuid);
    auto endpoint = agent::spdk::discovery::SpdkEndpointBuilder::build_default(fabric_uuid);

    const auto& identifiers = request.get_identifiers();
    if (identifiers.size() != 1) {
        THROW(exceptions::InvalidValue, "spdk-agent", "Endpoint's identifier must be exactly one.");
    }

    std::string nqn{};
    try {
        nqn = attribute::Identifier::get_nqn(request);
        throw_if_durable_name_is_in_wrong_format(nqn);
        agent_framework::discovery::IdentifierBuilder::set_nqn(endpoint, nqn);
    }
    catch (const std::logic_error&) {
        THROW(exceptions::InvalidValue, "spdk-agent", "Endpoint cannot be created without NQN.");
    }

    if (check_if_endpoint_exists(nqn)) {
        THROW(exceptions::InvalidValue, "spdk-agent", "Endpoint with given NQN already exists.");
    }

    if (request.get_password().has_value()) {
        THROW(exceptions::UnsupportedField, "spdk-agent", "Password is not supported by SPDK.",
              literals::Endpoint::PASSWORD, request.get_password());
    }
    if (request.get_username().has_value()) {
        THROW(exceptions::UnsupportedField, "spdk-agent", "Username is not supported by SPDK.",
              literals::Endpoint::USERNAME, request.get_username());
    }

    if (request.get_connected_entities().empty()) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "At least one connected entity needs to be given.");
    }
    if (request.get_connected_entities().front().get_lun().has_value()) {
        THROW(exceptions::InvalidValue, "spdk-agent", "Specifying Logical unit number is not supported for SPDK.");
    }
    auto entity_role = request.get_connected_entities().front().get_entity_role();
    if (entity_role == enums::EntityRole::Initiator && !request.get_ip_transport_details().empty()) {
        THROW(exceptions::InvalidValue, "spdk-agent",
              "Initiator shouldn't have specified transport details.");
    }

    for (const auto& connected_entity : request.get_connected_entities()) {
        if (connected_entity.get_entity_role() != enums::EntityRole::Target &&
            connected_entity.get_entity_role() != enums::EntityRole::Initiator) {
            THROW(exceptions::InvalidValue, "spdk-agent",
                  "Only connected entity role Target or Initiator is supported.");
        }

        if (connected_entity.get_entity_role() != entity_role) {
            THROW(exceptions::InvalidValue, "spdk-agent",
                  "Connected entities in one Endpoint must have the same role.");
        }

        if (connected_entity.get_entity_role() == enums::EntityRole::Target) {
            if (!connected_entity.get_entity().has_value()) {
                THROW(exceptions::InvalidValue, "spdk-agent", "Entity entry is required.");
            }

            if (!module::get_manager<Volume>().entry_exists(connected_entity.get_entity())) {
                THROW(exceptions::InvalidValue, "spdk-agent", "Volume does not exist.");
            }
            if (Volume::is_volume_connected_to_endpoint(connected_entity.get_entity())) {
                THROW(exceptions::InvalidValue, "spdk-agent", "Requested volume is already in use.");
            }
        }
        endpoint.add_connected_entity(connected_entity);
    }

    if (!request.get_ip_transport_details().empty()) {
        for (const auto& ip_transport_details : request.get_ip_transport_details()) {
            if (ip_transport_details.get_ipv4_address().get_address().has_value() &&
                ip_transport_details.get_interface().has_value()) {

                if (!module::get_manager<NetworkInterface>().entry_exists(ip_transport_details.get_interface())) {
                    THROW(exceptions::InvalidValue, "spdk-agent", "Provided Network Interface does not exist.");
                }

                auto interface = module::get_manager<NetworkInterface>().get_entry(ip_transport_details.get_interface());

                auto ipv4_addresses = interface.get_ipv4_addresses();
                if (ipv4_addresses.empty()) {
                    THROW(exceptions::InvalidValue, "spdk-agent", "Provided interface has no IP address.");
                }

                if (ipv4_addresses.front().get_address() == ip_transport_details.get_ipv4_address().get_address()) {
                    auto transports = make_transport_details_from_interface(context, interface.get_uuid());
                    endpoint.set_ip_transport_details(transports);
                }
                else {
                    THROW(exceptions::InvalidValue, "spdk-agent",
                          "Provided ethernet interface does not match requested transport IPv4 address.");
                }
            }
            else if (ip_transport_details.get_interface().has_value()) {
                if (!module::get_manager<NetworkInterface>().entry_exists(ip_transport_details.get_interface())) {
                    THROW(exceptions::InvalidValue, "spdk-agent", "Provided Network Interface does not exist.");
                }

                auto transports = make_transport_details_from_interface(context, ip_transport_details.get_interface());
                endpoint.set_ip_transport_details(transports);
            }
            else if (ip_transport_details.get_ipv4_address().get_address().has_value()) {
                auto interface = find_interface(ip_transport_details.get_ipv4_address());
                auto transports = make_transport_details_from_interface(context, ip_transport_details.get_interface());
                endpoint.set_ip_transport_details(transports);
            }
        }
    }
    else {
        if (entity_role != enums::EntityRole::Initiator) {
            auto interfaces = module::get_manager<NetworkInterface>().get_entries();
            if (interfaces.empty()) {
                THROW(exceptions::InvalidValue, "spdk-agent", "There are no interfaces in the system.");
            }

            auto interface = interfaces.front();
            auto transports = make_transport_details_from_interface(context, interface.get_uuid());
            endpoint.set_ip_transport_details(transports);
        }
    }

    endpoint.set_oem(request.get_oem());
    agent::spdk::discovery::SpdkEndpointBuilder::nvmeof_protocol(endpoint);

    agent::spdk::SpdkStabilizer stabilizer{};
    stabilizer.stabilize(endpoint);

    // From now endpoint is validated and ready for creation on SPDK
    if (Endpoint::is_target(endpoint)) {
        construct_nvmf_subsystem(context, endpoint);
    }
    else {
        save_initiator_to_db(endpoint);
    }

    module::get_manager<Endpoint>().add_entry(endpoint);
    response.set_endpoint(endpoint.get_uuid());
    log_info("spdk-agent", "New endpoint created successfully: " << endpoint.get_uuid());
}
}

REGISTER_SPDK_COMMAND(AddEndpoint, ::add_endpoint);