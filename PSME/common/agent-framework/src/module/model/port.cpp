/*!
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
 * */

#include "agent-framework/module/model/port.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component Port::component = enums::Component::Port;
const enums::CollectionName Port::collection_name = enums::CollectionName::Ports;

Port::Port(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

Port::~Port() {}

json::Json Port::to_json() const {
    json::Json result = json::Json();
    result[literals::Port::PORT_ID] = get_port_id();
    result[literals::Port::PORT_TYPE] = get_port_type();
    result[literals::Port::CABLE_IDS] = get_cables_ids().to_json();
    result[literals::Port::SPEED_GBPS] = get_speed_gbps();
    result[literals::Port::MAX_SPEED_GBPS] = get_max_speed_gbps();
    result[literals::Port::MAX_WIDTH] = get_max_width();
    result[literals::Port::WIDTH] = get_width();
    result[literals::Port::STATUS] = get_status().to_json();
    result[literals::Port::OEM] = get_oem().to_json();
    result[literals::Port::ALLOWED_ACTIONS] = get_allowed_actions().to_json();
    result[literals::Port::PROTOCOL] = get_protocol();
    return result;
}

Port Port::from_json(const json::Json& json) {
    Port fabric_port;
    fabric_port.set_port_id(json[literals::Port::PORT_ID]);
    fabric_port.set_port_type(json[literals::Port::PORT_TYPE]);
    fabric_port.set_cables_ids(CablesIds::from_json(json[literals::Port::CABLE_IDS]));
    fabric_port.set_speed_gbps(json[literals::Port::SPEED_GBPS]);
    fabric_port.set_max_speed_gbps(json[literals::Port::MAX_SPEED_GBPS]);
    fabric_port.set_max_width(json[literals::Port::MAX_WIDTH]);
    fabric_port.set_width(json[literals::Port::WIDTH]);
    fabric_port.set_status(Status::from_json(json[literals::Port::STATUS]));
    fabric_port.set_oem(Oem::from_json(json[literals::Port::OEM]));
    fabric_port.set_allowed_actions(AllowedActions::from_json(json[literals::Port::ALLOWED_ACTIONS]));
    fabric_port.set_protocol(json[literals::Port::PROTOCOL]);
    return fabric_port;
}
