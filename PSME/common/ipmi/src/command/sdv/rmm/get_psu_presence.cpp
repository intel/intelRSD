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
 * @file get_psu_presence.cpp
 *
 * @brief GetPsuPresence IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/get_psu_presence.hpp"
#include "ipmi/command/sdv/enums.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetPsuPresence::GetPsuPresence() : GetPresence(sdv::Cmd::GET_PSU_PRESENCE) {}


request::GetPsuPresence::~GetPsuPresence() {}


response::GetPsuPresence::GetPsuPresence() : GetPresence(sdv::Cmd::GET_PSU_PRESENCE) {}


response::GetPsuPresence::~GetPsuPresence() {}


bool response::GetPsuPresence::is_present(std::uint8_t psu_index) const {
    return (psu_index >= get_slot_count()) ? false : (m_presence_mask & (1 << psu_index));
}
