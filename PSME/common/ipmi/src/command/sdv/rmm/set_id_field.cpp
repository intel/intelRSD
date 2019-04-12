/*!
 * @brief SetIdField IPMI command request and response.
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
 * @file ipmi/command/sdv/rmm/set_id_field.cpp
 */

#include "ipmi/command/sdv/rmm/set_id_field.hpp"



using namespace ipmi;
using namespace ipmi::command::sdv;


request::SetIdField::SetIdField() : ipmi::Request(NetFn::INTEL, Cmd::SET_ID_FIELD) {}


request::SetIdField::~SetIdField() {}


void request::SetIdField::pack(IpmiInterface::ByteBuffer& data) const {
    constexpr std::uint8_t RESERVED_FIRST_BYTE = 0x00;
    constexpr std::uint8_t NUMERIC_DATA_LENGTH = 4;
    data.push_back(RESERVED_FIRST_BYTE);
    data.push_back(static_cast<std::uint8_t>(m_id_field_type));

    if (m_is_data_numeric) {
        data.push_back(NUMERIC_DATA_LENGTH);
        /* Push whole 4 byte word in LSB-first order */
        data.push_back(static_cast<std::uint8_t>(m_numeric_data >> 24));
        data.push_back(static_cast<std::uint8_t>(m_numeric_data >> 16));
        data.push_back(static_cast<std::uint8_t>(m_numeric_data >> 8));
        data.push_back(static_cast<std::uint8_t>(m_numeric_data));
    }
    else {
        data.push_back(static_cast<std::uint8_t>(m_string_data.size()));
        for (const auto& elem : m_string_data) {
            data.push_back(static_cast<std::uint8_t>(elem));
        }
    }
}


response::SetIdField::SetIdField() : ipmi::Response(NetFn::INTEL + 1, Cmd::SET_ID_FIELD, RESPONSE_SIZE) {}


response::SetIdField::~SetIdField() {}


void response::SetIdField::unpack(const IpmiInterface::ByteBuffer& byte_buffer) {
    m_maximum_data_size = byte_buffer[MAXIMUM_DATA_SIZE_OFFSET];
}
