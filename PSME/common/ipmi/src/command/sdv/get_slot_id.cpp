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
 * @file get_slot_id.cpp
 *
 * @brief ...
 * */

#include "ipmi/command/sdv/get_slot_id.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetSlotId::GetSlotId() : Request(sdv::NetFn::RACKSCALE, sdv::Cmd::GET_SLOT_ID) { }

request::GetSlotId::~GetSlotId() { }

response::GetSlotId::GetSlotId() : Response(sdv::NetFn::RACKSCALE + 1, sdv::Cmd::GET_SLOT_ID, RESPONSE_SIZE) { }

response::GetSlotId::~GetSlotId() { }

void response::GetSlotId::unpack(const std::vector<std::uint8_t>& data) {
    m_slot_id = data[OFFSET_SLOT_ID];
    m_board_id = data[OFFSET_BOARD_ID];
    m_revision_id = data[OFFSET_REVISION_ID];
    m_reserved_id = data[OFFSET_RESERVED_ID];
}

std::uint8_t response::GetSlotId::get_slot_id() const {
    return m_slot_id;
}
