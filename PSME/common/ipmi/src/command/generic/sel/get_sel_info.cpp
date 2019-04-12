/*!
 * @brief Implementation of Get SEL Info Request/Response.
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
 * @file get_sel_info.cpp
 */

#include "ipmi/command/generic/sel/get_sel_info.hpp"

#include "ipmi/utils/byte_buffer.hpp"
#include "ipmi/command/generic/enums.hpp"
#include "ipmi/response_error.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

/*
 * Empty request, does not have any fields
 */

request::GetSelInfo::GetSelInfo() : Request(generic::NetFn::STORAGE, generic::Cmd::GET_SEL_INFO) {}

request::GetSelInfo::~GetSelInfo() {}

void request::GetSelInfo::pack(IpmiInterface::ByteBuffer&) const {
}

/*
 * Response structure:
 *
 * 0. Completion Code
 *     81h = cannot execute command, SEL erase in progress
 * 1. SEL Version - version number of the SEL command set for this SEL Device.
 *     51h for this specification.
 *     (BCD encoded).BCD encoded with bits 7:4 holding the Least Significant
 *     digit of the revision and bits 3:0 holding the Most Significant bits.
 * 2-3. Entries count - number of log entries in SEL, LS Byte first
 * 4-5. Free Space in bytes, LS Byte first. FFFFh indicates 65535 or more bytes of
 *     free space are available.
 * 6-9. Most recent addition timestamp. LS byte first.
 *     Returns FFFF_FFFFh if no SEL entries have ever been made or if a
 *     component update or error caused the retained value to be lost.
 * 10-13. Most recent erase timestamp. Last time that one or more entries were
 *     deleted from the log. LS byte first.
 * 14. Operation Support
 *     [7] - Overflow Flag. 1=Events have been dropped due to lack of space in
 *         the SEL.
 *     [6:4] - reserved. Write as 000
 *     [3] - 1b = Delete SEL command supported
 *     [2] - 1b = Partial Add SEL Entry command supported
 *     [1] - 1b = Reserve SEL command supported
 *     [0] - 1b = Get SEL Allocation Information command supported
 */
static constexpr std::size_t RESPONSE_SIZE = 15;
static constexpr unsigned SEL_VERSION_POS = 1;
static constexpr unsigned ENTRIES_COUNT_POS = 2;
static constexpr unsigned FREE_SPACE_POS = 4;
static constexpr unsigned LAST_ADD_TIMESTAMP_POS = 6;
static constexpr unsigned LAST_ERASE_TIMESTAMP_POS = 10;
static constexpr unsigned OPERATION_FLAGS_POS = 14;
static constexpr unsigned RESERVE_SEL_SUPPORTED_BIT = 1;
static constexpr unsigned DELETE_SEL_SUPPORTED_BIT = 3;
static constexpr unsigned IS_OVERFLOWED_BIT = 7;

static constexpr std::uint8_t IPMI_2_0_SEL_VERSION = 0x51;

response::GetSelInfo::GetSelInfo() : Response(generic::NetFn::STORAGE, generic::Cmd::GET_SEL_INFO, RESPONSE_SIZE) {}

response::GetSelInfo::~GetSelInfo() {}

void response::GetSelInfo::throw_error_on_completion_code(CompletionCode completion_code) const {
    if (completion_code == 0x81) {
        throw SelEraseInProgressError(*this);
    }
    Response::throw_error_on_completion_code(completion_code);
}

void response::GetSelInfo::unpack(const IpmiInterface::ByteBuffer& data) {
    if (data[SEL_VERSION_POS] != IPMI_2_0_SEL_VERSION) {
        throw IncorrectResponseVersionError(*this);
    }
    entries_count = ByteBuffer::get_uint16(data, ENTRIES_COUNT_POS);
    free_space = ByteBuffer::get_uint16(data, FREE_SPACE_POS);
    last_add_timestamp = ByteBuffer::get_uint32(data, LAST_ADD_TIMESTAMP_POS);
    last_erase_timestamp = ByteBuffer::get_uint32(data, LAST_ERASE_TIMESTAMP_POS);
    reserve_supported = ByteBuffer::is_bit_set(data, OPERATION_FLAGS_POS, RESERVE_SEL_SUPPORTED_BIT);
    delete_supported = ByteBuffer::is_bit_set(data, OPERATION_FLAGS_POS, DELETE_SEL_SUPPORTED_BIT);
    overflow = ByteBuffer::is_bit_set(data, OPERATION_FLAGS_POS, IS_OVERFLOWED_BIT);
}
