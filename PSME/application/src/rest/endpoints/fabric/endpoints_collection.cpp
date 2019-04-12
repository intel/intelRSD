/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
json::Json make_prototype() {
    json::Json r(json::Json::value_t::object);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#EndpointCollection.EndpointCollection";
    r[Common::ODATA_ID] = json::Json::value_t::null;
    r[Common::ODATA_TYPE] = "#EndpointCollection.EndpointCollection";
    r[Common::NAME] = "Endpoint Collection";
    r[Common::DESCRIPTION] = "Endpoint Collection";
    r[Collection::ODATA_COUNT] = json::Json::value_t::null;
    r[Collection::MEMBERS] = json::Json::value_t::array;

    return r;
}


attribute::Array<attribute::Identifier> get_identifiers(const json::Json& json_identifiers) {
    attribute::Array<attribute::Identifier> result{};
    if (!json_identifiers.is_null()) {
        for (const auto& json_identifier : json_identifiers) {
            attribute::Identifier identifier{};
            identifier.set_durable_name(json_identifier[Common::DURABLE_NAME]);
            identifier.set_durable_name_format(json_identifier[Common::DURABLE_NAME_FORMAT]);
            result.add_entry(std::move(identifier));
        }
    }
    return result;
}


OptionalField<std::int64_t> get_lun(const json::Json& json_connected_entity) {

    const auto& json_lun = json_connected_entity.value(Common::OEM, json::Json::object())
        .value(Common::RACKSCALE, json::Json::object())
        .value(Common::LUN, json::Json{});

    return OptionalField<std::int64_t>(json_lun);
}


void get_target_connected_entity(attribute::ConnectedEntity& connected_entity,
                                 const json::Json& json_connected_entity) {
    connected_entity.set_entity_role(enums::EntityRole::Target);
    if (!json_connected_entity.value(constants::Endpoint::ENTITY_LINK, json::Json()).is_null()) {
        const auto& connected_entity_url = json_connected_entity.value(constants::Endpoint::ENTITY_LINK,
                                                                       json::Json::object()).value(Common::ODATA_ID,
                                                                                                   std::string{});

        OptionalField<Uuid> connected_entity_uuid;
        auto params_volume = server::Multiplexer::get_instance()->try_get_params(connected_entity_url,
                                                                                 constants::Routes::VOLUME_PATH);
        auto params_drive = server::Multiplexer::get_instance()->try_get_params(connected_entity_url,
                                                                                constants::Routes::DRIVE_PATH);
        auto params_processor = server::Multiplexer::get_instance()->try_get_params(connected_entity_url,
                                                                                    constants::Routes::PROCESSOR_PATH);

        if (params_volume.size() > 0) {
            connected_entity_uuid =
                model::try_find<agent_framework::model::StorageService, agent_framework::model::Volume>(
                    params_volume).get_uuid();
        }
        else if (params_drive.size() > 0) {
            connected_entity_uuid =
                model::try_find<agent_framework::model::Chassis, agent_framework::model::Drive>(
                    params_drive).get_uuid();
        }
        else if (params_processor.size() > 0) {
            connected_entity_uuid =
                model::try_find<agent_framework::model::System, agent_framework::model::Processor>(
                    params_processor).get_uuid();
        }

        if (connected_entity_uuid.has_value()) {
            connected_entity.set_entity(connected_entity_uuid.value());
        }
        else {
            log_error("rest", "Cannot find entity: " << connected_entity_url);
            throw agent_framework::exceptions::InvalidValue("Cannot find entity: " + connected_entity_url);
        }

    }
    connected_entity.set_identifiers(
        get_identifiers(json_connected_entity.value(Common::IDENTIFIERS, json::Json::array())));

    connected_entity.set_lun(get_lun(json_connected_entity));
}


void get_initiator_connected_entity(attribute::ConnectedEntity& connected_entity,
                                    const json::Json& json_connected_entity) {

    connected_entity.set_entity_role(enums::EntityRole::Initiator);

    if (!json_connected_entity.value(constants::Endpoint::ENTITY_LINK, json::Json()).is_null()) {
        const auto& system_url = json_connected_entity.value(constants::Endpoint::ENTITY_LINK,
                                                             json::Json::object()).value(Common::ODATA_ID,
                                                                                         std::string{});
        auto params = server::Multiplexer::get_instance()->
            get_params(system_url, constants::Routes::SYSTEM_PATH);
        auto system_uuid =
            model::try_find<agent_framework::model::System>(params).get_uuid();

        if (system_uuid.has_value()) {
            connected_entity.set_entity(system_uuid.value());
        }
        else {
            log_error("rest", "Cannot find system: " << system_url);
            throw agent_framework::exceptions::InvalidValue("Cannot find system: " + system_url);
        }
    }

    const auto& json_identifiers = json_connected_entity.count(Common::IDENTIFIERS) ?
                                   json_connected_entity[Common::IDENTIFIERS] :
                                   json::Json::array();
    connected_entity.set_identifiers(get_identifiers(json_identifiers));

    connected_entity.set_lun(get_lun(json_connected_entity));

}


