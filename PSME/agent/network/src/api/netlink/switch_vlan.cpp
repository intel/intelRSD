/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file switch_vlan.cpp
 *
 * @brief Netlink Vlan functionality.
 * */

#include "api/netlink/switch_vlan.hpp"
#include "api/netlink/vlan_message.hpp"

using namespace netlink_base;
using namespace agent::network::api::netlink;

SwitchVlan::SwitchVlan(VlanId vlan_id) {
    set_vlan_id(vlan_id);
}

SwitchVlan::~SwitchVlan() { }

void SwitchVlan::add_switch_port(const PortIdentifier& port, bool tagged) {
    AddVlanPortMessage vlan_msg{port, get_vlan_id(), tagged, false};
    vlan_msg.send();
}

void SwitchVlan::remove_switch_port(const PortIdentifier& port) {
    DelVlanPortMessage vlan_msg{port, get_vlan_id()};
    vlan_msg.send();
}

void SwitchVlan::get_switch_info(const PortIdentifier& port, VlanPortInfo& info) {
    InfoVlanPortMessage vlan_msg{};
    vlan_msg.send();
    info.set_tagged(vlan_msg.is_tagged(port, get_vlan_id()));
}
