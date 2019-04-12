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
 * @file multicast_socket.hpp
 *
 * @brief MulticastSocket interface
 * */

#pragma once

#include "net/datagram_socket_impl.hpp"

#include "gmock/gmock.h"

namespace net {

class MockDatagramSocketImpl : public DatagramSocketImpl {
public:
    MOCK_METHOD4(send_to, long(const void*, size_t, const SocketAddress&, int));
    ~MockDatagramSocketImpl();
};

class MockMulticastSocket : public Socket {
public:
    MockMulticastSocket(SocketImpl::Ptr implementation) : Socket(std::move(implementation)) { }
    MockMulticastSocket(const MockMulticastSocket&) = default;
    ~MockMulticastSocket();
};

}
