/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file arg_value_message_header.hpp
 *
 * @brief LLDP argument value message header
 *
 *  +------------------------------------+
 *  |      ARG VALUE MESSAGE HEADER      |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      |  LEN  | ASCII    |
 *  +------------------------------------+
 *  | Arg Name Len    | 2     | 05       |
 *  | Arg Name        | X     | XXXXX    |
 *  | Arg Value Len   | 4     | 0007     |
 *  | Arg Value       | X     | VVVVVVV  |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/exceptions/api_error.hpp"
#include "api/lldp/arg_value_message_header.hpp"

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;
using namespace agent::network::api::lldp::exception;

namespace {
    constexpr uint32_t ARG_VALUE_MIN_HEADER_RAW_SIZE = 6;
    constexpr uint32_t ARG_LEN_RAW_SIZE = 2;
    constexpr uint32_t ARG_VALUE_LEN_RAW_SIZE = 4;
    constexpr uint32_t BYTE_RAW_SIZE = 2;
}

ArgValueMessageHeader::ArgValueMessageHeader(
        const string& arg, const string& value) : m_arg{arg}, m_value{value} {
}

void ArgValueMessageHeader::parse_raw(const string& raw) {
    std::stringstream parser{};

    /* check header length */
    if (ARG_VALUE_MIN_HEADER_RAW_SIZE > raw.length()) {
        throw ApiError("ArgValue header raw message is too short");
    }

    /* parse arg len len */
    uint32_t arg_len{0};
    parser.str(raw.substr(0, ARG_LEN_RAW_SIZE));
    parser >> std::hex >> arg_len;

    /* check raw message len */
    if ((arg_len + ARG_LEN_RAW_SIZE) > raw.length()) {
        throw ApiError("Raw message is too short to parse arg name");
    }

    /* store argument name */
    m_arg = raw.substr(ARG_LEN_RAW_SIZE, arg_len);

    /* parse arg value len */
    uint32_t arg_value_len{0};
    uint32_t arg_value_offset = arg_len + ARG_LEN_RAW_SIZE
                                                + ARG_VALUE_LEN_RAW_SIZE;
    parser.str(raw.substr(arg_len + ARG_LEN_RAW_SIZE, ARG_VALUE_LEN_RAW_SIZE));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> arg_value_len;

    /* check raw message len */
    if ((arg_value_len + arg_value_offset) > raw.length()) {
        throw ApiError("Raw message is too short to parse arg value");
    }

    /* store argument value */
    m_value = raw.substr(arg_value_offset, arg_value_len);
}

string ArgValueMessageHeader::get_raw() const {
    std::ostringstream raw_msg{};
    raw_msg << std::setfill('0') << std::hex;
    auto name_len = size_t(0xFF & m_arg.length());
    auto value_len = size_t(0xFFFF & m_value.length());
    raw_msg << std::setw(2) << name_len << m_arg.substr(0, name_len);
    raw_msg << std::setw(4) << value_len << m_value.substr(0, value_len);
    return raw_msg.str();
}

size_t ArgValueMessageHeader::get_raw_size() const {
    return ((m_arg.length() * m_value.length() * BYTE_RAW_SIZE)
            + ARG_VALUE_MIN_HEADER_RAW_SIZE);
}
