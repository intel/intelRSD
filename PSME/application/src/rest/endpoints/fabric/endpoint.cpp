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

#include "psme/rest/validators/json_validator.hpp"
#include "agent-framework/module/responses/storage.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/common.hpp"
#include "psme/rest/endpoints/fabric/endpoint.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/schemas/endpoint.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);
    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Endpoint.Endpoint";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Endpoint.v1_1_0.Endpoint";
    r[Common::NAME] = "Fabric Endpoint";

    r[Common::DESCRIPTION] = "Fabric Endpoint";
    r[Common::ID] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[constants::Endpoint::ENDPOINT_PROTOCOL] = json::Value::Type::NIL;
    r[constants::Endpoint::PCI_ID] = json::Value::Type::NIL;

    r[constants::Endpoint::CONNECTED_ENTITIES] = json::Value::Type::ARRAY;
    r[constants::Common::REDUNDANCY] = json::Value::Type::ARRAY;
    r[constants::Endpoint::HOST_RESERVATION_MEMORY_BYTES] = json::Value::Type::NIL;
    r[constants::Endpoint::IP_TRANSPORT_DETAILS] = json::Value::Type::ARRAY;
    r[Common::IDENTIFIERS] = json::Value::Type::ARRAY;
    r[Common::ACTIONS][Common::OEM] = json::Value::Type::OBJECT;

    r[Common::LINKS][Common::ODATA_TYPE] = "#Endpoint.v1_1_0.Links";
    r[Common::LINKS][constants::Switch::PORTS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.EndpointLinks";
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Fabric::ZONES] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Endpoint::INTERFACES] = json::Value::Type::ARRAY;

    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.Endpoint";
    r[Common::OEM][Common::RACKSCALE][constants::Endpoint::AUTHENTICATION][constants::Endpoint::USERNAME] =
        json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE][constants::Endpoint::AUTHENTICATION][constants::Endpoint::PASSWORD] =
        json::Value::Type::NIL;

    return r;
}

json::Value make_transport_detail_prototype() {
    json::Value json{};
    json[constants::Endpoint::TRANSPORT_PROTOCOL] = json::Value::Type::NIL;
    json[constants::Endpoint::IPV4_ADDRESS] = json::Value::Type::NIL;
    json[constants::Endpoint::IPV6_ADDRESS] = json::Value::Type::NIL;
    json[constants::Endpoint::PORT] = json::Value::Type::NIL;
    return json;
}


json::Value make_entity_prototype() {
    json::Value json{};
    json[constants::Endpoint::ENTITY_ROLE] = json::Value::Type::NIL;
    json[constants::Endpoint::PCI_FUNCTION_NUMBER] = json::Value::Type::NIL;
    json[constants::Endpoint::PCI_CLASS_CODE] = json::Value::Type::NIL;
    json[constants::Endpoint::ENTITY_PCI_ID] = json::Value::Type::NIL;
    json[constants::Endpoint::ENTITY_LINK] = json::Value::Type::NIL;
    json[constants::Common::IDENTIFIERS] = json::Value::Type::ARRAY;
    json[constants::Common::OEM] = json::Value::Type::OBJECT;
    return json;
}


