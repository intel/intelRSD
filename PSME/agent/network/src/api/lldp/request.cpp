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
 * @file lldp_request.cpp
 *
 * @brief LLDP request
 *
 *  +------------------------------------+
 *  |             REQUEST HEADER         |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      |  LEN  | ASCII    |
 *  +------------------------------------+
 *  | Message Type    | 1     | M        |
 *  | Module ID       | 8     | 00000001 |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/request.hpp"
#include "api/lldp/message_header.hpp"
#include "api/lldp/command_message_header.hpp"

#include <iomanip>
#include <sstream>

using std::string;
using namespace agent::network::api::lldp;

namespace {
    constexpr uint32_t MODULE_ID = 1;
}

void Request::push(const MessageHeader& header) {
    m_headers.emplace_back(header.get_raw());
}

std::string Request::get_message() const {
    /* format request message */
    std::ostringstream raw_msg{};
    raw_msg << std::setfill('0');
    char cmd{};
    cmd = char(CommandMessageHeader::CmdType::MODIFY);
    raw_msg << cmd << std::hex << std::setw(8) << MODULE_ID;
    for (const auto& data : m_headers) {
        raw_msg << data;
    }
    return raw_msg.str();
}
