/*!
 * @brief Implementation of Get SDR Request/Response.
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
 * @file command/generic/get_sdr.cpp
 */

#include "ipmi/command/generic/get_sdr.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetSdr::GetSdr() :
        Request(generic::NetFn::STORAGE, generic::Cmd::GET_SDR) {}

request::GetSdr::~GetSdr() {}

void request::GetSdr::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(std::uint8_t(m_reservation_id & 0xff));
    data.push_back(std::uint8_t(m_reservation_id >> 8));
    data.push_back(std::uint8_t(m_record_id & 0xff));
    data.push_back(std::uint8_t(m_record_id >> 8));
    data.push_back(m_offset_into_record);
    data.push_back(m_bytes_to_read);
}

response::GetSdr::GetSdr() :
        Response(generic::NetFn::STORAGE + 1, generic::Cmd::GET_SDR, MIN_RESPONSE_SIZE) {}

response::GetSdr::~GetSdr() {}

void response::GetSdr::unpack(const IpmiInterface::ByteBuffer& data) {
    m_next_record_id = std::uint16_t(data[OFFSET_DATA] | data[OFFSET_DATA + 1] << 8);
    m_record_data = {data.cbegin() + OFFSET_RECORD_DATA, data.cend()};
}
