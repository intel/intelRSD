/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
*/

#include "iscsi/socket.hpp"

#include <safe-string/safe_lib.hpp>

#include <stdexcept>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using namespace agent::storage::iscsi::tgt;

namespace {
constexpr const char* UNIX_TGT_SOCKET = "/var/run/tgtd.ipc_abstract_namespace.0";

union sock_address {
    sockaddr_un m_un;
    sockaddr m_addr;
};
}

Socket::Socket() {
    create_socket(AF_LOCAL, SOCK_STREAM, 0);
}

Socket::~Socket() {
    destroy_socket();
}

int Socket::get_descriptor() const {
    return m_fd;
}

void Socket::connect() {
    sock_address addr;
    memset(&addr, 0, sizeof(addr));

    addr.m_un.sun_family = AF_LOCAL;
    strncpy_s(addr.m_un.sun_path, sizeof(addr.m_un.sun_path),
              UNIX_TGT_SOCKET,
              sizeof(addr.m_un.sun_path));

    auto ret = ::connect(m_fd,
                         &addr.m_addr,
                         static_cast<socklen_t>(strnlen_s(addr.m_un.sun_path, RSIZE_MAX_STR) +
                                                sizeof(addr.m_un.sun_family)));
    if (0 > ret) {
        throw std::runtime_error(
            "Cannot connect to socket error code: " + std::to_string(ret));
    }
}

void Socket::send(const std::vector<char>& in) const {
    auto write_size = ::write(m_fd, &in[0], in.size());
    if (0 > write_size || in.size() != static_cast<std::size_t>(write_size)) {
        throw std::runtime_error("Cannot write data to socket");
    }
}

void Socket::read(std::vector<char>& out) const {
    std::size_t all = 0;
    while (all < out.size()) {
        auto read_size = ::read(m_fd, &out[0], out.size());
        if (0 > read_size) {
            throw std::runtime_error(
                "Cannot read data from socket" + std::to_string(read_size));
        }
        all += static_cast<std::size_t>(read_size);
    }
}

void Socket::recive(char* buffer, const std::size_t size) {
    auto ec = ::recv(m_fd, buffer, size, MSG_WAITALL);
    if (0 >= ec) {
        throw std::runtime_error("Cannot read data from socket");
    }
}

void Socket::create_socket(int domain, int type, int protocol) {
    m_fd = socket(domain, type, protocol);
    if (0 > m_fd) {
        throw std::runtime_error("Cannot create socket");
    }
}

void Socket::destroy_socket() {
    close(m_fd);
}
