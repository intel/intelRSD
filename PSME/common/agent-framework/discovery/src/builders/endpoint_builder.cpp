/*!
 * @brief Endpoint Builder implementation
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file endpoint_builder.cpp
 */

#include "agent-framework/discovery/builders/endpoint_builder.hpp"

using namespace agent_framework::discovery;


agent_framework::model::Endpoint EndpointBuilder::build_default(const Uuid& parent_uuid) {
    model::Endpoint endpoint{parent_uuid};

    endpoint.set_parent_type(model::enums::Component::Fabric);

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    endpoint.set_status(std::move(status));

    return endpoint;
}


void EndpointBuilder::iscsi_protocol(agent_framework::model::Endpoint& endpoint) {
    endpoint.set_protocol(agent_framework::model::enums::TransportProtocol::iSCSI);
}

void EndpointBuilder::nvmeof_protocol(agent_framework::model::Endpoint& endpoint) {
    endpoint.set_protocol(agent_framework::model::enums::TransportProtocol::NVMeOverFabrics);
}

void EndpointBuilder::fpgaof_protocol(agent_framework::model::Endpoint& endpoint) {
    endpoint.set_protocol(agent_framework::model::enums::TransportProtocol::OEM);
}

void EndpointBuilder::add_initiator_connected_entity(agent_framework::model::Endpoint& endpoint) {
    model::attribute::ConnectedEntity ce{};
    ce.set_entity_role(model::enums::EntityRole::Initiator);
    endpoint.add_connected_entity(ce);
}

void EndpointBuilder::add_target_connected_entity(agent_framework::model::Endpoint& endpoint, const Uuid& entity) {
    model::attribute::ConnectedEntity ce{};
    ce.set_entity_role(model::enums::EntityRole::Target);
    ce.set_entity(entity);
    endpoint.add_connected_entity(ce);
}
