/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file command/pnc/add_zone_endpoint.cpp
 *
 * @brief Pnc request add zone information implementation
 * */

#include "agent-framework/module/requests/pnc/add_zone_endpoint.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include <json/json.h>

using namespace agent_framework::model;
using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddZoneEndpoint::AddZoneEndpoint(const std::string& zone, const StringArray& endpoints,
                         const attribute::Oem& oem)
                         : m_zone(zone), m_endpoints(endpoints), m_oem(oem) {}

Json::Value AddZoneEndpoint::to_json() const {
    Json::Value value;
    value[Zone::ZONE] = m_zone;
    value[Zone::ENDPOINTS] = m_endpoints.to_json();
    value[Zone::OEM] = m_oem.to_json();
    return value;
}

AddZoneEndpoint AddZoneEndpoint::from_json(const Json::Value& json) {
    return AddZoneEndpoint{
        json[Zone::ZONE].asString(),
        StringArray::from_json(json[Zone::ENDPOINTS]),
        attribute::Oem::from_json(json[Zone::OEM])};
}
