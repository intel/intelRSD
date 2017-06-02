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
 * @file lldp_interface.cpp
 *
 * @brief LLDP communication interface.
 * */

#include "api/lldp/interface.hpp"
#include "api/lldp/exceptions/api_error.hpp"
#include "api/lldp/command_message_header.hpp"
#include "net/socket_address.hpp"

#include <sstream>
#include <memory>

using namespace agent::network::api::lldp;

namespace {

net::SockaddrStorage create_sockaddr_storage(const std::string& path) {
    net::SockaddrStorage sockaddr_storage{};
    struct sockaddr_un* local =
            reinterpret_cast<struct sockaddr_un*>(sockaddr_storage.addr);
    local->sun_family = AF_UNIX;
    local->sun_path[0] = '\0';
    snprintf(local->sun_path + 1, sizeof (local->sun_path) - 1,
            "%s", path.c_str());
    sockaddr_storage.addr_len =
            socklen_t(sizeof (sa_family_t) + strlen(local->sun_path + 1) + 1);
    return sockaddr_storage;
}

std::string send_request(net::DatagramSocket& sock, const std::string& command) {
    static constexpr int TIMEOUT = 3;

    sock.send_bytes(command.c_str(), command.length());

    while (true) {
        auto can_read = sock.poll(std::chrono::seconds(TIMEOUT),
                                  net::Socket::SELECT_READ);
        if (can_read) {
            static constexpr int MAXRESP{4096};
            char buf[MAXRESP];
            auto rc = sock.receive_bytes(buf, MAXRESP);
            if (buf[CommandMessageHeader::OFFSET_TYPE] !=
                    char(CommandMessageHeader::CmdType::RESPONSE)) {
                continue;
            }
            return std::string(buf, rc);
        }
        else {
            throw net::TimeoutException();
        }
    }
}

}

Interface::Interface() {
    try {
        net::SocketAddress local{create_sockaddr_storage("/com/intel/lldpad/" +
                                    std::to_string(getpid()))};
        net::SocketAddress dest{create_sockaddr_storage("/com/intel/lldpad")};
        m_sock.bind(local);
        m_sock.connect(dest);
    }
    catch (const net::NetException& e) {
        throw exception::ApiError(exception::ErrCode::NO_CONNECTION, e.what());
    }
}

Response Interface::send(const Request& request) {
    try {
        /* send request to the LLDP agent daemon, return response */
        return Response{::send_request(m_sock, request.get_message())};
    }
    catch (const std::exception& e) {
        throw exception::ApiError(std::string("send_request() failed: ") + e.what());
    }
}

Interface::~Interface() {
}
