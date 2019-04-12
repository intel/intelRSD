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

#include "agent-framework/module/model/remote_ethernet_switch.hpp"
#include "agent-framework/module/constants/network.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component RemoteEthernetSwitch::component =
    enums::Component::RemoteEthernetSwitch;
const enums::CollectionName RemoteEthernetSwitch::collection_name =
    enums::CollectionName::RemoteEthernetSwitches;


RemoteEthernetSwitch::RemoteEthernetSwitch(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


RemoteEthernetSwitch::~RemoteEthernetSwitch() {}


json::Json RemoteEthernetSwitch::to_json() const {
    json::Json result = json::Json();
    result[literals::RemoteEthernetSwitch::STATUS] = get_status().to_json();
    result[literals::RemoteEthernetSwitch::SWITCH_IDENTIFIER] = get_switch_identifier();
    result[literals::RemoteEthernetSwitch::MAC_ADDRESS] = get_mac_address();
    result[literals::RemoteEthernetSwitch::NEXT_HOP] = get_next_hop().to_json();
    result[literals::RemoteEthernetSwitch::OEM] = get_oem().to_json();
    return result;
}


RemoteEthernetSwitch RemoteEthernetSwitch::from_json(const json::Json& json) {
    RemoteEthernetSwitch rs;

    rs.set_status(attribute::Status::from_json(json[literals::RemoteEthernetSwitch::STATUS]));
    rs.set_switch_identifier(json[literals::RemoteEthernetSwitch::SWITCH_IDENTIFIER]);
    rs.set_mac_address(json[literals::RemoteEthernetSwitch::MAC_ADDRESS]);
    rs.set_next_hop(attribute::Array<attribute::NextHop>::from_json(json[literals::RemoteEthernetSwitch::NEXT_HOP]));
    rs.set_oem(attribute::Oem::from_json(json[literals::RemoteEthernetSwitch::OEM]));

    return rs;
}
