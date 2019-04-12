//
// DatagramSocketImpl.cpp
//
// $Id: //poco/1.4/Net/src/DatagramSocketImpl.cpp#1 $
//
// Library: Net
// Package: Sockets
// Module:  DatagramSocketImpl
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
 * @file datagram_socket_impl.cpp
 *
 * @brief DatagramSocket implementation
 * */

#include "net/datagram_socket_impl.hpp"

namespace net {

DatagramSocketImpl::DatagramSocketImpl() {
}

DatagramSocketImpl::DatagramSocketImpl(AddressFamily family) {
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
        throw NetException("Invalid or unsupported address family passed to DatagramSocketImpl");
    }
}

DatagramSocketImpl::DatagramSocketImpl(net_socket_t socketfd)
    : SocketImpl(socketfd) {
}

DatagramSocketImpl::~DatagramSocketImpl() {
}

void DatagramSocketImpl::init(int af) {
    init_socket(af, SOCK_DGRAM);
}

} // namespace net
