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
 * @file GetDrawerPower.cpp
 *
 * @brief GetDrawerPower IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_drawer_power.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetDrawerPower::GetDrawerPower() : Request(NetFn::INTEL, Cmd::GET_DRAWER_POWER) {}

void request::GetDrawerPower::pack(IpmiInterface::ByteBuffer&) const {
}

response::GetDrawerPower::GetDrawerPower(): Response(NetFn::INTEL + 1, Cmd::GET_DRAWER_POWER, MIN_RESPONSE_SIZE) {}

void response::GetDrawerPower::unpack(const IpmiInterface::ByteBuffer& data) {

    m_slots = uint8_t((data.size() - DATA_OFFSET) / ENTRY_SIZE);
    for (uint8_t slot_id = 0; slot_id < m_slots; ++slot_id) {
        unsigned value = data[DATA_OFFSET + slot_id * ENTRY_SIZE + POWER_LSB_OFFSET];
        value += (data[DATA_OFFSET + slot_id * ENTRY_SIZE + POWER_MSB_OFFSET] << 8);
        m_power_on_slot[slot_id] = uint16_t(value);
    }
}
