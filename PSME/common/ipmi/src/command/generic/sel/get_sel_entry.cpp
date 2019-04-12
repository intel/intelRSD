/*!
 * @brief Implementation of Get SEL entry (event) Request/Response.
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
 * @file get_sel_entry.cpp
 */

#include "ipmi/command/generic/sel/get_sel_entry.hpp"

#include "ipmi/command/generic/enums.hpp"
#include "ipmi/utils/byte_buffer.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

/*
 * Request structure:
 *
 * 0-1: Reservation ID, LS Byte first. Only required for partial Get. Use 0000h otherwise.
 *      The reservation ID should be set to 0000h for implementations that don’t implement
 *      the Reserve SEL command.
 * 2-3: SEL Record ID, LS Byte first.
 *      0000h = GET FIRST ENTRY
 *      FFFFh = GET LAST ENTRY
 * 4: Offset into record
 * 5: Bytes to read. FFh means read entire record.
 */
static constexpr unsigned RESERVATION_ID_POS = 0;
static constexpr unsigned ENTRY_ID_POS = 2;
static constexpr unsigned OFFSET_POS = 4;
static constexpr unsigned BYTES_TO_READ_POS = 5;

request::GetSelEntry::GetSelEntry(SelEntryId entry, SelReservationId reservation, std::uint8_t offset, std::uint8_t amount):
    Request(generic::NetFn::STORAGE, generic::Cmd::GET_SEL_ENTRY),
    entry_id(entry), reservation_id(reservation), record_offset(offset), to_read(amount) {}

request::GetSelEntry::~GetSelEntry() {}

void request::GetSelEntry::pack(IpmiInterface::ByteBuffer& data) const {
    ByteBuffer::put_uint16(data, RESERVATION_ID_POS, reservation_id);
    ByteBuffer::put_uint16(data, ENTRY_ID_POS, entry_id);
    ByteBuffer::put_uint8(data, OFFSET_POS, record_offset);
    ByteBuffer::put_uint8(data, BYTES_TO_READ_POS, to_read);
}

/*
 * Response structure:
 *
 * 0: Completion Code
 *    Return an error completion code if the SEL is empty.
 *    81h = cannot execute command, SEL erase in progress.
 * 1-2: Next SEL Record ID, LS Byte first (return FFFFh if the record just
 *      returned is the last record.)
 *      Note: FFFFh is not allowed as the record ID for an actual record.
 *      i.e. the Record ID in the Record Data for the last record should not be FFFFh.
 * 3-N: Record Data, 16 bytes for entire record
 */
static constexpr std::size_t RESPONSE_SIZE = 2;
static constexpr unsigned NEXT_ENTRY_ID_POS = 1;
static constexpr unsigned ENTRY_POS = 3;

response::GetSelEntry::GetSelEntry() : Response(generic::NetFn::STORAGE, generic::Cmd::GET_SEL_ENTRY, RESPONSE_SIZE) {}

response::GetSelEntry::~GetSelEntry() {}

void response::GetSelEntry::throw_error_on_completion_code(CompletionCode completion_code) const {
    if (completion_code == 0x81) {
        throw SelEraseInProgressError(*this);
    }
    Response::throw_error_on_completion_code(completion_code);
}

void response::GetSelEntry::unpack(const IpmiInterface::ByteBuffer& data) {
    next_entry_id = ByteBuffer::get_uint16(data, NEXT_ENTRY_ID_POS);
    entry = IpmiInterface::ByteBuffer(data.begin() + ENTRY_POS, data.end());
}

bool response::GetSelEntry::is_last_record() const {
    return next_entry_id == 0xffff;
}
