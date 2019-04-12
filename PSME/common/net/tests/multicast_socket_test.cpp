/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "net/multicast_socket.hpp"
#include "net/network_interface.hpp"
#include "net/socket_address.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cstdio>
#include <fstream>
#include <sstream>

using namespace net;

namespace {
NetworkInterface find_interface() {
    NetworkInterface::List list = NetworkInterface::get_interfaces();
    for (const auto& nic: list) {
        if (nic.supports_multicast() && nic.is_running()) {
            return nic;
        }
    }
    throw NetException("No valid network interface found.");
}
}

TEST(MulticastSocketTest, Constructor) {
    MulticastSocket msock(SocketAddress(), true);
    EXPECT_TRUE(nullptr != msock.impl());
}

TEST(MulticastSocketTest, JoinLeaveMulticastGroup) {
    constexpr const uint16_t port = 9998;
    constexpr const char mcast_group_cstr[] = "237.132.100.17";

    auto group_ip = IpAddress::from_string(mcast_group_cstr);

    MulticastSocket msock(SocketAddress(port), true);
    auto nic = find_interface();
    msock.join_group(group_ip, nic);
    EXPECT_THROW(msock.join_group(group_ip, nic), NetException);
    msock.leave_group(group_ip, nic);
    EXPECT_THROW(msock.leave_group(group_ip, nic), NetException);
}
