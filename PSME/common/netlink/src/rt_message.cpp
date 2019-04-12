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
 * @file rt_message.cpp
 *
 * @brief Route Netlink message
 * */

#include "netlink/rt_message.hpp"

#include <net/if.h>

using std::string;
using namespace netlink_base;

RtMessage::RtMessage(const string& iface) :
        m_ifname{iface}, NlMessage(NlSocket::Protocol::ROUTE) {
    /* try to get ifindex of provided interface name */
    m_ifindex = if_nametoindex(get_ifname().c_str());
}

bool RtMessage::is_ifname_valid() const {
    /* validate the interface name. Currently, there is only
     * validation for interface name len. Later it can be
     * improved if needed */
    return IFNAMSIZ < get_ifname().length();
}

RtMessage::~RtMessage() { }
