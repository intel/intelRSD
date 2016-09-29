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
 * @file arg_message_header.hpp
 *
 * @brief LLDP argument message header
 *
 *  +------------------------------------+
 *  |           ARG MESSAGE HEADER       |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      |  LEN  | ASCII    |
 *  +------------------------------------+
 *  | Arg Name Len    | 2     | 05       |
 *  | Arg Name        | X     | XXXXX    |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/exceptions/api_error.hpp"
#include "api/lldp/arg_message_header.hpp"

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;
using namespace agent::network::api::lldp::exception;

namespace {
    constexpr uint32_t ARG_MIN_HEADER_RAW_SIZE = 2;
    constexpr uint32_t BYTE_RAW_SIZE = 2;
}

ArgMessageHeader::ArgMessageHeader(const string& arg) : m_arg{arg} {
}

void ArgMessageHeader::parse_raw(const string&) {
    /* implementation is required for response parse only */
}

string ArgMessageHeader::get_raw() const {
    std::ostringstream raw_msg{};
    raw_msg << std::setfill('0') << std::hex;
    auto name_len = size_t(0xFF & m_arg.length());
    raw_msg << std::setw(2) << name_len << m_arg.substr(0, name_len);
    return raw_msg.str();
}

size_t ArgMessageHeader::get_raw_size() const {
    return ((m_arg.length() * BYTE_RAW_SIZE)
            + ARG_MIN_HEADER_RAW_SIZE);
}
