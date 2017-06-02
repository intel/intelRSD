/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file tlv_id_message_header.cpp
 *
 * @brief LLDP TLV ID message header
 *
 *  +------------------------------------+
 *  |       TLV ID MESSAGE HEADER        |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      |  LEN  | ASCII    |
 *  +------------------------------------+
 *  | TLV ID          | 8     | 0000007f |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/tlv_id_message_header.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;

TlvIdMessageHeader::TlvIdMessageHeader(TlvId tlv_id) : m_tlv_id{tlv_id} {
}

void TlvIdMessageHeader::parse_raw(const string& raw) {
    /* check the TLV ID size */
    if (get_raw_size() > raw.length()) {
        throw exception::ApiError("TLV ID raw message is too short");
    }

    /* parse TLV ID */
    uint32_t tlv_id{};
    std::stringstream parser{};
    parser.str(raw.substr(0, get_raw_size()));
    parser >> std::hex >> tlv_id;
    m_tlv_id = TlvId(tlv_id);
}

std::string TlvIdMessageHeader::get_raw() const {
    std::ostringstream raw_msg{};
    raw_msg << std::setfill('0') << std::hex;
    raw_msg << std::setw(8) << uint32_t(m_tlv_id);
    return raw_msg.str();
}

size_t TlvIdMessageHeader::get_raw_size() const {
    return (sizeof(m_tlv_id) * 2);
}
