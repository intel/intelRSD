/*!
 * @brief Implementation of Delete SEL entry (event) Request/Response.
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
 * @file delete_sel_entry.cpp
 */

#include "ipmi/command/generic/sel/delete_sel_entry.hpp"

#include "ipmi/utils/byte_buffer.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

/*
 * Request structure:
 *
 * 0-1: Reservation ID, LS Byte first. The reservation ID should be set to 0000h
 *     for implementations that don’t implement the Reserve SEL command.
 * 2-3: SEL Record ID to delete, LS Byte first.
 *     0000h = FIRST ENTRY
 *     FFFFh = LAST ENTRY
 */
static constexpr unsigned RESERVATION_ID_POS = 0;
static constexpr unsigned ENTRY_ID_POS = 2;

request::DeleteSelEntry::DeleteSelEntry(ipmi::command::generic::SelEntryId entry, ipmi::command::generic::SelReservationId reservation):
    Request(generic::NetFn::STORAGE, generic::Cmd::DELETE_SEL_ENTRY),
    entry_id(entry), reservation_id(reservation) {}

request::DeleteSelEntry::~DeleteSelEntry() {}

void request::DeleteSelEntry::pack(IpmiInterface::ByteBuffer& data) const {
    ByteBuffer::put_uint16(data, RESERVATION_ID_POS, reservation_id);
    ByteBuffer::put_uint16(data, ENTRY_ID_POS, entry_id);
}

/*
 * Response structure:
 *
 * 0: Completion Code - Generic plus following command specific:
 *     80h = operation not supported for this Record Type
 *     81h = cannot execute command, SEL erase in progress
 * 1-2: Record ID for deleted record, LS Byte first.
 */
static constexpr std::size_t RESPONSE_SIZE = 3;
static constexpr unsigned RECORD_ID_POS = 3;

response::DeleteSelEntry::DeleteSelEntry() : Response(generic::NetFn::APP, generic::Cmd::GET_SEL_INFO, RESPONSE_SIZE) {}

response::DeleteSelEntry::~DeleteSelEntry() {}

void response::DeleteSelEntry::throw_error_on_completion_code(CompletionCode completion_code) const {
    if (completion_code == 0x80) {
        throw WrongEntityError(*this);
    }
    if (completion_code == 0x81) {
        throw SelEraseInProgressError(*this);
    }
    Response::throw_error_on_completion_code(completion_code);
}

void response::DeleteSelEntry::unpack(const IpmiInterface::ByteBuffer& data) {
    removed_entry = ByteBuffer::get_uint16(data, RECORD_ID_POS);
}