attribute::Array<attribute::ConnectedEntity> get_connected_entities(const json::Json& json_connected_entities) {
    attribute::Array<attribute::ConnectedEntity> result{};
    for (const auto& json_connected_entity : json_connected_entities) {
        attribute::ConnectedEntity connected_entity{};
        auto role = enums::EntityRole::from_string(
            json_connected_entity[constants::Endpoint::ENTITY_ROLE].get<std::string>());

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


attribute::Ipv4Address get_ipv4_address(const json::Json& json_ipv4_address) {
    attribute::Ipv4Address ipv4_address{};
    ipv4_address.set_address(json_ipv4_address.value(IpAddress::ADDRESS, OptionalField<std::string>()));
    ipv4_address.set_address_origin(
        json_ipv4_address.value(IpAddress::ADDRESS_ORIGIN, OptionalField<enums::Ipv4AddressOrigin>()));
    ipv4_address.set_gateway(json_ipv4_address.value(IpAddress::GATEWAY, OptionalField<std::string>()));
    ipv4_address.set_subnet_mask(json_ipv4_address.value(IpAddress::SUBNET_MASK, OptionalField<std::string>()));
    return ipv4_address;
}


attribute::Ipv6Address get_ipv6_address(const json::Json& json_ipv6_address) {
    attribute::Ipv6Address ipv6_address{};
    ipv6_address.set_address(json_ipv6_address.value(IpAddress::ADDRESS, OptionalField<std::string>()));
    ipv6_address.set_address_origin(
        json_ipv6_address.value(IpAddress::ADDRESS_ORIGIN, OptionalField<enums::Ipv6AddressOrigin>()));
    ipv6_address.set_prefix_length(json_ipv6_address.value(IpAddress::PREFIX_LENGTH, OptionalField<uint32_t>()));
    ipv6_address.set_address_state(
        json_ipv6_address.value(IpAddress::ADDRESS_STATE, OptionalField<enums::Ipv6AddressState>()));
    return ipv6_address;
}


Uuid get_interface(const json::Json& json_interface) {
    const auto& interface_url = json_interface.value(Common::ODATA_ID, std::string{});

    auto params = server::Multiplexer::get_instance()->
        try_get_params(interface_url, constants::Routes::MANAGER_NETWORK_INTERFACE_PATH);

    // Just check if manager exists
    auto net_interface = model::try_find<agent_framework::model::Manager, agent_framework::model::NetworkInterface>(
        params).get();

    if (net_interface.has_value()) {
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
    else {
        params = server::Multiplexer::get_instance()->
            get_params(interface_url, constants::Routes::SYSTEM_ETHERNET_INTERFACE_PATH);
        auto interface_uuid =
            model::try_find<agent_framework::model::System, agent_framework::model::NetworkInterface>(
                params).get_uuid();

        if (!interface_uuid.has_value()) {
            throw agent_framework::exceptions::InvalidValue(
                "Could not find System Ethernet Interface " + interface_url + ".");
        }
        return interface_uuid.value();
    }
}


attribute::Array<attribute::IpTransportDetail> get_transports(const json::Json& json_transports,
                                                              const json::Json& json_interfaces) {
    attribute::Array<attribute::IpTransportDetail> result{};

    std::size_t transport_size{0};
    if (!json_transports.is_null()) {
        transport_size = json_transports.size();
    }
    std::size_t interfaces_size{0};
    if (!json_interfaces.is_null()) {
        interfaces_size = json_interfaces.size();
    }

    std::size_t array_size = std::max(transport_size, interfaces_size);
    for (std::size_t i = 0; i < array_size; ++i) {
        attribute::IpTransportDetail transport{};

        if (!json_transports.is_null()) {
            if (i < json_transports.size() && !json_transports.at(i).is_null()) {
                const auto& json_transport = json_transports.at(i);
                transport.set_transport_protocol(
                    json_transport.value(constants::Endpoint::TRANSPORT_PROTOCOL,
                                         OptionalField<agent_framework::model::enums::TransportProtocol>()));
                transport.set_port(json_transport.value(constants::Endpoint::PORT, OptionalField<std::uint32_t>()));
                transport.set_ipv4_address(
                    get_ipv4_address(json_transport.value(constants::Endpoint::IPV4_ADDRESS, json::Json::object())));
                transport.set_ipv6_address(
                    get_ipv6_address(json_transport.value(constants::Endpoint::IPV6_ADDRESS, json::Json::object())));
            }
        }

        if (!json_interfaces.is_null()) {
            if (i < json_interfaces.size() && !json_interfaces.at(i).is_null()) {
                transport.set_interface(get_interface(json_interfaces.at(i)));
            }
        }
        result.add_entry(std::move(transport));
    }
    return result;
}


agent_framework::model::attribute::Array<Uuid> get_ports_uuids(const json::Json& json_ports) {
    attribute::Array<Uuid> ports_uuids{};

    for (const json::Json& json_port: json_ports) {
        if (json_port.count(constants::Common::ODATA_ID)) {
            auto port_path = json_port[constants::Common::ODATA_ID].get<std::string>();
            auto port_params = server::Multiplexer::get_instance()->get_params(port_path, constants::Routes::PORT_PATH);
            auto port_uuid = psme::rest::model::try_find<agent_framework::model::Fabric,
                agent_framework::model::Switch,
                agent_framework::model::Port>(port_params).get_uuid();
            if (port_uuid.has_value()) {
                ports_uuids.add_entry(port_uuid.value());
            }
            else {
                THROW(agent_framework::exceptions::InvalidValue, "rest", "Could not find port: " + port_path);
            }
        }
    }

    return ports_uuids;
}

}


EndpointsCollection::EndpointsCollection(const std::string& path) : EndpointBase(path) {}


EndpointsCollection::~EndpointsCollection() {}


void EndpointsCollection::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    auto fabric_uuid = psme::rest::model::find<agent_framework::model::Fabric>(req.params).get_uuid();

    auto endpoint_ids = agent_framework::module::get_manager<agent_framework::model::Endpoint>().get_ids(fabric_uuid);

    json[Collection::ODATA_COUNT] = std::uint32_t(endpoint_ids.size());

    for (const auto& id : endpoint_ids) {
        json::Json link = json::Json();
        link[Common::ODATA_ID] = PathBuilder(req).append(id).build();
        json[Collection::MEMBERS].push_back(std::move(link));
    }
    set_response(res, json);
}


void EndpointsCollection::post(const server::Request& request, server::Response& response) {
    static const constexpr char TRANSACTION_NAME[] = "PostEndpointsCollection";

    auto json = JsonValidator::validate_request_body<schema::EndpointsCollectionPostSchema>(request);
    auto fabric = psme::rest::model::find<agent_framework::model::Fabric>(request.params).get();

    // AddEndpoint command does not accept protocol parameter so the validation must be done here
    auto endpoint_protocol = json.value(constants::Endpoint::ENDPOINT_PROTOCOL, OptionalField<enums::TransportProtocol>());
    if (endpoint_protocol.has_value() && fabric.get_protocol() != endpoint_protocol.value()) {
        throw agent_framework::exceptions::InvalidValue("Parent Fabric does not support requested Endpoint protocol.");
    }

    auto transport_details = json.value(constants::Endpoint::IP_TRANSPORT_DETAILS, json::Json::array());
    auto interfaces = json.value(Common::LINKS, json::Json::object())
        .value(Common::OEM, json::Json::object())
        .value(Common::RACKSCALE, json::Json::object())
        .value(constants::Endpoint::INTERFACES, json::Json::array());
    auto identifiers = json.value(Common::IDENTIFIERS, json::Json::array());
    auto connected_entities = json.value(constants::Endpoint::CONNECTED_ENTITIES, json::Json::array());
    auto ports = json.value(Common::LINKS, json::Json::object()).value(constants::Endpoint::PORTS, json::Json::array());
    auto authentication = json.value(Common::OEM, json::Json::object())
        .value(Common::RACKSCALE, json::Json::object())
        .value(constants::Endpoint::AUTHENTICATION, json::Json::object());

    agent_framework::model::requests::AddEndpoint add_endpoint_request{
        fabric.get_uuid(),
        ::get_transports(transport_details, interfaces),
        ::get_identifiers(identifiers),
        ::get_connected_entities(connected_entities),
        authentication.value(constants::Endpoint::USERNAME, OptionalField<std::string>()),
        authentication.value(constants::Endpoint::PASSWORD, OptionalField<std::string>()),
        ::get_ports_uuids(ports),
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
                 add_endpoint_response.get_endpoint(), true);

        const auto& created_endpoint = agent_framework::module::get_manager<agent_framework::model::Endpoint>()
            .get_entry(add_endpoint_response.get_endpoint());

        utils::set_location_header(request, response, PathBuilder(request).append(created_endpoint.get_id()).build());
        response.set_status(server::status_2XX::CREATED);
    };

    gami_agent->execute_in_transaction(TRANSACTION_NAME, add_endpoint);
}
