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
 * @file get_fan_pwm.cpp
 * */
#include "ipmi/command/sdv/get_fan_pwm.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetFanPwm::GetFanPwm() : Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_FAN_PWM) {}
request::GetFanPwm::~GetFanPwm() {}

void request::GetFanPwm::pack(std::vector<std::uint8_t>&) const {}

response::GetFanPwm::GetFanPwm() : Response(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_FAN_PWM, RESPONSE_SIZE) {}
response::GetFanPwm::~GetFanPwm() {}

void response::GetFanPwm::unpack(const std::vector<std::uint8_t>& data) {
    m_maximum_pwm = data[OFFSET_MAXIMUM_PWM];
}
