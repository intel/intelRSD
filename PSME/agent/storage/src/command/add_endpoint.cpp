/*!
 * @brief Implementation of AddEndpoint command.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file add_endpoint.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/managers/iscsi_target_manager.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "iscsi/utils.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace agent::storage;
using namespace database;

namespace {

using ConnectedEntities = attribute::Array<attribute::ConnectedEntity>;
static const constexpr char EMPTY_VALUE[] = "";
static const constexpr char DEFAULT_LUN[] = "1";


bool is_lun_correct(const std::string& s) {
    if (!s.empty() && std::all_of(s.begin(), s.end(), isdigit)) {
        try {
            if (static_cast<std::uint32_t>(std::stoul(s)) > 0) {
                return true;
            }
        }
        catch (const std::out_of_range&) {
            return false;
        }
    }
    return false;
}

void extract_iqn(const AddEndpoint::Request& request, const enums::EntityRole& entity_role, std::string& iqn) {
    const auto& identifiers = request.get_identifiers();
    if (1 != identifiers.size()) {
        THROW(exceptions::InvalidValue, "storage-agent", "Exactly one identifier (iQN) is expected.");
    }

    try {
        iqn = attribute::Identifier::get_iqn(request);
    }
    catch (const std::logic_error&) {
        THROW(exceptions::InvalidValue, "storage-agent", "iQN identifier needs to be given.");
    }

    if (entity_role == enums::EntityRole::Target) {
        if (iqn.empty()) {
            THROW(exceptions::InvalidValue, "storage-agent", "Target Endpoint iQN cannot be empty.");
        }
        for (const auto& endpoint : get_manager<Endpoint>().get_entries()) {
            if (iqn == attribute::Identifier::get_iqn(endpoint)) {
                THROW(exceptions::InvalidValue, "storage-agent", "Endpoint with iQN: " + iqn + " already exists.");
            }
        }
    }
    log_debug("storage-agent", "iQN read from request: " << iqn);
}


void extract_role(const AddEndpoint::Request& request, enums::EntityRole& entity_role) {
    const auto& connected_entities = request.get_connected_entities();
    if (connected_entities.empty()) {
        THROW(exceptions::InvalidValue, "storage-agent",
              "At least one connected entity needs to be given.");
    }

    const auto& role = connected_entities[0].get_entity_role();
    if (enums::EntityRole::Target != role && enums::EntityRole::Initiator != role) {
        THROW(exceptions::InvalidValue, "storage-agent",
              "Only connected entity role Target or Initiator is supported.");
    }
    bool pass_first = true;
    for (const auto& connected_entity : connected_entities) {
        if (pass_first) {
            pass_first = false;
            continue;
        }
        if (role != connected_entity.get_entity_role()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Connected entities in one Endpoint must have the same role.");
        }
        if (role == enums::EntityRole::Initiator) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Initiator Endpoint must have only one connected entity.");
        }
    }

    entity_role = role.value();
    log_debug("storage-agent", "Got Endpoint's connected entity role " << entity_role.to_string());
}


void process_connected_entities(const enums::EntityRole& entity_role, ConnectedEntities& connected_entities) {
    if (enums::EntityRole::Initiator == entity_role) {
        // It must have index 0, otherwise would fail at function extract_role(...)
        if (connected_entities[0].get_entity().has_value()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Connected Entity of Initiator Endpoint must not have any linked entity.");
        }
        if (!connected_entities[0].get_identifiers().empty()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Connected Entity of Initiator Endpoint must not have Identifiers.");
        }
    }
    else {
        std::vector<std::string> luns{};
        std::vector<Uuid> volumes{};
        std::string entity_lun{};
        for (auto& entity : connected_entities) {
            if (!entity.get_entity().has_value()) {
                THROW(exceptions::InvalidValue, "storage-agent", "Entity is required for Target Endpoint.");
            }
            if (!get_manager<Volume>().entry_exists(entity.get_entity().value())) {
                THROW(exceptions::InvalidValue, "storage-agent",
                      "Volume linked in connected entity cannot be found: " + entity.get_entity().value());
            }

            volumes.emplace_back(std::move(entity.get_entity()));
            // If there is only 1 ConnectedEntity, then LUN Identifier is not obligatory (will be set to "1" if not given)
            if (connected_entities.size() > 1) {
                try {
                    entity_lun = attribute::Identifier::get_lun(entity);
                }
                catch (const std::logic_error&) {
                    THROW(agent_framework::exceptions::InvalidValue, "storage-agent", "LUN Identifier must be set.");
                }

                if (!is_lun_correct(entity_lun)) {
                    THROW(exceptions::InvalidValue, "storage-agent", "LUN Identifier must contain a positive number.");
                }
                luns.emplace_back(std::move(entity_lun));
            }
            else {
                if (entity.get_identifiers().size() > 1) {
                    THROW(exceptions::InvalidValue, "storage-agent",
                          "Connected Entity must have only one LUN Identifier.");
                }
                try {
                    entity_lun = attribute::Identifier::get_lun(entity);
                    if (!is_lun_correct(entity_lun)) {
                        THROW(exceptions::InvalidValue, "storage-agent",
                              "LUN Identifier must contain a positive number or not be set.");
                    }
                }
                catch (const std::logic_error&) {
                    attribute::Identifier lun{};
                    lun.set_durable_name_format(enums::IdentifierType::LUN);
                    lun.set_durable_name(DEFAULT_LUN);
                    entity.add_identifier(std::move(lun));
                }
            }

            // Different ConnectedEntities cannot point to the same Entity (in this case a Volume)
            for (const auto& endpoint : get_manager<Endpoint>().get_entries()) {
                for (const auto& entity2 : endpoint.get_connected_entities()) {
                    if (entity2.get_entity() == entity.get_entity()) {
                        THROW(exceptions::InvalidValue, "storage-agent",
                              "Volume in Connected Entity is already used in another Endpoint.");
                    }
                }
            }
        }
        std::sort(volumes.begin(), volumes.end());
        if (std::unique(volumes.begin(), volumes.end()) != volumes.end()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Connected Entities must not have duplicated linked volume.");
        }
        std::sort(luns.begin(), luns.end());
        if (std::unique(luns.begin(), luns.end()) != luns.end()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "Connected Entities must not have duplicated LUN Identifiers.");
        }
    }
}

NetworkInterface extract_interface(const attribute::IpTransportDetail& transport) {
    OptionalField<Uuid> interface_from_ip{};
    if (transport.get_ipv4_address().get_address().has_value()) {
        auto address_from_request = transport.get_ipv4_address().get_address().value();
        auto interfaces = get_manager<NetworkInterface>()
            .get_keys([address_from_request](const NetworkInterface& nic) {
                for (const auto& ipv4 : nic.get_ipv4_addresses()) {
                    if (ipv4.get_address().has_value() && ipv4.get_address() == address_from_request) {
                        return true;
                    }
                }
                return false;
            });

        if (interfaces.empty()) {
            THROW(exceptions::InvalidValue, "storage-agent",
                  "There is no ethernet interface with requested IP address!");
        }
        interface_from_ip = interfaces.front();
    }

    OptionalField<Uuid> interface_from_request = transport.get_interface();
    if (interface_from_request.has_value()) {
        if (!get_manager<NetworkInterface>().entry_exists(interface_from_request.value())) {
            THROW(exceptions::InvalidValue, "storage-agent", "Requested ethernet interface does not exist.");
        }

        // If both IP address and interface UUID is provided, they have to point to the same instance
        if (interface_from_ip.has_value()) {
            if (interface_from_ip.value() != interface_from_request.value()) {
                THROW(exceptions::InvalidValue, "storage-agent",
                      "Requested ethernet interface does not match ethernet interface with IP from the request.");
            }
        }

        // If everything is OK, use requested interface
        log_debug("storage-agent", "Use interface " << interface_from_request.value() << " from request");
        return get_manager<NetworkInterface>().get_entry(interface_from_request.value());
    }
    else {
        // When interface UUID was not send, use interface matching IP
        log_debug("storage-agent", "Use interface " << interface_from_ip.value() << " from IP");
        return get_manager<NetworkInterface>().get_entry(interface_from_ip.value());
    }
}

void process_ip_transport_details(const AddEndpoint::Request& request, Endpoint& endpoint) {
    const auto& requested_transport_details = request.get_ip_transport_details();

    // If IP transport details not provided, use default portal interface
    if (requested_transport_details.empty()) {
        attribute::IpTransportDetail ip_transport_detail{};
        ip_transport_detail.set_transport_protocol(enums::TransportProtocol::iSCSI);

        const auto& iscsi_data = get_manager<IscsiTarget, managers::IscsiTargetManager>().get_iscsi_data();
        auto interfaces = get_manager<NetworkInterface>().get_keys([&iscsi_data](const NetworkInterface interface) {
            return interface.get_name() == iscsi_data.get_portal_interface();
        });

        ip_transport_detail.set_ipv4_address({iscsi_data.get_portal_ip()});
        ip_transport_detail.set_port(iscsi_data.get_portal_port());

        if (!interfaces.empty()) {
            ip_transport_detail.set_interface(interfaces.front());
        }
        else {
            log_warning("storage-agent", "Cannot find iSCSI portal interface "
                << iscsi_data.get_portal_interface() << " in the system's interfaces");
        }

        endpoint.add_ip_transport_detail(std::move(ip_transport_detail));
    } else {
        const auto& iscsi_data = get_manager<IscsiTarget, managers::IscsiTargetManager>().get_iscsi_data();
        for (const auto& transport : requested_transport_details) {
            if (transport.get_ipv6_address().get_address().has_value()) {
                THROW(exceptions::InvalidValue, "storage-agent", "IPv6 is not supported for transport details.");
            }
            if (transport.get_port().has_value() && transport.get_port().value() != iscsi_data.get_portal_port()) {
                THROW(exceptions::InvalidValue, "storage-agent", "Invalid port provided.");
            }
            if (transport.get_transport_protocol().has_value() &&
                transport.get_transport_protocol().value() != enums::TransportProtocol::iSCSI) {
                THROW(exceptions::InvalidValue, "storage-agent", "Only iSCSI is supported as transport protocol.");
            }

            auto interface = extract_interface(transport);

            attribute::IpTransportDetail ip_transport_detail{};
            ip_transport_detail.set_transport_protocol(enums::TransportProtocol::iSCSI);

            ip_transport_detail.set_ipv4_address(interface.get_ipv4_addresses().front());
            ip_transport_detail.set_port(iscsi_data.get_portal_port());
            ip_transport_detail.set_interface(interface.get_uuid());

            endpoint.add_ip_transport_detail(std::move(ip_transport_detail));
        }
    }
}


void check_credentials(const AddEndpoint::Request& request) {
    if (request.get_username().has_value() != request.get_password().has_value()) {
        THROW(exceptions::InvalidValue, "storage-agent", "Both username and password must be either set or empty.");
    }

    if (request.get_username().has_value() && iscsi::tgt::is_endpoint_username_in_use(request.get_username())) {
        THROW(exceptions::InvalidValue, "storage-agent", "Given username is already in use in another endpoint.");
    }
}


void store_persistent_attributes(const Uuid& endpoint_uuid, const std::string& iqn, const enums::EntityRole& role,
                                 const std::string& username, const ConnectedEntities& connected_entities) {
    auto& pa = PersistentAttributes::get_instance();

    Aggregate<PersistentAttributes> aggr_endpoint{pa, endpoint_uuid, literals::Endpoint::ENDPOINT};
    aggr_endpoint.put(literals::Identifier::DURABLE_NAME, iqn);
    if (!username.empty()) {
        aggr_endpoint.put(literals::Endpoint::USERNAME, username);
    }
    if (enums::EntityRole::Initiator == role) {
        Aggregate<PersistentAttributes> aggr_entity{pa, Resource::make_uuid(), literals::ConnectedEntity::ENTITY};
        aggr_entity.put(literals::ConnectedEntity::ROLE, role.to_string());
        aggr_entity.dump();

        aggr_endpoint.append(literals::ConnectedEntity::ENTITY, aggr_entity.get_uuid());
    }
    else if (enums::EntityRole::Target == role) {
        for (const auto& entity : connected_entities) {
            Aggregate<PersistentAttributes> aggr_entity{pa, Resource::make_uuid(), literals::ConnectedEntity::ENTITY};
            aggr_entity.put(literals::ConnectedEntity::ROLE, role.to_string());
            aggr_entity.put(literals::ConnectedEntity::IDENTIFIERS, attribute::Identifier::get_lun(entity));
            aggr_entity.put(literals::ConnectedEntity::ENTITY, entity.get_entity());
            aggr_entity.dump();
            aggr_endpoint.append(literals::ConnectedEntity::ENTITY, aggr_entity.get_uuid());
        }
    }
    else {
        THROW(agent_framework::exceptions::InvalidValue, "storage-agent",
              "Only connected entity role Target or Initiator is supported.");
    }
    aggr_endpoint.dump();
}


void add_endpoint(const AddEndpoint::Request& request, AddEndpoint::Response& response) {
    log_debug("storage-agent", "Adding endpoint.");

    get_manager<Fabric>().get_entry(request.get_fabric());
    Endpoint endpoint{request.get_fabric()};

    std::string iqn{};
    enums::EntityRole role{enums::EntityRole::Target};

    endpoint.set_protocol(enums::StorageProtocol::iSCSI);

    extract_role(request, role);
    extract_iqn(request, role, iqn);
    endpoint.set_identifiers(request.get_identifiers());

    check_credentials(request);
    if (request.get_username().has_value()) {
        endpoint.set_username(request.get_username());
        endpoint.set_password(request.get_password());
    }
    endpoint.set_oem(request.get_oem());

    auto connected_entities = request.get_connected_entities();
    process_connected_entities(role, connected_entities);
    endpoint.set_connected_entities(connected_entities);
    endpoint.set_status({enums::State::Enabled, enums::Health::OK});

    if (enums::EntityRole::Target == role) {
        process_ip_transport_details(request, endpoint);
    }

    // Endpoints are stored in database so stabilization is not needed.
    store_persistent_attributes(endpoint.get_uuid(), iqn, role,
                                request.get_username().has_value() ? request.get_username() : EMPTY_VALUE,
                                endpoint.get_connected_entities());

    attribute::Identifier uuid{};
    uuid.set_durable_name(endpoint.get_uuid());
    uuid.set_durable_name_format(enums::IdentifierType::UUID);
    endpoint.add_identifier(std::move(uuid));

    response.set_endpoint(endpoint.get_uuid());
    get_manager<Endpoint>().add_entry(endpoint);
    log_debug("storage-agent", "Added endpoint with UUID '" + endpoint.get_uuid() + "'");
}
}

REGISTER_COMMAND(AddEndpoint, ::add_endpoint);
