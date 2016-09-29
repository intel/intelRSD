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
 * @file response.hpp
 *
 * @brief LLDP response
 *
 *  +------------------------------------+
 *  |           RESPONSE HEADER          |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      | LEN   | ASCII    |
 *  +------------------------------------+
 *  | Message type    |   1   | R        |
 *  | Response status |   2   | 00       |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/response.hpp"
#include "api/lldp/message_header.hpp"
#include "api/lldp/command_message_header.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include <sstream>
#include <map>

using std::string;
using namespace agent::network::api::lldp;

Response::Response(const std::string& raw) {
    std::stringstream parser{};

    /* sanity check */
    if (OFFSET_RESPONSE > raw.length()) {
        throw exception::ApiError("Response raw message is too short");
    }

    /* check message type */
    if (char(CommandMessageHeader::CmdType::RESPONSE) !=
        raw[CommandMessageHeader::OFFSET_TYPE]) {
        throw exception::ApiError("Invalid response type");
    }

    /* parse message version */
    parser.str(raw.substr(OFFSET_STATS, OFFSET_STATS_LEN));
    parser >> std::hex >> m_status;

    /* store the raw message of next header */
    m_raw_message = (raw.c_str() + OFFSET_RESPONSE);
}

Response::operator bool() const {
    return m_status == 0;
}

void Response::pop_header(MessageHeader& header) {
    /* check end of raw message */
    if (!m_raw_message.length()) {
        throw exception::ApiError(exception::ErrCode::EOM, "End of message");
    }

    /* parse raw message */
    header.parse_raw(m_raw_message);

    /* remove raw message */
    m_raw_message.erase(0, header.get_raw_size());
}
