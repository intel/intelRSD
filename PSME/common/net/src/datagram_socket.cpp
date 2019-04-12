//
// DatagramSocket.cpp
//
// $Id: //poco/1.4/Net/src/DatagramSocket.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  DatagramSocket
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
 * @file datagram_socket.cpp
 *
 * @brief DatagramSocket interface
 * */

#include "net/datagram_socket.hpp"
#include "net/datagram_socket_impl.hpp"

namespace net {

DatagramSocket::DatagramSocket() : Socket(SocketImpl::Ptr(new DatagramSocketImpl)) {
}

DatagramSocket::DatagramSocket(AddressFamily family)
    : Socket(SocketImpl::Ptr(new DatagramSocketImpl(family))) {
}

DatagramSocket::DatagramSocket(const SocketAddress& rAddress, bool reuseAddress)
    : Socket(SocketImpl::Ptr(new DatagramSocketImpl(rAddress.family()))) {
    bind(rAddress, reuseAddress);
}

DatagramSocket::DatagramSocket(const Socket& socket) : Socket(socket) {
    if (!dynamic_cast<DatagramSocketImpl*> (impl())) {
        throw NetException("Cannot assign incompatible socket");
    }
}

DatagramSocket::~DatagramSocket() {
}

DatagramSocket& DatagramSocket::operator=(const Socket& socket) {
    if (dynamic_cast<DatagramSocketImpl*> (socket.impl())) {
        Socket::operator=(socket);
    }
    else {
        throw NetException("Cannot assign incompatible socket");
    }
    return *this;
}

void DatagramSocket::connect(const SocketAddress& rAddress) {
    impl()->connect(rAddress);
}

void DatagramSocket::bind(const SocketAddress& rAddress, bool reuseAddress) {
    impl()->bind(rAddress, reuseAddress);
}

long DatagramSocket::send_bytes(const void* buffer, size_t length, int flags) {
    return impl()->send_bytes(buffer, length, flags);
}

long DatagramSocket::receive_bytes(void* buffer, size_t length, int flags) {
    return impl()->receive_bytes(buffer, length, flags);
}

long DatagramSocket::send_to(const void* buffer, size_t length, const SocketAddress& rAddress, int flags) {
    return impl()->send_to(buffer, length, rAddress, flags);
}

long DatagramSocket::receive_from(void* buffer, size_t length, SocketAddress& rAddress, int flags) {
    return impl()->receive_from(buffer, length, rAddress, flags);
}

void DatagramSocket::set_broadcast(bool flag) {
    impl()->set_broadcast(flag);
}

bool DatagramSocket::is_broadcast_enabled() const {
    return impl()->get_broadcast();
}

} // namespace net
