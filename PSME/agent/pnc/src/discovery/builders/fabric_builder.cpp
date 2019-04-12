/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 *
 * @file fabric_builder.cpp
 *
 * @brief FabricBuilder implementation
 * */

#include "discovery/builders/fabric_builder.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;

void FabricBuilder::build_default() {
    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    m_obj.set_status(std::move(status));

    m_obj.set_protocol(enums::TransportProtocol::PCIe);

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Switches,
        enums::CollectionType::Switches
    ));
    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Zones,
        enums::CollectionType::Zones
    ));
    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Endpoints,
        enums::CollectionType::Endpoints
    ));
}
