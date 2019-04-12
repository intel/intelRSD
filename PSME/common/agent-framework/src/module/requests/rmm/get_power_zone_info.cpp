/*!
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
 * @file request/rmm/get_power_zone_info.cpp
 * @brief RMM request get power zone information implementation
 * */

#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/requests/rmm/get_power_zone_info.hpp"



using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


GetPowerZoneInfo::GetPowerZoneInfo(const std::string& zone) : m_zone{zone} {}


json::Json GetPowerZoneInfo::to_json() const {
    json::Json value = json::Json();
    value[PowerZone::ZONE] = m_zone;
    return value;
}


GetPowerZoneInfo GetPowerZoneInfo::from_json(const json::Json& json) {
    return GetPowerZoneInfo{json[PowerZone::ZONE].get<std::string>()};
}
