/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file del_vlan_port_message.cpp
 *
 * @brief Delete vlan port message
 * */

#include "api/netlink/del_vlan_port_message.hpp"

#include <netlink/msg.h>

using std::string;
using namespace agent::network::api::netlink;

DelVlanPortMessage::DelVlanPortMessage(const string& port, uint32_t vlan) :
    AddVlanPortMessage(port, vlan, true, false) {
    /* set-up Netlink attributes */
    set_nlhdr_type(RTM_DELLINK);
    set_nlhdr_flags(0);
}

DelVlanPortMessage::~DelVlanPortMessage() { }
