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
 * @file requests/network/add_port_vlan.cpp
 *
 * @brief Network request AddPortVlan implementation
 * */

#include "agent-framework/module-ref/requests/network/add_port_vlan.hpp"
#include "agent-framework/module-ref/constants/network.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include <json/json.h>

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddPortVlan::AddPortVlan(const std::string& port,
            const OptionalField<std::string>& vlan_name,
            unsigned int vlan_id,
            bool tagged,
            agent_framework::model::attribute::Oem):
                m_port(port),
                m_vlan_name(vlan_name),
                m_vlan_id(vlan_id),
                m_tagged(tagged),
                m_oem{} {}

Json::Value AddPortVlan::to_json() const {
    Json::Value value;
    value[PortVlan::PORT] = m_port;
    value[PortVlan::VLAN_NAME] = m_vlan_name;
    value[PortVlan::VLAN_ID] = m_vlan_id;
    value[PortVlan::TAGGED] = m_tagged;
    value[PortVlan::OEM] = m_oem.to_json();
    return value;
}

AddPortVlan AddPortVlan::from_json(const Json::Value& json) {
    return AddPortVlan{
        json[PortVlan::PORT].asString(),
        json[PortVlan::VLAN_NAME],
        json[PortVlan::VLAN_ID].asUInt(),
        json[PortVlan::TAGGED].asBool(),
        agent_framework::model::attribute::Oem::from_json(json[PortVlan::OEM])
    };
}
