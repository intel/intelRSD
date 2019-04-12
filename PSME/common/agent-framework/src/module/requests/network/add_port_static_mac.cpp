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
 * @file requests/network/add_port_static_mac.cpp
 *
 * @brief Network request AddPortStaticMac implementation
 * */

#include "agent-framework/module/requests/network/add_port_static_mac.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddPortStaticMac::AddPortStaticMac(const OptionalField<std::string>& port,
            const OptionalField<std::string>& mac_address,
            const OptionalField<uint32_t>& vlan_id,
            agent_framework::model::attribute::Oem):
                m_port{port},
                m_mac_address{mac_address},
                m_vlan_id{vlan_id},
                m_oem{} {}

json::Json AddPortStaticMac::to_json() const {
    json::Json value = json::Json();
    value[StaticMac::PORT] = m_port;
    value[StaticMac::ADDRESS] = m_mac_address;
    value[StaticMac::VLAN_ID] = m_vlan_id;
    value[StaticMac::OEM] = m_oem.to_json();
    return value;
}

AddPortStaticMac AddPortStaticMac::from_json(const json::Json& json) {
    return AddPortStaticMac{
        json[StaticMac::PORT],
        json[StaticMac::ADDRESS],
        json.value(StaticMac::VLAN_ID, OptionalField<uint32_t>()),
        agent_framework::model::attribute::Oem::from_json(json[StaticMac::OEM])
    };
}
