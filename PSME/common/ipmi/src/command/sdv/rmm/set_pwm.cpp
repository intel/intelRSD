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
 * @file set_pwm.cpp
 *
 * @brief SetPwm IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/set_pwm.hpp"
#include "ipmi/command/sdv/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::SetPwm::SetPwm() : Request(NetFn::INTEL, Cmd::SET_PWM) {}


request::SetPwm::~SetPwm() {}


void request::SetPwm::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_asset_index);
    data.push_back(m_pwm_value);
}


void request::SetPwm::set_asset_index(std::uint8_t asset_index) {
    m_asset_index = asset_index;
}


void request::SetPwm::set_pwm_value(std::uint8_t pwm_value) {
    m_pwm_value = pwm_value;
}


response::SetPwm::SetPwm() : Response(NetFn::INTEL + 1, Cmd::SET_PWM, RESPONSE_SIZE) {}


response::SetPwm::~SetPwm() {}
