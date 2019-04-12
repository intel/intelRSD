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
 * @file get_chassis_status.cpp
 *
 * @brief IPMI Get Chassis Status Command.
 * */

#include "ipmi/command/generic/get_chassis_status.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetChassisStatus::GetChassisStatus():
    Request(generic::NetFn::CHASSIS, generic::Cmd::GET_CHASSIS_STATUS) {}

void request::GetChassisStatus::pack(std::vector<std::uint8_t>& data) const {
    (void)data;
}

response::GetChassisStatus::GetChassisStatus():
    Response(generic::NetFn::CHASSIS, generic::Cmd::GET_CHASSIS_STATUS, RESPONSE_SIZE) {}

void response::GetChassisStatus::unpack(const std::vector<std::uint8_t>& data) {
    m_power_on = MASK_SYSTEM_POWER & data[OFFSET_POWER_STATUS];
}

