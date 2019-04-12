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
 * @file command/pnc/get_port_info.cpp
 *
 * @brief Pnc request get port information implementation
 * */

#include "agent-framework/module/requests/pnc/get_port_info.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;

GetPortInfo::GetPortInfo(const std::string& port): m_port{port} {}

json::Json GetPortInfo::to_json() const {
    json::Json value = json::Json();
    value[Port::PORT] = m_port;
    return value;
}

GetPortInfo GetPortInfo::from_json(const json::Json& json) {
    return GetPortInfo{json[Port::PORT].get<std::string>()};
}
