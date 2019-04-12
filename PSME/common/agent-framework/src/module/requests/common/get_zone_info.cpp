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
 * @file get_zone_info.cpp
 */

#include "agent-framework/module/requests/common/get_zone_info.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetZoneInfo::GetZoneInfo(const std::string& zone): m_zone{zone} {}

json::Json GetZoneInfo::to_json() const {
    json::Json value = json::Json();
    value[Zone::ZONE] = m_zone;
    return value;
}

GetZoneInfo GetZoneInfo::from_json(const json::Json& json) {
    return GetZoneInfo{json[Zone::ZONE].get<std::string>()};
}
