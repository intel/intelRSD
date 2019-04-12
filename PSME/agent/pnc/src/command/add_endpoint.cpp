/*!
 * @brief AddEndpoint command implementation
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file command/add_endpoint.cpp
 */


#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "agent-framework/discovery/builders/identifier_builder.hpp"
#include "discovery/builders/endpoint_builder.hpp"
#include "discovery/discovery_actions.hpp"
#include "tools/toolset.hpp"
#include "tools/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "tree_stability/pnc_stabilizer.hpp"



using namespace agent::pnc;
using namespace agent::pnc::discovery::builders;
using namespace agent::pnc::discovery;
using namespace agent::pnc::tools;
using namespace agent_framework::command;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::enums;
using namespace agent_framework::module;

namespace {
void throw_if_fabric_not_in_model(const Uuid& fabric_uuid) {
    if (!get_manager<Fabric>().entry_exists(fabric_uuid)) {
        THROW(InvalidValue, "pnc-gami",
              "Invalid value in Fabric property, invalid Fabric UUID.");
    }
}


void throw_if_connected_entities_have_wrong_size(const Array<ConnectedEntity>& connected_entities) {
    if (connected_entities.size() != 1) {
        THROW(InvalidValue, "pnc-gami",
              "ConnectedEntities array should have exactly 1 entry. Found: "
              + std::to_string(static_cast<unsigned>(connected_entities.size())));
    }
}


void validate_request_for_initiator(const AddEndpoint::Request& request) {
    const auto& ports_uuids = request.get_ports();
    if (ports_uuids.size() != 1) {
        THROW(InvalidValue, "pnc-gami",
              "Ports array should have exactly 1 member.");
    }

    if (request.get_connected_entities()[0].get_entity().has_value()) {
        THROW(InvalidValue, "pnc-gami",
              "Property Entity should be empty for Initiator Endpoint.");
    }

    for (const auto& port_uuid: ports_uuids) {
        Port port = get_manager<Port>().get_entry(port_uuid);   // Throws if invalid port
        if (port.get_port_type() != PciePortType::UpstreamPort &&
            port.get_port_type() != PciePortType::BidirectionalPort) {
            THROW(InvalidValue, "pnc-gami",
                  "Invalid value in Ports property, when \"EntityRole\" is set to \"Initiator\","
                      "\"Ports\" should contain link to Upstream Port.");
        }

        const auto& associated_endpoints = get_m2m_manager<Endpoint, Port>().get_parents(port_uuid);
        if (associated_endpoints.size() > 0) {
            THROW(InvalidValue, "pnc-gami",
                  "Invalid value in Ports property, specified Port already has an associated Endpoint.");
        }
    }
}


void validate_request_for_target(const AddEndpoint::Request& request) {
    const auto& entity = request.get_connected_entities()[0].get_entity();
    if (!entity.has_value()) {
        THROW(InvalidValue, "pnc-gami",
              "Field Entity is required for Target Endpoints.");
    }

    if (!Toolset::get().model_tool->exists_in_components(entity.value())) {
        log_debug("pnc-gami",
                  "Invalid value in Entity property: Device with UUID " + entity.value() + " not found.");
        THROW(InvalidValue, "pnc-gami",
              "Invalid value in Entity property: Device not found.");
    }

    if (request.get_ports().size() > 0) {
        THROW(InvalidValue, "pnc-gami",
              "Array of Ports should be empty for Target Endpoints.");
    }

    // Theoretically this case should never happen
    auto ports_uuids = Toolset::get().model_tool->get_ports_by_device_uuid(entity.value());
    if (ports_uuids.size() == 0) {
        log_debug("pnc-gami", "Device with UUID " + entity.value() + " doesn't have associated Port.");
        THROW(InvalidValue, "pnc-gami", "Device doesn't have associated Port.");
    }

    const auto& associated_endpoints = get_m2m_manager<Endpoint, Port>().get_parents(ports_uuids[0]);
    if (associated_endpoints.size() > 0) {
        THROW(InvalidValue, "pnc-gami",
              "Invalid value in Entity property, Port of specified entity already has an associated Endpoint.");
    }
}


/*!
 * @brief Validates request
 * @param request Request to validate
 * @throw Throws an exception with explanation if request fails.
 */
void validate_request(const AddEndpoint::Request& request) {
    const auto& connected_entities = request.get_connected_entities();

    ::throw_if_fabric_not_in_model(request.get_fabric());
    ::throw_if_connected_entities_have_wrong_size(connected_entities);
    if (connected_entities[0].get_entity_role() == EntityRole::Initiator) {
        ::validate_request_for_initiator(request);
    }
    else if (connected_entities[0].get_entity_role() == EntityRole::Target) {
        ::validate_request_for_target(request);
    }
    else {
        THROW(InvalidValue, "pnc-gami", "\"EntityRole\" should be \"Target\" or \"Initiator\".");
    }
}

void add_initiator_endpoint(const AddEndpoint::Request& request, AddEndpoint::Response& response) {
    log_debug("pnc-gami", "Adding endpoint");
    get_manager<Fabric>().get_entry(request.get_fabric());

    EndpointBuilder endpoint_builder;
    endpoint_builder.init(request.get_fabric());

    Endpoint endpoint = endpoint_builder.add_host_entity().build();

    Port port = get_manager<Port>().get_entry(request.get_ports()[0]); // Validated to have exactly 1 upstream port

    PncStabilizer stabilizer{};
    Uuid endpoint_uuid = stabilizer.stabilize(endpoint, std::vector<agent_framework::model::Port>{port});

    agent_framework::discovery::IdentifierBuilder::set_uuid(endpoint, endpoint_uuid);

    endpoint_uuid = log_and_add<Endpoint>(endpoint);
    get_m2m_manager<Endpoint, Port>().add_entry(endpoint_uuid, port.get_uuid());

    agent_framework::database::EndpointEntity db{endpoint_uuid};
    db.put(db_keys::ENDPOINT_ROLE, literals::Endpoint::INITIATOR);
    db.put(db_keys::PORT, port.get_uuid());

    agent_framework::database::FabricEntity(request.get_fabric()).append(db_keys::ENDPOINTS, endpoint_uuid);

    response.set_endpoint(endpoint.get_uuid());
    log_info("pnc-gami", "Added endpoint with UUID '" + endpoint.get_uuid() + "'");
}


void add_target_endpoint(const AddEndpoint::Request& request, AddEndpoint::Response& response) {

    log_debug("pnc-gami", "Adding endpoint");

    get_manager<Fabric>().get_entry(request.get_fabric());

    Uuid connected_device_uuid = request.get_connected_entities()[0].get_entity().value(); // Validated to exist

    auto ports_uuids = Toolset::get().model_tool->get_ports_by_device_uuid(connected_device_uuid);

    if (ports_uuids.size() > 1) { // Should never happen
        log_warning("pnc-gami", "Device has more than 1 associated PCIe port. Using port with UUID " + ports_uuids[0]);
    }

    auto port = agent_framework::module::get_manager<agent_framework::model::Port>().get_entry(ports_uuids[0]);

    EndpointBuilder endpoint_builder;
    endpoint_builder.init(request.get_fabric());
    Endpoint endpoint = endpoint_builder.add_device_entity(connected_device_uuid).build();

    PncStabilizer stabilizer{};
    Uuid endpoint_uuid = stabilizer.stabilize(endpoint, std::vector<agent_framework::model::Port>{port});

    agent_framework::discovery::IdentifierBuilder::set_uuid(endpoint, endpoint_uuid);

    endpoint_uuid = log_and_add<Endpoint>(endpoint);
    get_m2m_manager<Endpoint, Port>().add_entry(endpoint_uuid, ports_uuids[0]);

    agent_framework::database::EndpointEntity db{endpoint_uuid};
    db.put(db_keys::DEVICE, connected_device_uuid);
    db.put(db_keys::DEVICE_PORT, port.get_uuid());
    db.put(db_keys::ENDPOINT_ROLE, literals::Endpoint::TARGET);

    agent_framework::database::FabricEntity(request.get_fabric()).append(db_keys::ENDPOINTS, endpoint_uuid);

    response.set_endpoint(endpoint.get_uuid());
    log_info("pnc-gami", "Added endpoint with UUID '" + endpoint.get_uuid() + "'");
}


void add_endpoint(const AddEndpoint::Request& request, AddEndpoint::Response& response) {
    ::validate_request(request);

    auto entity_role = request.get_connected_entities()[0].get_entity_role(); // Validated to have 1 connected entity
    if (entity_role == EntityRole::Initiator) {
        ::add_initiator_endpoint(request, response);
    }
    else if (entity_role == EntityRole::Target) {
        ::add_target_endpoint(request, response);
    }
}
}

REGISTER_COMMAND(AddEndpoint, ::add_endpoint);
