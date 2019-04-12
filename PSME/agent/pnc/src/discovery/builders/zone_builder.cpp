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
 * @file zone_builder.cpp
 *
 * @brief ZoneBuilder implementation
 * */

#include "discovery/builders/zone_builder.hpp"
#include "discovery/discovery_utils.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent::pnc::gas::partition;
using namespace agent::pnc::discovery::utils;

void ZoneBuilder::build_default() {
    attribute::Status status;
    status.set_health(enums::Health::OK);
    status.set_state(enums::State::Enabled);
    m_obj.set_status(std::move(status));

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::Endpoints,
        enums::CollectionType::Endpoints
    ));
}

ZoneBuilder::ReturnType ZoneBuilder::update_id(std::uint8_t zone_id) {
    m_obj.set_zone_id(zone_id);
    return *this;
}

ZoneBuilder::ReturnType ZoneBuilder::update_pc(const PartitionConfiguration& pc) {
    attribute::Status status{};
    status.set_health(partition_state_to_status_health(pc.output.fields.state));
    status.set_state(partition_status_to_status_state(pc.output.fields.status));
    m_obj.set_status(std::move(status));
    return *this;
}

ZoneBuilder::ReturnType ZoneBuilder::update_links(const std::string& switch_uuid) {
    m_obj.set_switch_uuid(switch_uuid);
    return *this;
}
