/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/endpoints/fabric/endpoints_collection.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/endpoints_collection.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"


using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace psme::rest::constants;
using namespace psme::rest::validators;
using namespace agent_framework::model;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EndpointCollection.EndpointCollection";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#EndpointCollection.EndpointCollection";
    r[Common::NAME] = "Endpoint Collection";
    r[Common::DESCRIPTION] = "Endpoint Collection";
    r[Collection::ODATA_COUNT] = json::Value::Type::NIL;
    r[Collection::MEMBERS] = json::Value::Type::ARRAY;

    return r;
}

attribute::Array<attribute::Identifier> get_identifiers(const json::Value& json_identifiers) {
    attribute::Array<attribute::Identifier> result{};
    if (!json_identifiers.is_null()) {
        const auto& array_identifiers = json_identifiers.as_array();
        for (const auto& json_identifier : array_identifiers) {
            attribute::Identifier identifier{};
            identifier.set_durable_name(json_identifier[Common::DURABLE_NAME]);
            identifier.set_durable_name_format(json_identifier[Common::DURABLE_NAME_FORMAT]);
            result.add_entry(std::move(identifier));
        }
    }
    return result;
}

void get_target_connected_entity(attribute::ConnectedEntity& connected_entity,
                                 const json::Value& json_connected_entity) {
    connected_entity.set_entity_role(enums::EntityRole::Target);
    if (!json_connected_entity[constants::Endpoint::ENTITY_LINK].is_null()) {
        const auto& volume_url = json_connected_entity[constants::Endpoint::ENTITY_LINK][Common::ODATA_ID].as_string();
        try {
            auto params = server::Multiplexer::get_instance()->
                get_params(volume_url, constants::Routes::VOLUME_PATH);

            Uuid volume_uuid =
                model::Find<agent_framework::model::Volume>(params[PathParam::VOLUME_ID])
                    .via<agent_framework::model::StorageService>(params[PathParam::SERVICE_ID])
                    .get_uuid();

            connected_entity.set_entity(volume_uuid);
        }
        catch (const agent_framework::exceptions::NotFound& ex) {
            log_error("rest", "Cannot find volume: " << volume_url);
            throw agent_framework::exceptions::InvalidValue(ex.get_message());
        }
    }
    connected_entity.set_identifiers(get_identifiers(json_connected_entity[Common::IDENTIFIERS]));
}


void get_initiator_connected_entity(attribute::ConnectedEntity& connected_entity,
                                      const json::Value& json_connected_entity) {
    connected_entity.set_entity_role(enums::EntityRole::Initiator);

    if (!json_connected_entity[constants::Endpoint::ENTITY_LINK].is_null()) {
        const auto& system_url = json_connected_entity[constants::Endpoint::ENTITY_LINK][Common::ODATA_ID].as_string();
        try {
            auto params = server::Multiplexer::get_instance()->
                get_params(system_url, constants::Routes::SYSTEM_PATH);

            Uuid system_uuid =
                model::Find<agent_framework::model::System>(params[PathParam::SYSTEM_ID])
                    .get_uuid();

            connected_entity.set_entity(system_uuid);
        }
        catch (const agent_framework::exceptions::NotFound& ex) {
            log_error("rest", "Cannot find system: " << system_url);
            throw agent_framework::exceptions::InvalidValue(ex.get_message());
        }
    }
    connected_entity.set_identifiers(get_identifiers(json_connected_entity[Common::IDENTIFIERS]));
}


attribute::Array<attribute::ConnectedEntity> get_connected_entities(const json::Value& json_connected_entities) {
    attribute::Array<attribute::ConnectedEntity> result{};
    const auto& array_connected_entities = json_connected_entities.as_array();
    for (const auto& json_connected_entity : array_connected_entities) {
        attribute::ConnectedEntity connected_entity{};
        auto role = enums::EntityRole::from_string(json_connected_entity[constants::Endpoint::ENTITY_ROLE].as_string());

        if (role == enums::EntityRole::Target) {
            get_target_connected_entity(connected_entity, json_connected_entity);
        }
        else if (role == enums::EntityRole::Initiator) {
            get_initiator_connected_entity(connected_entity, json_connected_entity);
        }
        else {
            throw agent_framework::exceptions::UnsupportedValue("Unsupported ConnectedEntity Role.");
        }
        result.add_entry(std::move(connected_entity));
    }
    return result;
}

attribute::Ipv4Address get_ipv4_address(const json::Value& json_ipv4_address) {
    attribute::Ipv4Address ipv4_address{};
    ipv4_address.set_address(json_ipv4_address[IpAddress::ADDRESS]);
    ipv4_address.set_address_origin(json_ipv4_address[IpAddress::ADDRESS_ORIGIN]);
    ipv4_address.set_gateway(json_ipv4_address[IpAddress::GATEWAY]);
    ipv4_address.set_subnet_mask(json_ipv4_address[IpAddress::SUBNET_MASK]);
    return ipv4_address;
}

attribute::Ipv6Address get_ipv6_address(const json::Value& json_ipv6_address) {
    attribute::Ipv6Address ipv6_address{};
    ipv6_address.set_address(json_ipv6_address[IpAddress::ADDRESS]);
    ipv6_address.set_address_origin(json_ipv6_address[IpAddress::ADDRESS_ORIGIN]);
    ipv6_address.set_prefix_length(json_ipv6_address[IpAddress::PREFIX_LENGTH]);
    ipv6_address.set_address_state(json_ipv6_address[IpAddress::ADDRESS_STATE]);
    return ipv6_address;
}

