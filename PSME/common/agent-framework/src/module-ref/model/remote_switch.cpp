/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "agent-framework/module-ref/model/remote_switch.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component RemoteSwitch::component =
    enums::Component::RemoteSwitch;
const enums::CollectionName RemoteSwitch::collection_name =
    enums::CollectionName::RemoteSwitches;

RemoteSwitch::RemoteSwitch(const std::string& parent_uuid) :
    Resource{parent_uuid} {}

RemoteSwitch::~RemoteSwitch() {}

Json::Value RemoteSwitch::to_json() const {
    Json::Value result;
    result[literals::RemoteSwitch::STATUS] = get_status().to_json();
    result[literals::RemoteSwitch::SWITCH_IDENTIFIER] =
        get_switch_identifier();
    result[literals::RemoteSwitch::MAC_ADDRESS] = get_mac_address();
    result[literals::RemoteSwitch::NEXT_HOP] =
        Json::Value(Json::ValueType::arrayValue);
    for(const auto& hop : get_next_hop()){
        result[literals::RemoteSwitch::NEXT_HOP].append(hop.to_json());
    }
    result[literals::RemoteSwitch::OEM] = get_oem().to_json();
    return result;
}

RemoteSwitch RemoteSwitch::from_json(const Json::Value& json)
{
    RemoteSwitch rs;

    rs.set_status(attribute::Status::from_json(json[literals::RemoteSwitch::STATUS]));
    rs.set_switch_identifier(json[literals::RemoteSwitch::SWITCH_IDENTIFIER]);
    rs.set_mac_address(json[literals::RemoteSwitch::MAC_ADDRESS]);
    for(const auto& hop : json[literals::RemoteSwitch::NEXT_HOP])
        rs.add_next_hop(attribute::NextHop::from_json(hop));
    rs.set_oem(attribute::Oem::from_json(json[literals::RemoteSwitch::OEM]));
    rs.set_resource_hash(compute_hash(json));

    return rs;
}
