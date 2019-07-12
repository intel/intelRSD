/*!
* @brief Implementation of GetLogServiceInfo class.
*
* @copyright Copyright (c) 2019 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* @file get_log_service_info.cpp
*/

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/requests/common/get_log_service_info.hpp"
#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


GetLogServiceInfo::GetLogServiceInfo(const Uuid& log_service): m_log_service{log_service} {}


json::Json GetLogServiceInfo::to_json() const {
    json::Json value = json::Json();
    value[LogService::SERVICE] = m_log_service;
    return value;
}


GetLogServiceInfo GetLogServiceInfo::from_json(const json::Json& json) {
    return GetLogServiceInfo{json[LogService::SERVICE].get<Uuid>()};
}
