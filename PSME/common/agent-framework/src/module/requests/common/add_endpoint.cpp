/*!
 * @brief Implementation of AddEndpoint class.
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
 * @file add_endpoint.cpp
 */

#include "agent-framework/module/requests/common/add_endpoint.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddEndpoint::AddEndpoint(const Uuid& fabric,
                         const IpTransportDetails& transports,
                         const Identifiers& identifiers,
                         const ConnectedEntities& connected_entities,
                         const OptionalField<std::string>& username,
                         const OptionalField<std::string>& password,
                         const Ports& ports,
                         const attribute::Oem& oem) :
    m_ip_transport_details(transports), m_identifiers(identifiers), m_fabric(fabric),
    m_connected_entities(connected_entities), m_username(username),
    m_password(password), m_ports(ports), m_oem(oem) {}

json::Json AddEndpoint::to_json() const {
    json::Json value = json::Json();
    value[Endpoint::FABRIC] = m_fabric;
    value[Endpoint::IP_TRANSPORT_DETAILS] = m_ip_transport_details.to_json();
    value[Endpoint::IDENTIFIERS] = m_identifiers.to_json();
    value[Endpoint::CONNECTED_ENTITIES] = m_connected_entities.to_json();
    value[Endpoint::USERNAME] = m_username;
    value[Endpoint::PASSWORD] = m_password;
    value[Endpoint::PORTS] = m_ports.to_json();
    value[Endpoint::OEM] = m_oem.to_json();
    return value;
}

AddEndpoint AddEndpoint::from_json(const json::Json& json) {
    return AddEndpoint{
        json[Endpoint::FABRIC],
        IpTransportDetails::from_json(json[Endpoint::IP_TRANSPORT_DETAILS]),
        Identifiers::from_json(json[Endpoint::IDENTIFIERS]),
        ConnectedEntities::from_json(json[Endpoint::CONNECTED_ENTITIES]),
        json[Endpoint::USERNAME],
        json[Endpoint::PASSWORD],
        Ports::from_json(json.value(Endpoint::PORTS, json::Json::array())),
        attribute::Oem::from_json(json[Endpoint::OEM])
    };
}
