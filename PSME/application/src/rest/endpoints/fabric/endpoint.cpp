/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "psme/rest/endpoints/fabric/endpoint.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);
    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Endpoint.Endpoint";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Endpoint.v1_0_0.Endpoint";
    r[Common::NAME] = "Fabric Endpoint";

    r[Common::DESCRIPTION] = "Fabric Endpoint";
    r[Common::ID] = json::Value::Type::NIL;

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[constants::Endpoint::ENDPOINT_PROTOCOL] = json::Value::Type::NIL;
    r[constants::Endpoint::PCI_ID] = json::Value::Type::NIL;

    r[constants::PncDrive::IDENTIFIERS] = json::Value::Type::ARRAY;
    r[constants::Endpoint::CONNECTED_ENTITIES] = json::Value::Type::ARRAY;
    r[constants::Common::REDUNDANCY] = json::Value::Type::ARRAY;
    r[constants::Endpoint::HOST_RESERVATION_MEMORY_BYTES] = json::Value::Type::NIL;
    r[Common::LINKS][Switch::PORTS] = json::Value::Type::ARRAY;
    r[Common::ACTIONS][Common::OEM] = json::Value::Type::OBJECT;
    r[Common::OEM] = json::Value::Type::OBJECT;

    return r;
}


json::Value make_entity_prototype() {
    json::Value json;
    json[constants::Endpoint::ENTITY_ROLE] = json::Value::Type::NIL;
    json[constants::Endpoint::ENTITY_TYPE] = json::Value::Type::NIL;
    json[constants::Endpoint::PCI_FUNCTION_NUMBER] = json::Value::Type::NIL;
    json[constants::Endpoint::PCI_CLASS_CODE] = json::Value::Type::NIL;
    json[constants::Endpoint::ENTITY_PCI_ID] = json::Value::Type::NIL;
    json[constants::Endpoint::ENTITY_LINK] = json::Value::Type::NIL;
    json[constants::PncDrive::IDENTIFIERS] = json::Value::Type::ARRAY;
    json[constants::Common::OEM] = json::Value::Type::OBJECT;
    return json;
}


json::Value
entity_to_json(const agent_framework::model::attribute::ConnectedEntity& entity,
               const agent_framework::model::Endpoint& endpoint) {
    json::Value json = make_entity_prototype();
    json[constants::Endpoint::ENTITY_ROLE] = entity.get_entity_role();
    json[constants::Endpoint::ENTITY_TYPE] = entity.get_entity_type();
    if (entity.get_entity_type() != agent_framework::model::enums::EntityType::Drive) {
        return json;
    }

    if (!entity.get_entity().has_value()) {
        log_warning(GET_LOGGER("rest"),
                 "No drive specified for Drive entity in endpoint " + endpoint.get_uuid()
                 + ". Current endpoint status is " + endpoint.get_status().get_state().to_string() + ".");
        return json;
    }

    auto& drive_manager = agent_framework::module::get_manager<agent_framework::model::Drive>();

    if (!drive_manager.entry_exists(entity.get_entity())) {
        log_error(GET_LOGGER("rest"),
                 "Unknown entity " + entity.get_entity().value() + " detected under Endpoint " + endpoint.get_uuid() + ".");
        return json;
    }

    const auto drive = drive_manager.get_entry(entity.get_entity());
    json[constants::Endpoint::ENTITY_LINK][Common::ODATA_ID] = endpoint::PathBuilder(
        endpoint::utils::get_component_url(agent_framework::model::enums::Component::Drive,
                                           drive.get_uuid())).build();

    for (const auto& identifier : drive.get_identifiers()) {
        json::Value id;
        id[PncDrive::DURABLE_NAME] = identifier.get_durable_name();
        id[PncDrive::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format().to_string();
        json[PncDrive::IDENTIFIERS].push_back(std::move(id));
    }

    const auto drive_function_uuids = agent_framework::module::PncComponents::get_instance()->get_drive_function_manager().get_children(
        drive.get_uuid());

    if (drive_function_uuids.size() == 0) {
        log_info(GET_LOGGER("rest"),
                 "PNC Drive " + drive.get_uuid() + " has no PCIeFunctions!");
        return json;
    }

    try {
        const auto function = agent_framework::module::get_manager<agent_framework::model::PcieFunction>().get_entry(
            drive_function_uuids.front());
        try {
            if (function.get_function_id().has_value()) {
                json[constants::Endpoint::PCI_FUNCTION_NUMBER] = std::stoi(function.get_function_id());
            }
        }
        catch (const std::exception& ex) {
            log_warning(GET_LOGGER("rest"), "Invalid function id type:" << ex.what());
        }
        json[constants::Endpoint::PCI_CLASS_CODE] = function.get_pci_class_code();
        json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::DEVICE_ID] = function.get_pci_device_id();
        json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::VENDOR_ID] = function.get_pci_vendor_id();
        json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::SUBSYSTEM_ID] = function.get_pci_subsystem_id();
        json[constants::Endpoint::ENTITY_PCI_ID][PcieFunction::SUBSYSTEM_VENDOR_ID] = function.get_pci_subsystem_vendor_id();
    }
    catch (agent_framework::exceptions::InvalidUuid&) {
        log_error(GET_LOGGER("rest"),
                  "Drive " + drive.get_uuid() + " has non-existing PCIeFunctions!");
    }

    return json;
}

}


endpoint::Endpoint::Endpoint(const std::string& path) : EndpointBase(path) {}


endpoint::Endpoint::~Endpoint() {}


void endpoint::Endpoint::get(const server::Request& req, server::Response& res) {
    auto json = ::make_prototype();

    json[Common::ODATA_ID] = PathBuilder(req).build();

    const auto endpoint = psme::rest::model::Find<agent_framework::model::Endpoint>(req.params[PathParam::ENDPOINT_ID])
        .via<agent_framework::model::Fabric>(req.params[PathParam::FABRIC_ID]).get();

    json[Common::ID] = req.params[PathParam::FABRIC_ID];

    endpoint::status_to_json(endpoint, json);
    json[constants::Endpoint::ENDPOINT_PROTOCOL] = endpoint.get_protocol();

    for (const auto& identifier : endpoint.get_identifiers()) {
        json::Value id_link;
        id_link[PncDrive::DURABLE_NAME] = identifier.get_durable_name();
        id_link[PncDrive::DURABLE_NAME_FORMAT] = identifier.get_durable_name_format().to_string();
        json[constants::PncDrive::IDENTIFIERS].push_back(std::move(id_link));
    }

    for (const auto& entity : endpoint.get_connected_entities()) {
        json[constants::Endpoint::CONNECTED_ENTITIES].push_back(std::move(entity_to_json(entity, endpoint)));
    }

    for (const auto& port_uuid : agent_framework::module::PncComponents::get_instance()->get_endpoint_port_manager().get_children(
        endpoint.get_uuid())) {
        json::Value port_link;
        port_link[Common::ODATA_ID] = endpoint::PathBuilder(
            endpoint::utils::get_component_url(agent_framework::model::enums::Component::Port,
                                               port_uuid)).build();
        json[Common::LINKS][Switch::PORTS].push_back(std::move(port_link));
    }

    set_response(res, json);
}

