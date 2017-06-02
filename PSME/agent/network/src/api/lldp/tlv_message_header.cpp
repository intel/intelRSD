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
 * @brief LLDP TLV message header
 *
 * The format of TLV header is described in
 * 802.1AB-2009 LLDP spec
 *
 * */

#include "api/lldp/tlv_message_header.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;
using namespace agent::network::api::lldp::exception;

namespace {
    constexpr uint32_t TLV_HEADER_RAW_SIZE = 4;
    constexpr uint32_t BYTE_RAW_SIZE = 2;

    void hex_string2byte_array(const string& hex_str,
            uint8_t* buff, size_t size) {
        /* check the buffer size */
        if ((size * BYTE_RAW_SIZE) > hex_str.length()) {
            throw ApiError("Cannot convert HEX string to byte array");
        }
        /* convert hex string to byte array */
        uint32_t byte{};
        std::stringstream parser{};
        for (auto hex_ptr = hex_str.c_str(); 0 < size; --size) {
            parser.str({hex_ptr, BYTE_RAW_SIZE});
            parser.seekg(0, std::ios_base::beg);
            parser >> std::hex >> byte;
            hex_ptr += BYTE_RAW_SIZE;
            *buff++ = uint8_t(byte);
        }
    }
}

void TlvMessageHeader::parse_raw(const string& raw) {
    uint16_t tlv_size{0};

    /* check TLV header length */
    if (TLV_HEADER_RAW_SIZE > raw.length()) {
        throw ApiError("TLV header raw message is too short");
    }

    /* parse TLV header */
    uint16_t tlv_header{};
    std::stringstream parser{};
    parser.str(raw.substr(0, TLV_HEADER_RAW_SIZE));
    parser >> std::hex >> tlv_header;

    /* get TLV id & size based on 802.1AB-2009 */
    m_tlv_id = TlvId(tlv_header >> 9);
    tlv_size = tlv_header & 0x1FF;

    /* check raw data size */
    if ((TLV_HEADER_RAW_SIZE + (tlv_size * BYTE_RAW_SIZE)) > raw.length()) {
        throw ApiError("TLV raw message is too short");
    }

    /* parse TLV payload */
    m_payload.resize(tlv_size);
    hex_string2byte_array(raw.c_str() + TLV_HEADER_RAW_SIZE,
                                    m_payload.get(), tlv_size);
}

std::string TlvMessageHeader::get_raw() const {
    /* implementation is required for request only */
    return {};
}

size_t TlvMessageHeader::get_raw_size() const {
    return TLV_HEADER_RAW_SIZE + (m_payload.get_size() * BYTE_RAW_SIZE);
}
