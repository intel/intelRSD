/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 *
 * @file get_chassis_alerts.cpp
 *
 * @brief GetChassisAlerts IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_chassis_alert.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetChassisAlert::GetChassisAlert() : Request(NetFn::INTEL, Cmd::GET_CHASSIS_ALERT) {}

request::GetChassisAlert::~GetChassisAlert() { }

void request::GetChassisAlert::pack(std::vector<std::uint8_t>& /*data*/) const {}

response::GetChassisAlert::GetChassisAlert(): Response(NetFn::INTEL+1, Cmd::GET_CHASSIS_ALERT, RESPONSE_SIZE) {}

void response::GetChassisAlert::unpack(const std::vector<std::uint8_t>& data) {
    m_alerts_mask = data[1];
}

std::uint8_t response::GetChassisAlert::get_alerts_mask() const {
    return m_alerts_mask;
}

bool response::GetChassisAlert::is_alerted(std::uint8_t chassis_index) const {
    return (chassis_index >= get_slot_count()) ? false : (m_alerts_mask & (1 << chassis_index));
}