Uuid get_interface(const json::Value& json_interface) {
    const auto& interface_url = json_interface[Common::ODATA_ID].as_string();
    try {
        auto params = server::Multiplexer::get_instance()->
            get_params(interface_url, constants::Routes::MANAGER_NETWORK_INTERFACE_PATH);

        // Just check if manager exists
        model::Find<agent_framework::model::NetworkInterface>(params[PathParam::NIC_ID])
            .via<agent_framework::model::Manager>(params[PathParam::MANAGER_ID])
            .get();

        throw psme::rest::error::ErrorFactory::create_invalid_payload_error(
            "Only System Ethernet Interface link is supported!",
            {
                PathBuilder(Common::LINKS)
                    .append(Common::OEM)
                    .append(Common::RACKSCALE)
                    .append(constants::Endpoint::INTERFACES)
                    .build()
            },
            "Please provide correct link to System Ethernet Interface."
        );
    }
    catch (const agent_framework::exceptions::NotFound&) {
        // Expected behaviour
    }

    Uuid interface_uuid{};
    try {
        auto params = server::Multiplexer::get_instance()->
            get_params(interface_url, constants::Routes::SYSTEM_ETHERNET_INTERFACE_PATH);

        interface_uuid =
            model::Find<agent_framework::model::NetworkInterface>(params[PathParam::NIC_ID])
                .via<agent_framework::model::System>(params[PathParam::SYSTEM_ID])
                .get_uuid();
    }
    catch (const agent_framework::exceptions::NotFound&) {
        throw agent_framework::exceptions::InvalidValue("Could not find System Ethernet Interface " + interface_url + ".");
    }
    return interface_uuid;
}

attribute::Array<attribute::IpTransportDetail> get_transports(const json::Value& json_transports,
                                                              const json::Value& json_interfaces) {
    attribute::Array<attribute::IpTransportDetail> result{};

    std::size_t transport_size{0};
    if (!json_transports.is_null()) {
        transport_size = json_transports.as_array().size();
    }
    std::size_t interfaces_size{0};
    if (!json_interfaces.is_null()) {
        interfaces_size = json_interfaces.as_array().size();
    }

    std::size_t array_size = std::max(transport_size, interfaces_size);
    for (std::size_t i = 0; i < array_size; ++i) {
        attribute::IpTransportDetail transport{};

        if (!json_transports.is_null()) {
            const auto& json_transport_array = json_transports.as_array();
            if (i < json_transport_array.size() && !json_transport_array.at(i).is_null()) {
                const auto& json_transport = json_transport_array.at(i);
                transport.set_transport_protocol(json_transport[constants::Endpoint::TRANSPORT_PROTOCOL]);
                transport.set_port(json_transport[constants::Endpoint::PORT]);
                transport.set_ipv4_address(get_ipv4_address(json_transport[constants::Endpoint::IPV4_ADDRESS]));
                transport.set_ipv6_address(get_ipv6_address(json_transport[constants::Endpoint::IPV6_ADDRESS]));
            }
        }

        if (!json_interfaces.is_null()) {
            const auto& json_interfaces_array = json_interfaces.as_array();
            if (i < json_interfaces_array.size() && !json_interfaces_array.at(i).is_null()) {
                transport.set_interface(get_interface(json_interfaces_array.at(i)));
            }
        }
        result.add_entry(std::move(transport));
    }
    return result;
}

}


EndpointsCollection::EndpointsCollection(const std::string& path) : EndpointBase(path) {}


EndpointsCollection::~EndpointsCollection() {}


void EndpointsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto fabric_uuid = psme::rest::model::Find<agent_framework::model::Fabric>(
        req.params[PathParam::FABRIC_ID]).get_uuid();

    auto endpoint_ids = agent_framework::module::get_manager<agent_framework::model::Endpoint>().get_ids(fabric_uuid);

    json[Collection::ODATA_COUNT] = std::uint32_t(endpoint_ids.size());

    for (const auto& id : endpoint_ids) {
        json::Value link{};
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }
    set_response(res, json);
}


void EndpointsCollection::post(const server::Request& request, server::Response& response) {
    auto json = JsonValidator::validate_request_body<schema::EndpointsCollectionPostSchema>(request);
    auto fabric = psme::rest::model::Find<agent_framework::model::Fabric>(request.params[PathParam::FABRIC_ID]).get();

    auto transport_details = json[constants::Endpoint::IP_TRANSPORT_DETAILS];
    auto interfaces = json[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Endpoint::INTERFACES];
    auto identifiers = json[Common::IDENTIFIERS];
    auto connected_entities = json[constants::Endpoint::CONNECTED_ENTITIES];
    auto authentication = json[Common::OEM][Common::RACKSCALE][constants::Endpoint::AUTHENTICATION];

    agent_framework::model::requests::AddEndpoint add_endpoint_request{
        fabric.get_uuid(),
        ::get_transports(transport_details, interfaces),
        ::get_identifiers(identifiers),
        ::get_connected_entities(connected_entities),
        authentication[constants::Endpoint::USERNAME],
        authentication[constants::Endpoint::PASSWORD],
        attribute::Oem{}
    };

    auto agent_id = fabric.get_agent_id();
    auto gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);
    auto add_endpoint = [&, gami_agent] {
        auto add_endpoint_response = gami_agent->
            execute<agent_framework::model::responses::AddEndpoint>(add_endpoint_request);

        model::handler::HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::Endpoint)->
            load(gami_agent,
                 fabric.get_uuid(), agent_framework::model::enums::Component::Fabric,
                 add_endpoint_response.get_endpoint(), false);

        const auto& created_endpoint = agent_framework::module::get_manager<agent_framework::model::Endpoint>().get_entry(
            add_endpoint_response.get_endpoint());

        utils::set_location_header(request, response, PathBuilder(request).append(created_endpoint.get_id()).build());
        response.set_status(server::status_2XX::CREATED);
    };

    gami_agent->execute_in_transaction(add_endpoint);
}
