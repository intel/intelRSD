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
 * @file delete_zone.cpp
 */

#include "agent-framework/module/requests/common/delete_zone.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;
using namespace agent_framework::model;

DeleteZone::DeleteZone(const std::string& zone): m_zone(zone) {}

json::Json DeleteZone::to_json() const {
    json::Json value = json::Json();
    value[literals::Zone::ZONE] = m_zone;
    return value;
}

DeleteZone DeleteZone::from_json(const json::Json& json) {
    return DeleteZone{json[Zone::ZONE].get<std::string>()};
}
