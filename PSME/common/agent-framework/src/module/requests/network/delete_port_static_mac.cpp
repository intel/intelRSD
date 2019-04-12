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
 * @file requests/network/delete_port_static_mac.cpp
 *
 * @brief Network request DeletePortStaticMac implementation
 * */

#include "agent-framework/module/requests/network/delete_port_static_mac.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/constants/common.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

DeletePortStaticMac::DeletePortStaticMac(const std::string& static_mac, Oem oem):
                m_static_mac{static_mac},
                m_oem{oem} {}

json::Json DeletePortStaticMac::to_json() const {
    json::Json value = json::Json();
    value[StaticMac::STATIC_MAC] = m_static_mac;
    value[StaticMac::OEM] = m_oem.to_json();
    return value;
}

DeletePortStaticMac DeletePortStaticMac::from_json(const json::Json& json) {
    return DeletePortStaticMac{
        json[StaticMac::STATIC_MAC],
        Oem{}
    };
}
