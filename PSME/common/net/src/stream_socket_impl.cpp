//
// StreamSocketImpl.cpp
//
// $Id: //poco/1.4/Net/src/StreamSocketImpl.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  StreamSocketImpl
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
 * @file stream_socket_impl.cpp
 *
 * @brief StreamSocket implementation
 * */

#include "net/stream_socket_impl.hpp"
#include <thread>

namespace net {

StreamSocketImpl::StreamSocketImpl() {
}

StreamSocketImpl::StreamSocketImpl(AddressFamily family) {
    if (AddressFamily::IPv4 == family) {
        init(AF_INET);
    }
#ifdef NET_HAVE_IPv6
    else if (AddressFamily::IPv6 == family) {
        init(AF_INET6);
    }
#endif
#if defined(NET_OS_FAMILY_UNIX)
    else if (AddressFamily::UNIX_LOCAL == family) {
        init(AF_UNIX);
    }
#endif
    else {
        throw NetException("Invalid or unsupported address family passed to StreamSocketImpl");
    }
}

StreamSocketImpl::StreamSocketImpl(net_socket_t socketfd) : SocketImpl(socketfd) {
}

StreamSocketImpl::~StreamSocketImpl() {
}

long StreamSocketImpl::send_bytes(const void* buffer, size_t length, int flags) {
    const char* p = reinterpret_cast<const char*>(buffer);
    long remaining = length;
    long sent = 0;
    const auto blocking = get_blocking();
    while (remaining > 0) {
        auto n = SocketImpl::send_bytes(p, remaining, flags);
        if (n < 0) {
            throw NetException("send_bytes error");
        }
        p += n;
        sent += n;
        remaining -= n;
        if (blocking && remaining > 0) {
            std::this_thread::yield();
        }
        else {
            break;
        }
    }
    return sent;
}

} // namespace net
