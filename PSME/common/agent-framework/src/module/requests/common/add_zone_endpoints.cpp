/*!
 * @copyright Copyright (c) 2016-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file add_zone_endpoints.cpp
 */

#include "agent-framework/module/requests/common/add_zone_endpoints.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


AddZoneEndpoints::AddZoneEndpoints(const Uuid& zone, const UuidArray& endpoints, const attribute::Oem& oem)
    : m_zone(zone), m_endpoints(endpoints), m_oem(oem) {}


json::Json AddZoneEndpoints::to_json() const {
    json::Json value = json::Json();
    value[Zone::ZONE] = m_zone;
    value[Zone::ENDPOINTS] = m_endpoints.to_json();
    value[Zone::OEM] = m_oem.to_json();
    return value;
}


AddZoneEndpoints AddZoneEndpoints::from_json(const json::Json& json) {
    return AddZoneEndpoints{
        json[Zone::ZONE],
        UuidArray::from_json(json[Zone::ENDPOINTS]),
        attribute::Oem::from_json(json[Zone::OEM])
    };
}
