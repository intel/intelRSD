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
 * @file request/rmm/get_psu_info.cpp
 * @brief RMM request get PSU information implementation
 * */

#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/requests/rmm/get_psu_info.hpp"



using namespace agent_framework::model::requests;
using namespace agent_framework::model::literals;


GetPsuInfo::GetPsuInfo(const std::string& psu) : m_psu{psu} {}


json::Json GetPsuInfo::to_json() const {
    json::Json value = json::Json();
    value[Psu::PSU] = m_psu;
    return value;
}


GetPsuInfo GetPsuInfo::from_json(const json::Json& json) {
    return GetPsuInfo{json[Psu::PSU].get<std::string>()};
}
