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
 * @file get_sel_time.cpp
 */

#include "ipmi/command/generic/sel/get_sel_time.hpp"

#include "ipmi/utils/byte_buffer.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetSelTime::GetSelTime(): Request(generic::NetFn::STORAGE, generic::Cmd::GET_SEL_TIME) {}

request::GetSelTime::~GetSelTime() {}

void request::GetSelTime::pack(IpmiInterface::ByteBuffer&) const {
    /* no data to be put into the request */
}

/*
 * Response structure:
 *
 * 0: Completion Code
 * 1-4: Present Timestamp clock reading. LS byte first. See Section 0, Timestamp Format.
 */
static constexpr std::size_t RESPONSE_SIZE = 5;
static constexpr unsigned TIMESTAMP_POS = 1;

response::GetSelTime::GetSelTime() : Response(generic::NetFn::APP, generic::Cmd::GET_SEL_TIME, RESPONSE_SIZE) {}

response::GetSelTime::~GetSelTime() {}

void response::GetSelTime::unpack(const IpmiInterface::ByteBuffer& data) {
    m_sel_time = ByteBuffer::get_uint32(data, TIMESTAMP_POS);
}
