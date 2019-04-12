/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/attributes/neighbor_info.hpp"
#include "agent-framework/module/constants/network.hpp"

using namespace agent_framework::model::attribute;

NeighborInfo::NeighborInfo() { }

NeighborInfo::~NeighborInfo() { }

json::Json NeighborInfo::to_json() const {
    json::Json result = json::Json();
    result[literals::NeighborInfo::SWITCH_IDENTIFIER] = get_switch_identifier();
    result[literals::NeighborInfo::PORT_IDENTIFIER] = get_port_identifier();
    result[literals::NeighborInfo::CABLE_ID] = get_cable_id();
    return result;
}

NeighborInfo NeighborInfo::from_json(const json::Json& json) {
    attribute::NeighborInfo neighbor_info;
    neighbor_info.set_switch_identifier(json[literals::NeighborInfo::SWITCH_IDENTIFIER]);
    neighbor_info.set_port_identifier(json[literals::NeighborInfo::PORT_IDENTIFIER]);
    neighbor_info.set_cable_id(json[literals::NeighborInfo::CABLE_ID]);
    return neighbor_info;
}
