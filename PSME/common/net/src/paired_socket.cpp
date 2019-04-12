/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file paired_socket.cpp
 *
 * @brief PairedSocket interface
 * */

#include "net/paired_socket.hpp"
#include "net/socket_impl.hpp"

namespace net {

class PairedSocketImpl : public SocketImpl {
public:

    PairedSocketImpl(net_socket_t fd) : SocketImpl(fd) {
        set_blocking(false);
    }

    ~PairedSocketImpl();
};

PairedSocketImpl::~PairedSocketImpl() {
}

PairedSocket::PairedSocket(net_socket_t fd) : Socket(SocketImpl::Ptr(new PairedSocketImpl(fd))) {
}

PairedSocket::~PairedSocket() {
}

long PairedSocket::send_bytes(const void* buffer, size_t length, int flags) {
    return impl()->send_bytes(buffer, length, flags);
}

long PairedSocket::receive_bytes(void* buffer, size_t length, int flags) {
    return impl()->receive_bytes(buffer, length, flags);
}

PairedSockets PairedSocket::create_socket_pair() {
    int fd_pair[2];
    if (0 != ::socketpair(AF_UNIX, SOCK_DGRAM, 0, fd_pair)) {
        throw NetException("Failed to create socketpair.");
    }
    return PairedSockets{PairedSocket(fd_pair[0]), PairedSocket(fd_pair[1])};
}

} // namespace net
