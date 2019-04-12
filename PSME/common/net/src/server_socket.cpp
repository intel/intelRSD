//
// ServerSocketImpl.cpp
//
// $Id: //poco/1.4/Net/src/ServerSocketImpl.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  ServerSocketImpl
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//
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
 * @file server_socket.cpp
 *
 * @brief ServerSocket interface
 * */

#include "net/server_socket.hpp"
#include "net/socket_impl.hpp"

namespace net {

/*!
 * Server stream socket implementation.
 */
class ServerSocketImpl : public SocketImpl {
public:
    /*! Constructor */
    ServerSocketImpl() {}

protected:
    /*! Destructor */
    virtual ~ServerSocketImpl();
};

ServerSocketImpl::~ServerSocketImpl() {}

ServerSocket::ServerSocket() : Socket(SocketImpl::Ptr(new ServerSocketImpl)) {
}

ServerSocket::ServerSocket(const Socket& socket) : Socket(socket) {
    if (!dynamic_cast<ServerSocketImpl*> (impl())) {
        throw NetException("Cannot assign incompatible socket");
    }
}

ServerSocket::ServerSocket(const SocketAddress& address, int backlog)
    : Socket(SocketImpl::Ptr(new ServerSocketImpl)) {
    impl()->bind(address, true);
    impl()->listen(backlog);
}

ServerSocket::ServerSocket(std::uint16_t port, int backlog)
    : Socket(SocketImpl::Ptr(new ServerSocketImpl)) {
    IpAddress any_ipv6_address;
    SocketAddress socket_address(any_ipv6_address, port);
    impl()->bind(socket_address, true);
    impl()->listen(backlog);
}

ServerSocket::ServerSocket(SocketImpl::Ptr pimpl) : Socket(std::move(pimpl)) {
}

ServerSocket::~ServerSocket() {
}

ServerSocket& ServerSocket::operator=(const Socket& socket) {
    if (dynamic_cast<ServerSocketImpl*> (socket.impl())) {
        Socket::operator=(socket);
    }
    else {
        throw NetException("Cannot assign incompatible socket");
    }
    return *this;
}

void ServerSocket::bind(const SocketAddress& address, bool reuse_address) {
    impl()->bind(address, reuse_address);
}

void ServerSocket::bind(std::uint16_t port, bool reuse_address) {
    IpAddress any_address;
    SocketAddress socketAddress(any_address, port);
    impl()->bind(socketAddress, reuse_address);
}

#ifdef NET_HAVE_IPv6
void ServerSocket::bind6(const SocketAddress& address, bool reuse_address, bool ipv6only) {
    impl()->bind6(address, reuse_address, ipv6only);
}

void ServerSocket::bind6(std::uint16_t port, bool reuse_address, bool ipv6only) {
    IpAddress any_ipv6_addr(in6addr_any);
    SocketAddress socketAddress(any_ipv6_addr, port);
    impl()->bind6(socketAddress, reuse_address, ipv6only);
}
#endif

void ServerSocket::listen(int backlog) {
    impl()->listen(backlog);
}

StreamSocket ServerSocket::accept(SocketAddress& client_addr) {
    return StreamSocket(impl()->accept(client_addr));
}

StreamSocket ServerSocket::accept() {
    SocketAddress client_addr;
    return StreamSocket(impl()->accept(client_addr));
}

} // namespace net