json::Value entity_to_json(const agent_framework::model::attribute::ConnectedEntity& entity,
                           const agent_framework::model::Endpoint& endpoint) {
    auto json = make_entity_prototype();
    json[constants::Endpoint::ENTITY_ROLE] = entity.get_entity_role();

    for (const auto& identifier : entity.get_identifiers()) {
        json::Value id{};
        id[Common::DURABLE_NAME] = identifier.get_durable_name();
        id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        json[Common::IDENTIFIERS].push_back(std::move(id));
    }

    if (!entity.get_entity().has_value()) {
        log_warning("rest",
                    "No entity link specified for endpoint's entity" + endpoint.get_uuid()
                    + ". Current endpoint status is " + endpoint.get_status().get_state().to_string() + ".");
        return json;
    }

    auto& volume_manager = agent_framework::module::get_manager<agent_framework::model::Volume>();
    auto& drive_manager = agent_framework::module::get_manager<agent_framework::model::Drive>();
    auto& system_manager = agent_framework::module::get_manager<agent_framework::model::System>();

    // handle volume targets
    if (entity.get_entity_role() == agent_framework::model::enums::EntityRole::Target
        && volume_manager.entry_exists(entity.get_entity())) {

        json[constants::Endpoint::ENTITY_LINK][Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Volume,
                                               entity.get_entity())).build();
        return json;
    }

    // handle drive targets
    if (entity.get_entity_role() == agent_framework::model::enums::EntityRole::Target
        && drive_manager.entry_exists(entity.get_entity())) {

        // fill link
        const auto drive = drive_manager.get_entry(entity.get_entity());
        json[constants::Endpoint::ENTITY_LINK][Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Drive,
                                               drive.get_uuid())).build();

        // fill identifiers
        for (const auto& identifier : drive.get_identifiers()) {
            json::Value id{};
            id[Common::DURABLE_NAME] = identifier.get_durable_name();
            id[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
            json[Common::IDENTIFIERS].push_back(std::move(id));
        }

        const auto drive_function_uuids = agent_framework::module::PncComponents::get_instance()->
            get_drive_function_manager().get_children(drive.get_uuid());

        if (drive_function_uuids.size() == 0) {
            log_info("rest", "PNC Drive " + drive.get_uuid() + " has no PCIeFunctions!");
            return json;
        }

        try {
            const auto function = agent_framework::module::get_manager<agent_framework::model::PcieFunction>()
                .get_entry(drive_function_uuids.front());

            try {
                if (function.get_function_id().has_value()) {
                    json[constants::Endpoint::PCI_FUNCTION_NUMBER] = std::stoi(function.get_function_id());
                }
            }
            catch (const std::exception& ex) {
                log_warning("rest", "Invalid function id type:" << ex.what());
            }
            json[constants::Endpoint::PCI_CLASS_CODE] = function.get_pci_class_code();
            json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::DEVICE_ID] = function.get_pci_device_id();
            json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::VENDOR_ID] = function.get_pci_vendor_id();
            json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::SUBSYSTEM_ID] = function.get_pci_subsystem_id();
            json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::SUBSYSTEM_VENDOR_ID] = function.get_pci_subsystem_vendor_id();
        }
        catch (agent_framework::exceptions::InvalidUuid&) {
            log_error("rest", "Drive " + drive.get_uuid() + " has non-existing PCIeFunctions!");
        }

        return json;
    }

    if (entity.get_entity_role() == agent_framework::model::enums::EntityRole::Initiator
        && system_manager.entry_exists(entity.get_entity())) {

        json[constants::Endpoint::ENTITY_LINK][Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::System,
                                               entity.get_entity())).build();
        return json;
    }

    // handle links to unknown resources
    log_error("rest", "Entity on endpoint " << endpoint.get_uuid()
        << " links to an unknown resource: "
        << "EntityRole=" << entity.get_entity_role() << ", "
        << "EntityUuid=" << entity.get_entity());

    return json;
}


void fill_links(const agent_framework::model::Endpoint& endpoint, json::Value& json) {
    auto& m2m = agent_framework::module::get_m2m_manager<agent_framework::model::Zone, agent_framework::model::Endpoint>();
    // fill Zones links
    for (const auto& zone_uuid : m2m.get_parents(endpoint.get_uuid())) {
        json::Value zone_link{};
        zone_link[Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Zone,
                                               zone_uuid)).build();
        json[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Fabric::ZONES].push_back(zone_link);
    }

    for (const auto& transport : endpoint.get_ip_transport_details()) {
        if (transport.get_interface().has_value()) {
            try {
                auto link = json::Value{};
                link[Common::ODATA_ID] = endpoint::PathBuilder(
                    endpoint::utils::get_component_url(agent_framework::model::enums::Component::NetworkInterface,
                                                       transport.get_interface())).build();

                json[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Endpoint::INTERFACES].push_back(link);
            }
            catch (const agent_framework::exceptions::InvalidUuid&) {
                log_warning("rest", "Endpoint " << endpoint.get_uuid()
                                                << " points to a missing interface "
                                                << transport.get_interface());
            }
        }
    }
}

}


endpoint::Endpoint::Endpoint(const std::string& path) : EndpointBase(path) {}


endpoint::Endpoint::~Endpoint() {}


