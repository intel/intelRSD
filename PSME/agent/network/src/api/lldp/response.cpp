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
#include "api/lldp/exceptions/api_error.hpp"

#include "api/lldp/lldpad.hpp"

#include <sstream>
#include <map>

using std::string;
using namespace agent::network::api::lldp;

namespace {
    static std::map<uint32_t, Response::Status> g_status_enum_map{
        {cmd_success, Response::Status::SUCCESS},
        {cmd_agent_not_found, Response::Status::NO_AGENT}
    };
    constexpr uint32_t UKNOWN_STATUS = cmd_max_status;
    static std::map<uint32_t, string> g_status_msg_map{
        {cmd_success, "Successful"},
        {cmd_failed, "Failed"},
        {cmd_device_not_found, "Device not found or inactive"},
        {cmd_agent_not_found, "Agent instance for device not found"},
        {cmd_invalid, "Invalid command"},
        {cmd_bad_params, "Invalid parameters"},
        {cmd_peer_not_present, "Peer feature not present"},
        {cmd_ctrl_vers_not_compatible, "Version not compatible"},
        {cmd_not_capable, "Device not capable"},
        {cmd_not_applicable, "Command not applicable"},
        {cmd_no_access, "Access denied"},
        {cmd_agent_not_supported, "TLV does not support agent type"},
        {UKNOWN_STATUS, "Unknown status"}
    };
}

Response::Response(const std::string& raw) {
    std::stringstream parser{};

    /* sanity check */
    if (CLIF_RSP_OFF > raw.length()) {
        throw exception::ApiError("Response raw message is too short");
    }

    /* check message type */
    if (CMD_RESPONSE != raw[MSG_TYPE]) {
        throw exception::ApiError("Invalid response type");
    }

    /* parse mesasge version */
    parser.str(raw.substr(CLIF_STAT_OFF, CLIF_STAT_LEN));
    parser >> std::hex >> m_status;

    /* store the raw message of next header */
    m_raw_message = (raw.c_str() + CLIF_RSP_OFF);
}

Response::operator bool() const {
    return m_status == cmd_success;
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

const string& Response::get_status_message() const {
    auto error_it = ::g_status_msg_map.find(m_status);
    if (g_status_msg_map.end() != error_it) {
        return error_it->second;
    }
    return g_status_msg_map[UKNOWN_STATUS];
}

Response::Status Response::get_status() const {
    auto error_it = ::g_status_enum_map.find(m_status);
    if (g_status_enum_map.end() != error_it) {
        return error_it->second;
    }
    return Status::FAILURE;
}
