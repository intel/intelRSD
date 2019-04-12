/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file command/pnc/get_switch_info.cpp
 *
 * @brief Pnc request get switch information implementation
 * */

#include "agent-framework/module/requests/pnc/get_switch_info.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetSwitchInfo::GetSwitchInfo(const std::string& pcie_switch): m_switch{pcie_switch} {}

json::Json GetSwitchInfo::to_json() const {
    json::Json value = json::Json();
    value[Switch::SWITCH] = m_switch;
    return value;
}

GetSwitchInfo GetSwitchInfo::from_json(const json::Json& json) {
    return GetSwitchInfo{json[Switch::SWITCH].get<std::string>()};
}
