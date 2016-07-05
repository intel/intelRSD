/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file switch_vlan.cpp
 *
 * @brief Netlink Vlan functionality.
 * */

#include "api/netlink/switch_vlan.hpp"
#include "netlink/socket.hpp"
#include "api/netlink/vlan_message.hpp"

using namespace netlink_base;
using namespace agent::network::api::netlink;

SwitchVlan::SwitchVlan(VlanId vlan_id) {
    set_vlan_id(vlan_id);
}

SwitchVlan::~SwitchVlan() { }

void SwitchVlan::add_switch_port(const PortIdentifier& port, bool tagged) {
    Socket socket{};
    socket.connect();
    AddVlanPortMessage vlan_msg(port, get_vlan_id(), tagged);
    socket.send_message(vlan_msg);
    socket.receive_message(vlan_msg);
}

void SwitchVlan::remove_switch_port(const PortIdentifier& port) {
    Socket socket;
    socket.connect();
    DelVlanPortMessage vlan_msg(port, get_vlan_id());
    socket.send_message(vlan_msg);
}

void SwitchVlan::get_switch_info(const PortIdentifier& port, VlanPortInfo& info) {
    Socket socket;
    socket.connect();
    InfoVlanPortMessage vlan_msg;
    socket.send_message(vlan_msg);
    socket.receive_message(vlan_msg);
    info.set_tagged(vlan_msg.is_tagged(port, get_vlan_id()));
}
