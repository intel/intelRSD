/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file get_port_state_message.cpp
 *
 * @brief Get port state message class implementation
 * */

#include "api/netlink/get_port_state_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <net/if.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

GetPortStateMessage::GetPortStateMessage(const std::string& ifname) :
    PortMessage{ifname} {
    set_family(AF_UNSPEC);
    set_filter_mask(0);
}

GetPortStateMessage::~GetPortStateMessage() {}

void GetPortStateMessage::process_link_message(struct nl_msg*,
                                               const struct ifinfomsg* ifm) {
    if (ifm->ifi_flags & IFF_UP) {
        set_link_state(State::Up);
    }
    else {
        set_link_state(State::Down);
    }
    if (ifm->ifi_flags & IFF_RUNNING) {
        set_operational_state(State::Up);
    }
    else {
        set_operational_state(State::Down);
    }
}
