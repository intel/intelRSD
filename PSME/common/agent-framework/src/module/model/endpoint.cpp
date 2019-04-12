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


#include "agent-framework/module/model/zone.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/endpoint.hpp"
#include "agent-framework/module/constants/pnc.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component Endpoint::component = enums::Component::Endpoint;
const enums::CollectionName Endpoint::collection_name = enums::CollectionName::Endpoints;


Endpoint::Endpoint(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


Endpoint::~Endpoint() {}


json::Json Endpoint::to_json() const {
    json::Json result = json::Json();
    result[literals::Endpoint::NAME] = get_name();
    result[literals::Endpoint::DESCRIPTION] = get_description();
    result[literals::Endpoint::PROTOCOL] = get_protocol();
    result[literals::Endpoint::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::Endpoint::CONNECTED_ENTITIES] = get_connected_entities().to_json();
    result[literals::Endpoint::IP_TRANSPORT_DETAILS] = get_ip_transport_details().to_json();
    result[literals::Endpoint::COLLECTIONS] = get_collections().to_json();
    result[literals::Endpoint::STATUS] = get_status().to_json();
    result[literals::Endpoint::USERNAME] = get_username();
    result[literals::Endpoint::PASSWORD] = {}; // Password shall not be send from Agents
    result[literals::Endpoint::OEM] = get_oem().to_json();
    result[literals::Endpoint::OEM_PROTOCOL] = get_oem_protocol();
    return result;
}


Endpoint Endpoint::from_json(const json::Json& json) {
    Endpoint endpoint{};

    endpoint.set_name(json[literals::Endpoint::NAME]);
    endpoint.set_description(json[literals::Endpoint::DESCRIPTION]);
    endpoint.set_protocol(json[literals::Endpoint::PROTOCOL]);
    endpoint.set_connected_entities(ConnectedEntities::from_json(json[literals::Endpoint::CONNECTED_ENTITIES]));
    endpoint.set_identifiers(Identifiers::from_json(json[literals::Endpoint::IDENTIFIERS]));
    endpoint.set_ip_transport_details(IpTransportDetails::from_json(json[literals::Endpoint::IP_TRANSPORT_DETAILS]));
    endpoint.set_collections(Collections::from_json(json[literals::Endpoint::COLLECTIONS]));
    endpoint.set_username(json[literals::Endpoint::USERNAME]);
    endpoint.set_password(json[literals::Endpoint::PASSWORD]);
    endpoint.set_status(attribute::Status::from_json(json[literals::Endpoint::STATUS]));
    endpoint.set_oem(attribute::Oem::from_json(json[literals::Endpoint::OEM]));
    endpoint.set_oem_protocol(json[literals::Endpoint::OEM_PROTOCOL]);

    return endpoint;
}


bool Endpoint::has_entity(const Uuid& endpoint_uuid, const Uuid& resource_uuid) {
    auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);
    for (const auto& entity : endpoint.get_connected_entities()) {
        if (resource_uuid == entity.get_entity()) {
            return true;
        }
    }
    return false;
}


bool Endpoint::is_resource_in_endpoint(const Uuid& resource_uuid) {
    for (const auto& endpoint : module::get_manager<Endpoint>().get_keys()) {
        if (has_entity(endpoint, resource_uuid)) {
            return true;
        }
    }
    return false;
}


bool Endpoint::is_in_role(const Uuid& endpoint_uuid, const agent_framework::model::enums::EntityRole& role) {
    auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);
    return Endpoint::is_in_role(endpoint, role);
}


bool Endpoint::is_in_role(const Endpoint& endpoint, const enums::EntityRole& role) {
    for (const auto& entity : endpoint.get_connected_entities()) {
        if (role == entity.get_entity_role()) {
            return true;
        }
    }
    return false;
}


bool Endpoint::is_bound_to_zone(const Uuid& endpoint_uuid) {
    auto parents = agent_framework::module::get_m2m_manager<Zone, Endpoint>().get_parents(endpoint_uuid);
    return !parents.empty();
}


bool Endpoint::is_initiator(const Uuid& endpoint_uuid) {
    auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);
    return Endpoint::is_initiator(endpoint);
}


bool Endpoint::is_target(const Uuid& endpoint_uuid) {
    auto endpoint = module::get_manager<Endpoint>().get_entry(endpoint_uuid);
    return Endpoint::is_target(endpoint);
}


bool Endpoint::is_target(const agent_framework::model::Endpoint& endpoint) {
    return Endpoint::is_in_role(endpoint, enums::EntityRole::Target) ||
           Endpoint::is_in_role(endpoint, enums::EntityRole::Both);
}


bool Endpoint::is_initiator(const agent_framework::model::Endpoint& endpoint) {
    return Endpoint::is_in_role(endpoint, enums::EntityRole::Initiator) ||
           Endpoint::is_in_role(endpoint, enums::EntityRole::Both);
}