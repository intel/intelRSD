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
 * @file command/common/get_pcie_function_info.cpp
 *
 * @brief Common request get function information implementation
 * */

#include "agent-framework/module/requests/common/get_pcie_function_info.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetPcieFunctionInfo::GetPcieFunctionInfo(const std::string& function): m_function{function} {}

json::Json GetPcieFunctionInfo::to_json() const {
    json::Json value = json::Json();
    value[PcieFunction::FUNCTION] = m_function;
    return value;
}

GetPcieFunctionInfo GetPcieFunctionInfo::from_json(const json::Json& json) {
    return GetPcieFunctionInfo{json[PcieFunction::FUNCTION].get<std::string>()};
}
