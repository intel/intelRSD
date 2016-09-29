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
 * @file lldp_interface.cpp
 *
 * @brief LLDP communication interface.
 * */

#include "api/lldp/interface.hpp"
#include "api/lldp/exceptions/api_error.hpp"
#include "api/lldp/command_message_header.hpp"

#include <sstream>
#include <memory>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using std::string;
using namespace agent::network::api::lldp;

namespace {
    int send_request(int sock, const string& command, string& reply,
                     size_t& received) {
        static constexpr int TIMEOUT = 3;
        if (send(sock, command.c_str(), command.length(), 0) < 0) {
            return -1;
        }
        timeval tv{};
        fd_set fd{};
        while(true) {
            tv.tv_sec = TIMEOUT;
            tv.tv_usec = 0;
            FD_ZERO(&fd);
            FD_SET(sock, &fd);
            int ret = select(sock + 1, &fd, NULL, NULL, &tv);
            if (ret == -1) {
                return -1;
            }
            if (ret == 0) { // timeout
                return -2;
            }
            if (FD_ISSET(sock, &fd)) {
                static constexpr int MAXRESP{4096};
                char buf[MAXRESP + 1];
                auto rc = recv(sock, buf, MAXRESP, 0);
                if (rc < 0) {
                    return -1;
                }
                if (buf[CommandMessageHeader::OFFSET_TYPE] !=
                    char(CommandMessageHeader::CmdType::RESPONSE)) {
                    continue;
                }
                buf[rc] = '\0';
                reply = buf;
                received = rc;
                return 0;
            }
            else {
                return -2;
            }
        }
    }
}

Interface::Interface() {
    m_sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (m_sock < 0) {
        return;
    }
    sockaddr_un local{};
    sockaddr_un dest{};
    socklen_t len;
    local.sun_family = AF_LOCAL;
    local.sun_path[0] = '\0';
    snprintf(local.sun_path + 1, sizeof(local.sun_path) - 1,
             "/com/intel/lldpad/%d", getpid());
    len = socklen_t(sizeof(sa_family_t) + strlen(local.sun_path + 1) + 1);
    if (bind(m_sock, reinterpret_cast<sockaddr*>(&local), len) < 0) {
        close(m_sock);
        m_sock = -1;
        return;
    }
    dest.sun_family = AF_LOCAL;
    dest.sun_path[0] = '\0';
    snprintf(dest.sun_path + 1, sizeof(dest.sun_path) - 1,
             "/com/intel/lldpad");
    len = socklen_t(sizeof(sa_family_t) + strlen(dest.sun_path + 1) + 1);
    if (connect(m_sock, reinterpret_cast<struct sockaddr *> (&dest), len) < 0) {
        close(m_sock);
        m_sock = -1;
    }
}

Response Interface::send(const Request& request) {
    size_t received{};
    string response{};

    /* sanity check */
    if (-1 == m_sock) {
        throw exception::ApiError(exception::ErrCode::NO_CONNECTION,
            "LLDP agent connection is closed");
    }

    /* send request to the LLDP agent daemon */
    const auto& req_msg = request.get_message();
    int rc = ::send_request(m_sock, req_msg, response, received);
    if (rc < 0) {
        throw exception::ApiError("send_request() failed");
    }

    /* return response */
    return Response{{response.c_str(), received}};
}

Interface::~Interface() {
    if (m_sock != -1) {
        close(m_sock);
    }
}
