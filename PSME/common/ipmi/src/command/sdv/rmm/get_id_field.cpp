/*!
 * @brief GetIdFields IPMI command request and response.
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
 * @file ipmi/command/sdv/rmm/get_id_field.cpp
 */

#include "ipmi/command/sdv/rmm/get_id_field.hpp"

#include <algorithm>



using namespace ipmi;
using namespace ipmi::command::sdv;


request::GetIdField::GetIdField() : ipmi::Request(NetFn::INTEL, Cmd::GET_ID_FIELD) {}


request::GetIdField::~GetIdField() {}


void request::GetIdField::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(static_cast<std::uint8_t>(m_id_field_type));
}


response::GetIdField::GetIdField() : ipmi::Response(NetFn::INTEL + 1, Cmd::GET_ID_FIELD, MINIMUM_RESPONSE_SIZE) {}


response::GetIdField::~GetIdField() {}


void response::GetIdField::unpack(const IpmiInterface::ByteBuffer& byte_buffer) {
    m_maximum_data_size = byte_buffer[MAXIMUM_DATA_SIZE_OFFSET];
    m_received_data_size = byte_buffer[RECEIVED_DATA_SIZE_OFFSET];
    m_received_data.resize(static_cast<size_t>(m_received_data_size));
    std::copy(byte_buffer.begin() + RECEIVED_DATA_OFFSET, byte_buffer.end(), m_received_data.begin());
}
