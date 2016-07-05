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
 * @file requests/network/add_vlan.cpp
 *
 * @brief Network request AddVlan implementation
 * */

#include "agent-framework/module-ref/requests/network/add_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddVlan::AddVlan(const std::string& switch_id,
            const std::string& vlan_name,
            unsigned int vlan_id,
            agent_framework::model::attribute::Oem):
                m_switch(switch_id),
                m_vlan_name(vlan_name),
                m_vlan_id(vlan_id),
                m_oem{} {}

Json::Value AddVlan::to_json() const {
    Json::Value value;
    value[Vlan::SWITCH] = m_switch;
    value[Vlan::VLAN_NAME] = m_vlan_name;
    value[Vlan::VLAN_ID] = m_vlan_id;
    value[Oem::OEM] = m_oem.to_json();
    return value;
}

AddVlan AddVlan::from_json(const Json::Value& json) {
    return AddVlan{
        json[Vlan::SWITCH].asString(),
        json[Vlan::VLAN_NAME].asString(),
        json[Vlan::VLAN_ID].asUInt(),
        agent_framework::model::attribute::Oem::from_json(
          json[Vlan::OEM])
    };
}
