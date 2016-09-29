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

#include <sstream>
#include <iomanip>

using std::string;
using namespace agent::network::api::lldp;
using namespace agent::network::api::lldp::exception;

CommandMessageHeader::CommandMessageHeader() :
     m_ifname{"Unknown"}, m_version{VERSION} {
}

CommandMessageHeader::CommandMessageHeader(const string& ifname,
                        CmdType type, CmdCode code, uint32_t option) :
        m_ifname{ifname}, m_type{type}, m_code{code}, m_option{option},
        m_version{VERSION} {
}

void CommandMessageHeader::parse_raw(const string& raw) {
    std::stringstream parser{};

    /* check header length */
    if (int(OFFSET_IFNAME) > raw.length()) {
        throw ApiError("Command header raw message is too short");
    }

    /* parse command type */
    char type{};
    parser.str(raw.substr(OFFSET_TYPE, OFFSET_VERSION - OFFSET_TYPE));
    parser >> type;
    m_type = CmdType(type);
    switch (m_type) {
        case CmdType::RESPONSE:
        case CmdType::REQUEST:
        case CmdType::MODIFY:
            break;
        case CmdType::UNKNOWN:
        default:
            throw ApiError("Unknown commmand type [type=" + parser.str() + "]");
    };

    /* parse mesasge version */
    parser.str(raw.substr(OFFSET_VERSION, OFFSET_CODE - OFFSET_VERSION));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> m_version;

    /* check message version */
    if (VERSION != m_version) {
        throw ApiError("Unknown response ver [ver=" + parser.str() + "]");
    }

    /* parse command code */
    uint16_t code{};
    parser.str(raw.substr(OFFSET_CODE, OFFSET_OPTIONS - OFFSET_CODE));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> code;
    m_code = CmdCode(code);

    /* parse command option */
    parser.str(raw.substr(OFFSET_OPTIONS,
                          OFFSET_IFNAME_LEN - OFFSET_OPTIONS));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> m_option;

    /* parse interface name len */
    uint32_t ifname_len{0};
    parser.str(raw.substr(OFFSET_IFNAME_LEN, OFFSET_IFNAME
                          - OFFSET_IFNAME_LEN));
    parser.seekg(0, std::ios_base::beg);
    parser >> std::hex >> ifname_len;

    /* check raw message len */
    if ((OFFSET_IFNAME + ifname_len) > raw.length()) {
        throw ApiError("Raw message is too short to parse interface name");
    }

    /* parse interface name len */
    m_ifname = raw.substr(OFFSET_IFNAME, ifname_len);
}

size_t CommandMessageHeader::get_raw_size() const {
    return OFFSET_IFNAME + m_ifname.length();
}

string CommandMessageHeader::get_raw() const {

    /* check command type */
    switch (m_type) {
        case CmdType::RESPONSE:
        case CmdType::REQUEST:
        case CmdType::MODIFY:
            break;
        case CmdType::UNKNOWN :
        default :
            throw ApiError("Unknown commmand type");
    };

    /* format the raw message */
    std::ostringstream raw_msg{};
    char type{};
    type = char(m_type);
    raw_msg << std::setfill('0');
    raw_msg << type << std::dec << m_version;
    raw_msg << std::hex << std::setw(2) << uint16_t(m_code);
    raw_msg << std::hex << std::setw(8) << m_option;
    raw_msg << std::hex << std::setw(2) << m_ifname.length();
    raw_msg << m_ifname;
    return raw_msg.str();
}
