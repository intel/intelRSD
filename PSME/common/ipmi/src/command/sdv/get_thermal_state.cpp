/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file get_thremal_state.cpp
 *
 * @brief Get Thermal State request and response.
 * */
#include "ipmi/command/sdv/get_thermal_state.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetThermalState::GetThermalState() :
        Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_THERMAL_STATE) {}
request::GetThermalState::~GetThermalState() {}

void request::GetThermalState::pack(std::vector<std::uint8_t>& data) const {
    (void)data;
}

response::GetThermalState::GetThermalState() :
        Response(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_THERMAL_STATE, RESPONSE_SIZE) {}
response::GetThermalState::~GetThermalState() {}

void response::GetThermalState::unpack(const std::vector<std::uint8_t>& data) {
    m_thermal_state = THERMAL_STATE(data[OFFSET_THERMAL_STATE]);
}
