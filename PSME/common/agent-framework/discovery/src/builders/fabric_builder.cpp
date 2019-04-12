/*!
 * @brief Fabric Builder implementation
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
 * @file fabric_builder.cpp
 * */

#include "agent-framework/discovery/builders/fabric_builder.hpp"

using namespace agent_framework::discovery;


agent_framework::model::Fabric FabricBuilder::build_default(const Uuid& parent_uuid) {
    model::Fabric fabric{parent_uuid};

    model::attribute::Status status{};
    status.set_health(model::enums::Health::OK);
    status.set_state(model::enums::State::Enabled);
    fabric.set_status(std::move(status));

    fabric.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Zones,
        model::enums::CollectionType::Zones
    ));
    fabric.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Endpoints,
        model::enums::CollectionType::Endpoints
    ));

    return fabric;
}

void FabricBuilder::nvmeof_protocol(agent_framework::model::Fabric& fabric) {
    fabric.set_protocol(agent_framework::model::enums::TransportProtocol::NVMeOverFabrics);
}

void FabricBuilder::iscsi_protocol(agent_framework::model::Fabric& fabric) {
    fabric.set_protocol(agent_framework::model::enums::TransportProtocol::iSCSI);
}

void FabricBuilder::fpgaof_protocol(agent_framework::model::Fabric& fabric) {
    fabric.set_protocol(agent_framework::model::enums::TransportProtocol::OEM);
    fabric.set_oem_protocol(agent_framework::model::enums::OemProtocol::FPGAoF);
}
