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
 * @file client.cpp
 *
 * @brief LLDP client
 *
 * */

#include "api/lldp/client.hpp"
#include "api/lldp/interface.hpp"
#include "api/lldp/arg_message_header.hpp"
#include "api/lldp/arg_value_message_header.hpp"
#include "api/lldp/tlv_message_header.hpp"
#include "api/lldp/tlv_id_message_header.hpp"
#include "api/lldp/br_score_message_header.hpp"
#include "api/lldp/command_message_header.hpp"
#include "api/lldp/exceptions/api_error.hpp"

#include <map>
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <dirent.h>
#include <signal.h>


using std::string;
using namespace agent::network::api::lldp;

namespace {
    static const constexpr char* ENABLE_TLV_ARG = "enableTx";
    static const constexpr char* ENABLE_TLV_VALUE = "yes";
    static const constexpr char* DISABLE_TLV_VALUE = "no";

    static const constexpr char* ADMIN_STATUS_ARG = "adminStatus";
    static const constexpr char* ADMIN_STATUS_DISABLED = "disabled";
    static const constexpr char* ADMIN_STATUS_RXTX = "rxtx";
    static const constexpr char* ADMIN_STATUS_RX= "rx";
    static const constexpr char* ADMIN_STATUS_TX= "tx";

    static inline Response send_request(const Request& request) {
        Interface conn{};

        /* send LLDP request to the LLDP daemon */
        auto resp = conn.send(request);
        if (!resp) {
            throw exception::ApiError("Request message send failed ("
                + std::to_string(resp.get_status()) + ")");
        }
        return resp;
    }

    string get_file_content(const string& path) {
        std::ifstream ifile(path.c_str());
        string ret{};
        if (ifile) {
            std::stringstream ss{};
            ss << ifile.rdbuf();
            ret = ss.str();
            auto pos = ret.find('\n');
            if (pos != string::npos) {
                ret.erase(pos);
            }
        }
        return ret;
    }

    pid_t get_agent_pid() {
        pid_t pid = 0;
        DIR *dir = opendir("/proc");
        if (dir) {
            dirent *ent = nullptr;
            while ((ent = readdir(dir))) {
                string path = string("/proc/") + ent->d_name + "/comm";
                string procname = get_file_content(path);
                if (procname == "lldpad") {
                    pid = atoi(ent->d_name);
                    break;
                }
            }
            closedir(dir);
        }
        return pid;
    }
}

Client::Client(const string& ifname) : m_ifname{ifname} {
}

TlvPayload Client::get_neighbor_tlv(TlvId tlv_id) const {
    Interface conn{};
    Request req{};
    static constexpr int NO_AGENT = 3;

    /* push neighbor get-tlv header into the request */
    uint32_t opts = uint32_t(CommandMessageHeader::CmdOption::NEIGHBOR);
    req.push(CommandMessageHeader{m_ifname,
        CommandMessageHeader::CmdType::REQUEST,
        CommandMessageHeader::CmdCode::GET_TLV,
        opts});

    /* push bridge scope & TLV id header into the request */
    req.push(BrScoreMessageHeader{});
    req.push(TlvIdMessageHeader{tlv_id});

    /* send LLDP request to the LLDP daemon */
    auto resp = conn.send(req);
    if (!resp) {
        if (NO_AGENT == resp.get_status()) {
            throw exception::ApiError(exception::ErrCode::NO_INFO,
                                      "LLDP Agent instance not found for given interface name");
        }
        /* failed to send the request */
        throw exception::ApiError("Request message send failed ("
            + std::to_string(resp.get_status()) + ")");
    }

    /* get response information */
    auto cmd_hdr = resp.pop<CommandMessageHeader>();

    /* check response command code and options */
    if ((CommandMessageHeader::CmdCode::GET_TLV != cmd_hdr.get_code()) ||
            (cmd_hdr.get_option() != opts)) {
        throw exception::ApiError("Response command header error");
    }

    /* parse and check TLV id */
    auto tlv_id_hdr = resp.pop<TlvIdMessageHeader>();
    if (tlv_id != TlvId(tlv_id_hdr)) {
        throw exception::ApiError("Wrong TLV id in response");
    }

    try {
        /* get & return TLV payload */
        auto tlv_hdr = resp.pop<TlvMessageHeader>();
        return tlv_hdr.get_payload();
    }
    catch (const exception::ApiError& e) {
        if (exception::ErrCode::EOM == e.code()) {
            throw exception::ApiError(exception::ErrCode::NO_INFO,
                "Port Description TLV not available");
        }
        throw;
    }
}

