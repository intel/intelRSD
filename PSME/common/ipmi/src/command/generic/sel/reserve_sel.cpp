/*!
 * @brief Implementation of Get SEL reservation ("lock") Request/Response.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file reserve_sel.cpp
 */

#include "ipmi/command/generic/sel/reserve_sel.hpp"

#include "ipmi/utils/byte_buffer.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

/*
 * Empty request, does not have any fields
 */

request::ReserveSel::ReserveSel() : Request(generic::NetFn::STORAGE, generic::Cmd::RESERVE_SEL) {}

request::ReserveSel::~ReserveSel() {}

void request::ReserveSel::pack(IpmiInterface::ByteBuffer&) const {
}

/*
 * Response structure:
 *
 * 0: Completion Code
 *     81h = cannot execute command, SEL erase in progress
 * 1-2: Reservation ID, LS Byte first, 0000h reserved.
 */
static constexpr std::size_t RESPONSE_SIZE = 3;
static constexpr unsigned RESERVATION_ID_POS = 1;


response::ReserveSel::ReserveSel() : Response(generic::NetFn::STORAGE, generic::Cmd::RESERVE_SEL, RESPONSE_SIZE) {}

response::ReserveSel::~ReserveSel() {}

void response::ReserveSel::throw_error_on_completion_code(CompletionCode completion_code) const {
    if (completion_code == 0x81) {
        throw SelEraseInProgressError(*this);
    }
    Response::throw_error_on_completion_code(completion_code);
}

void response::ReserveSel::unpack(const IpmiInterface::ByteBuffer& data) {
    reservation_id = ByteBuffer::get_uint16(data, RESERVATION_ID_POS);
}
