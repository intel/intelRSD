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
 * @file command_message_header.cpp
 *
 * @brief LLDP command message header
 *
 *  +------------------------------------+
 *  |        COMMAND MESSAGE HEADER      |
 *  +-----------------+-------+----------+
 *  | FIELD NAME      |  LEN  | ASCII    |
 *  +------------------------------------+
 *  | Command Type    | 1     | C        |
 *  | Message Version | 1     | 3        |
 *  | Command Code    | 2     | 01       |
 *  | Command Option  | 8     | 00000002 |
 *  | IFace Name Size | 2     | 05       |
 *  | IFace Name      | X     | sw0p1    |
 *  +-----------------+-------+----------+
 *
 * */

#include "api/lldp/command_message_header.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include "api/lldp/lldpad.hpp"

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;
using namespace agent::network::api::lldp::exception;

CommandMessageHeader::CommandMessageHeader() :
     m_ifname{"Unknown"}, m_version{CLIF_RSP_VERSION} {
}

CommandMessageHeader::CommandMessageHeader(const string& ifname,
                        CmdType type, CmdCode code, uint32_t option) :
        m_ifname{ifname}, m_type{type}, m_code{code}, m_option{option},
        m_version{CLIF_RSP_VERSION} {
}

void CommandMessageHeader::parse_raw(const string& raw) {
    std::stringstream parser{};

    /* check header length */
    if (CMD_IF > raw.length()) {
        throw ApiError("Command header raw message is too short");
    }

    /* parse command type */
    char type{};
    parser.str(raw.substr(MSG_TYPE, MSG_VER - MSG_TYPE));
    parser >> type;
    switch (type) {
    case CMD_RESPONSE : m_type = CmdType::RESPONSE; break;
    case CMD_REQUEST : m_type = CmdType::REQUEST; break;
    default :
        m_type = CmdType::UNKNOWN;
        throw ApiError("Unknown commmand type [type=" + parser.str() + "]");
    };

    /* parse mesasge version */
    parser.str(raw.substr(MSG_VER, CMD_CODE - MSG_VER));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> m_version;

    /* check message version */
    if (CLIF_RSP_VERSION != m_version) {
        throw ApiError("Unknown response ver [ver=" + parser.str() + "]");
    }

    /* parse command code */
    uint16_t code{};
    parser.str(raw.substr(CMD_CODE, CMD_OPS - CMD_CODE));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> code;
    m_code = CmdCode(code);

    /* parse command option */
    parser.str(raw.substr(CMD_OPS, CMD_IF_LEN - CMD_OPS));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> m_option;

    /* parse interface name len */
    uint32_t ifname_len{0};
    parser.str(raw.substr(CMD_IF_LEN, CMD_IF - CMD_IF_LEN));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> ifname_len;

    /* check raw message len */
    if ((CMD_IF + ifname_len) > raw.length()) {
        throw ApiError("Raw message is too short to parse interface name");
    }

    /* parse interface name len */
    m_ifname = raw.substr(CMD_IF, ifname_len);
}

size_t CommandMessageHeader::get_raw_size() const {
    return CMD_IF + m_ifname.length();
}

string CommandMessageHeader::get_raw() const {

    /* convert command type */
    char cmd_type{};
    switch (m_type) {
    case CmdType::RESPONSE : cmd_type = CMD_RESPONSE; break;
    case CmdType::REQUEST : cmd_type = CMD_REQUEST; break;
    case CmdType::UNKNOWN :
    default :
        throw ApiError("Unknown commmand type");
    };

    /* format the raw message */
    std::ostringstream raw_msg{};
    raw_msg << std::setfill('0');
    raw_msg << cmd_type << std::dec << m_version;
    raw_msg << std::hex << std::setw(2) << uint16_t(m_code);
    raw_msg << std::hex << std::setw(8) << m_option;
    raw_msg << std::hex << std::setw(2) << m_ifname.length();
    raw_msg << m_ifname;
    return raw_msg.str();
}
