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
    json::Json result{};
    result[literals::Endpoint::FABRIC] = get_fabric();
    result[literals::Endpoint::PROTOCOL] = get_protocol();
    result[literals::Endpoint::IDENTIFIERS] = get_identifiers().to_json();
    result[literals::Endpoint::CONNECTED_ENTITIES] = get_connected_entities().to_json();
    result[literals::Endpoint::IP_TRANSPORT_DETAILS] = get_ip_transport_details().to_json();
    result[literals::Endpoint::COLLECTIONS] = get_collections().to_json();
    result[literals::Endpoint::STATUS] = get_status().to_json();
    result[literals::Endpoint::USERNAME] = get_username();
    result[literals::Endpoint::PASSWORD] = get_password();
    result[literals::Endpoint::OEM] = get_oem().to_json();
    return result;
}

Endpoint Endpoint::from_json(const json::Json& json) {
    Endpoint endpoint{json[literals::Endpoint::FABRIC].get<std::string>()};

    endpoint.set_protocol(json[literals::Endpoint::PROTOCOL]);
    endpoint.set_connected_entities(ConnectedEntities::from_json(json[literals::Endpoint::CONNECTED_ENTITIES]));
    endpoint.set_identifiers(Identifiers::from_json(json[literals::Endpoint::IDENTIFIERS]));
    endpoint.set_ip_transport_details(IpTransportDetails::from_json(json[literals::Endpoint::IP_TRANSPORT_DETAILS]));
    endpoint.set_collections(Collections::from_json(json[literals::Endpoint::COLLECTIONS]));
    endpoint.set_username(json[literals::Endpoint::USERNAME]);
    endpoint.set_password(json[literals::Endpoint::PASSWORD]);
    endpoint.set_status(attribute::Status::from_json(json[literals::Endpoint::STATUS]));
    endpoint.set_oem(attribute::Oem::from_json(json[literals::Endpoint::OEM]));

    return endpoint;
}

bool Endpoint::has_drive_entity(const std::string& drive_uuid) const {
    for (const auto& entity : m_connected_entities){
        if (drive_uuid == entity.get_entity()) {
            return true;
        }
    }
    return false;
}
