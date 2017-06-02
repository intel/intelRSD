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
 * @file set_port_state_message.cpp
 *
 * @brief Set port state message class implementation
 * */

#include "api/netlink/set_port_state_message.hpp"
#include "netlink/nl_exception_invalid_ifname.hpp"

#include <net/if.h>
#include <net/if_arp.h>

using std::string;
using namespace netlink_base;
using namespace agent::network::api::netlink;

SetPortStateMessage::SetPortStateMessage(const std::string& ifname,
                                         const State& state) :
    PortMessage{ifname}, m_link_state{state} {
    set_family(AF_UNSPEC);
    set_filter_mask(0);
    set_nlhdr_type(RTM_SETLINK);
    set_nlhdr_flags(NLM_F_REPLACE|NLM_F_REQUEST|NLM_F_ACK);
}

SetPortStateMessage::~SetPortStateMessage() {}

void SetPortStateMessage::process_link_message(struct nl_msg*,
                                               const struct ifinfomsg*) {}
void SetPortStateMessage::prepare_ifi_message(struct ifinfomsg& ifi) {
    ifi.ifi_type = ARPHRD_ETHER;
    ifi.ifi_change |= IFF_UP;
    if (State::Up == m_link_state) {
        ifi.ifi_flags = IFF_UP | IFF_RUNNING;
    }
    else {
        ifi.ifi_flags &= static_cast<unsigned int>(~IFF_UP);
    }
}