void endpoint::Endpoint::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    const auto endpoint = psme::rest::model::Find<agent_framework::model::Endpoint>(req.params[PathParam::ENDPOINT_ID])
        .via<agent_framework::model::Fabric>(req.params[PathParam::FABRIC_ID]).get();

    json[Common::ODATA_ID] = PathBuilder(req).build();
    json[Common::ID] = req.params[PathParam::ENDPOINT_ID];

    endpoint::status_to_json(endpoint, json);
    json[constants::Endpoint::ENDPOINT_PROTOCOL] = endpoint.get_protocol();

    for (const auto& identifier : endpoint.get_identifiers()) {
        json::Value id_link{};
        id_link[Common::DURABLE_NAME] = identifier.get_durable_name();
        id_link[Common::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format();
        json[Common::IDENTIFIERS].push_back(std::move(id_link));
    }

    for (const auto& entity : endpoint.get_connected_entities()) {
        json[constants::Endpoint::CONNECTED_ENTITIES].push_back(std::move(entity_to_json(entity, endpoint)));
    }

    for (const auto& port_uuid : agent_framework::module::PncComponents::get_instance()->get_endpoint_port_manager()
        .get_children(endpoint.get_uuid())) {

        json::Value port_link{};
        port_link[Common::ODATA_ID] =
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Port, port_uuid);
        json[Common::LINKS][Switch::PORTS].push_back(std::move(port_link));
    }

    for (const auto& transport : endpoint.get_ip_transport_details()) {
        auto t = ::make_transport_detail_prototype();
        t[constants::Endpoint::TRANSPORT_PROTOCOL] = transport.get_transport_protocol();

        json::Value ipv4{};
        ipv4[constants::IpAddress::ADDRESS] = transport.get_ipv4_address().get_address();
        ipv4[constants::IpAddress::ADDRESS_ORIGIN] = transport.get_ipv4_address().get_address_origin();
        ipv4[constants::IpAddress::GATEWAY] = transport.get_ipv4_address().get_gateway();
        ipv4[constants::IpAddress::SUBNET_MASK] = transport.get_ipv4_address().get_subnet_mask();
        t[constants::Endpoint::IPV4_ADDRESS] = std::move(ipv4);

        json::Value ipv6{};
        ipv6[constants::IpAddress::ADDRESS] = transport.get_ipv6_address().get_address();
        ipv6[constants::IpAddress::ADDRESS_ORIGIN] = transport.get_ipv6_address().get_address_origin();
        ipv6[constants::IpAddress::ADDRESS_STATE] = transport.get_ipv6_address().get_address_state();
        ipv6[constants::IpAddress::PREFIX_LENGTH] = transport.get_ipv6_address().get_prefix_length();
        t[constants::Endpoint::IPV6_ADDRESS] = std::move(ipv6);

        t[constants::Endpoint::PORT] = transport.get_port();
        json[constants::Endpoint::IP_TRANSPORT_DETAILS].push_back(std::move(t));

    }

    json[Common::OEM][Common::RACKSCALE][constants::Endpoint::AUTHENTICATION][constants::Endpoint::USERNAME] =
        endpoint.get_username();

    fill_links(endpoint, json);
    set_response(res, json);
}


void endpoint::Endpoint::patch(const server::Request& request, server::Response& response) {
    auto json = JsonValidator::validate_request_body<schema::EndpointPatchSchema>(request);

    const auto endpoint = psme::rest::model::Find<agent_framework::model::Endpoint>(
        request.params[PathParam::ENDPOINT_ID])
        .via<agent_framework::model::Fabric>(request.params[PathParam::FABRIC_ID]).get();

    const auto& agent_id = endpoint.get_agent_id();

    agent_framework::model::attribute::Attributes attributes{};

    const auto& auth = json[Common::OEM][Common::RACKSCALE][constants::Endpoint::AUTHENTICATION];

    if (auth.is_member(constants::Endpoint::USERNAME)) {
        if (auth[constants::Endpoint::USERNAME].is_null()) {
            attributes.set_value(agent_framework::model::literals::Endpoint::USERNAME, json::Json{});
        }
        else {
            attributes.set_value(agent_framework::model::literals::Endpoint::USERNAME, auth[constants::Endpoint::USERNAME].as_string());
        }

        if (auth[constants::Endpoint::PASSWORD].is_null()) {
            attributes.set_value(agent_framework::model::literals::Endpoint::PASSWORD, json::Json{});
        }
        else {
            attributes.set_value(agent_framework::model::literals::Endpoint::PASSWORD, auth[constants::Endpoint::PASSWORD].as_string());
        }
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes set_component_attributes_request{endpoint.get_uuid(),
                                                                                                  attributes};
        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(agent_id);

        auto set_target_attributes = [&, gami_agent] {
            gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(
                set_component_attributes_request);

            auto parent_uuid = endpoint.get_parent_uuid();
            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Endpoint)->
                load(gami_agent, parent_uuid,
                     agent_framework::model::enums::Component::Fabric, endpoint.get_uuid(), false);
        };
        gami_agent->execute_in_transaction(set_target_attributes);

    }
    get(request, response);
}


void endpoint::Endpoint::del(const server::Request& req, server::Response& res) {

    const auto endpoint = psme::rest::model::Find<agent_framework::model::Endpoint>(req.params[PathParam::ENDPOINT_ID])
        .via<agent_framework::model::Fabric>(req.params[PathParam::FABRIC_ID]).get();

    auto gami_req = agent_framework::model::requests::DeleteEndpoint(endpoint.get_uuid());

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(endpoint.get_agent_id());

    auto remove_endpoint = [&, gami_agent] {
        // try removing endpoint from agent's model
        gami_agent->execute<agent_framework::model::responses::DeleteEndpoint>(gami_req);

        // remove endpoint from REST model, DO NOT use endpoint reference after this line!
        psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
            agent_framework::model::enums::Component::Endpoint)->
            remove(endpoint.get_uuid());

        res.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(remove_endpoint);
}