void Client::enable_tlv(TlvId tlv_id, bool enable) {
    Request req{};

    /* push enableTx tlv header into the request */
    uint32_t opts = uint32_t(CommandMessageHeader::CmdOption::ARG);
    opts |= uint32_t(CommandMessageHeader::CmdOption::ARGVAL);

    req.push(CommandMessageHeader{m_ifname,
        CommandMessageHeader::CmdType::REQUEST,
        CommandMessageHeader::CmdCode::SET_TLV,
        opts});

    /* push bridge scope & TLV id header into the request */
    req.push(BrScoreMessageHeader{});
    req.push(TlvIdMessageHeader{tlv_id});

    /* push enableTx=yes/no argument into the request */
    string value = enable ? ENABLE_TLV_VALUE : DISABLE_TLV_VALUE;
    req.push(ArgValueMessageHeader{ENABLE_TLV_ARG, value});

    /* send LLDP request to the LLDP daemon */
    auto resp = ::send_request(req);

    /* get response information */
    auto cmd_hdr = resp.pop<CommandMessageHeader>();

    /* check response command code and options */
    if ((CommandMessageHeader::CmdCode::SET_TLV != cmd_hdr.get_code()) ||
            (cmd_hdr.get_option() != opts)) {
        throw exception::ApiError("Response command header error");
    }

    /* parse and check TLV id */
    auto tlv_id_hdr = resp.pop<TlvIdMessageHeader>();
    if (tlv_id != TlvId(tlv_id_hdr)) {
        throw exception::ApiError("Wrong TLV id in response");
    }
}

void Client::set_mode(Mode mode) {
    Request req{};

    /* push set admin status header into the request */
    uint32_t opts = uint32_t(CommandMessageHeader::CmdOption::ARG);
    opts |= uint32_t(CommandMessageHeader::CmdOption::ARGVAL);

    req.push(CommandMessageHeader{m_ifname,
        CommandMessageHeader::CmdType::REQUEST,
        CommandMessageHeader::CmdCode::SET_LLDP,
        opts});

    /* push bridge scope header into the request */
    req.push(BrScoreMessageHeader{});

    /* push adminStatus=rx/tx/rxtx/disabled argument into the request */
    std::map<Mode, string> enum_map{
        {Mode::DISABLED, ADMIN_STATUS_DISABLED},
        {Mode::RXTX, ADMIN_STATUS_RXTX},
        {Mode::RX, ADMIN_STATUS_RX},
        {Mode::TX, ADMIN_STATUS_TX}
    };
    auto value_it = enum_map.find(mode);
    if (enum_map.end() == value_it) {
        throw exception::ApiError("Invalid LLDP mode");
    }
    req.push(ArgValueMessageHeader{ADMIN_STATUS_ARG, value_it->second});

    /* send LLDP request to the LLDP daemon */
    auto resp = ::send_request(req);

    /* get response information */
    auto cmd_hdr = resp.pop<CommandMessageHeader>();

    /* check response command code and options */
    if ((CommandMessageHeader::CmdCode::SET_LLDP != cmd_hdr.get_code()) ||
            (cmd_hdr.get_option() != opts)) {
        throw exception::ApiError("Response command header error");
    }
}

Client::Mode Client::get_mode() const {
    Request req{};

    /* push get admin status header into the request */
    uint32_t opts = uint32_t(CommandMessageHeader::CmdOption::ARG);
    req.push(CommandMessageHeader{m_ifname,
        CommandMessageHeader::CmdType::REQUEST,
        CommandMessageHeader::CmdCode::GET_LLDP,
        opts});

    /* push bridge scope & adminStatus header into the request */
    req.push(BrScoreMessageHeader{});
    req.push(ArgMessageHeader{ADMIN_STATUS_ARG});

    /* send LLDP request to the LLDP daemon */
    auto resp = ::send_request(req);

    /* get response information */
    auto cmd_hdr = resp.pop<CommandMessageHeader>();

    /* check response command code and options */
    if ((CommandMessageHeader::CmdCode::GET_LLDP != cmd_hdr.get_code()) ||
            (cmd_hdr.get_option() != opts)) {
        throw exception::ApiError("Response command header error");
    }

    /* get & return admin status */
    auto arg = resp.pop<ArgValueMessageHeader>();
    if (ADMIN_STATUS_ARG != arg.get_name()) {
        throw exception::ApiError("Invalid response arg name");
    }

    /* convert admin status string to enum */
    std::map<string, Mode> enum_map {
        {ADMIN_STATUS_DISABLED, Mode::DISABLED},
        {ADMIN_STATUS_RXTX, Mode::RXTX},
        {ADMIN_STATUS_RX, Mode::RX},
        {ADMIN_STATUS_TX, Mode::TX}
    };
    auto enum_it = enum_map.find(arg.get_value());
    if (enum_it == enum_map.end()) {
        throw exception::ApiError("Invalid response admin status");
    }

    /* return LLDP mode */
    return enum_it->second;
}

void Client::agent_reload() const {
    if (!kill(get_agent_pid(), SIGHUP)) {
        throw exception::ApiError("Cannot restart LLDP agent daemon");
    }
}
