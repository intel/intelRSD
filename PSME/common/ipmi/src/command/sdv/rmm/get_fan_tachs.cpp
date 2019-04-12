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
 * @file get_fan_tachs.cpp
 *
 * @brief GetFanTach IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_fan_tachs.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetFanTachs::GetFanTachs() : Request(NetFn::INTEL, Cmd::GET_FAN_TACHS) {}

request::GetFanTachs::~GetFanTachs() { }

void request::GetFanTachs::pack(IpmiInterface::ByteBuffer& /*data*/) const {}

response::GetFanTachs::GetFanTachs(): Response(NetFn::INTEL+1, Cmd::GET_FAN_TACHS, RESPONSE_SIZE) {}

void response::GetFanTachs::unpack(const IpmiInterface::ByteBuffer& data) {
    for (std::size_t i = 0; i < NUM_READINGS; ++i) {
        m_tach_readings[i] = std::uint16_t((data[i*2+1] << 8) | (data[i*2+2]));
    }
}

std::uint16_t response::GetFanTachs::get_tach_reading(std::uint8_t fan_index) const {
    return fan_index >= NUM_READINGS ? 0 : m_tach_readings[fan_index];
}

std::uint8_t response::GetFanTachs::get_slot_count() const {
    return static_cast<std::uint8_t>(NUM_READINGS) + 1;
}
