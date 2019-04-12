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
 * @file add_zone.cpp
 */

#include "agent-framework/module/requests/common/add_zone.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

AddZone::AddZone(const std::string& pcie_fabric, const StringArray& endpoints,
                         const attribute::Oem& oem)
                         : m_fabric(pcie_fabric), m_endpoints(endpoints), m_oem(oem) {}

json::Json AddZone::to_json() const {
    json::Json value = json::Json();
    value[Zone::FABRIC] = m_fabric;
    value[Zone::ENDPOINTS] = m_endpoints.to_json();
    value[Zone::OEM] = m_oem.to_json();
    return value;
}

AddZone AddZone::from_json(const json::Json& json) {
    return AddZone{
        json[Zone::FABRIC],
        StringArray::from_json(json[Zone::ENDPOINTS]),
        attribute::Oem::from_json(json[Zone::OEM])};
}
