//
// StreamSocket.cpp
//
// $Id: //poco/1.4/Net/src/StreamSocket.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  StreamSocket
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
 * @file stream_socket.cpp
 *
 * @brief StreamSocket interface
 * */

#include "net/stream_socket.hpp"
#include "net/stream_socket_impl.hpp"

namespace net {

StreamSocket::StreamSocket() : Socket(SocketImpl::Ptr(new StreamSocketImpl)) {
}

StreamSocket::StreamSocket(const SocketAddress& address)
    : Socket(SocketImpl::Ptr(new StreamSocketImpl(address.family()))) {
    connect(address);
}

StreamSocket::StreamSocket(AddressFamily family)
    : Socket(SocketImpl::Ptr(new StreamSocketImpl(family))) {
}

StreamSocket::StreamSocket(const Socket& socket) : Socket(socket) {
    if (!dynamic_cast<StreamSocketImpl*>(impl())) {
        throw NetException("incompatible socket");
    }
}

StreamSocket::StreamSocket(SocketImpl::Ptr p_impl) : Socket(std::move(p_impl)) {
    if (!dynamic_cast<StreamSocketImpl*>(impl())) {
        throw NetException("incompatible socket");
    }
}

StreamSocket::~StreamSocket() {
}

StreamSocket& StreamSocket::operator=(const Socket& socket) {
    if (dynamic_cast<StreamSocketImpl*>(socket.impl())) {
        Socket::operator=(socket);
    }
    else {
        throw NetException("incompatible socket");
    }
    return *this;
}

void StreamSocket::connect(const SocketAddress& address) {
    impl()->connect(address);
}

void StreamSocket::connect(const SocketAddress& address, const Duration& timeout) {
    impl()->connect(address, timeout);
}

void StreamSocket::connect_non_blocking(const SocketAddress& address) {
    impl()->connect_non_blocking(address);
}

void StreamSocket::shutdown_receive() {
    impl()->shutdown_receive();
}

void StreamSocket::shutdown_send() {
    impl()->shutdown_send();
}

void StreamSocket::shutdown() {
    impl()->shutdown();
}

long StreamSocket::send_bytes(const void* buffer, size_t length, int flags) {
    return impl()->send_bytes(buffer, length, flags);
}

long StreamSocket::receive_bytes(void* buffer, size_t length, int flags) {
    return impl()->receive_bytes(buffer, length, flags);
}

void StreamSocket::send_urgent(unsigned char data) {
    impl()->send_urgent(data);
}

} // namespace net
