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
 * @file switch_vlan_port.cpp
 *
 * @brief Switch Vlan Port functionality.
 * */

#include "hal/switch_port_vlan.hpp"
#include "hal/eos_sdk/eos_agent.hpp"
#include "agent-framework/exceptions/exception.hpp"


using namespace agent::network::hal;
using namespace agent_framework::exceptions;

using VlanId = std::uint16_t;

void SwitchPortVlan::add_port(const PortIdentifier& port, bool tagged) {
    if (tagged) {
        EosAgent::get_instance()->add_tagged_vlan(port, get_vlan_id());
    }
    else {
        THROW(UnsupportedValue, "network-agent",
              "Adding untagged VLAN is not supported");
    }
}

void SwitchPortVlan::remove_port(const PortIdentifier& port, bool tagged) {
    if (tagged) {
        EosAgent::get_instance()->delete_tagged_vlan(port, get_vlan_id());
    }
    else {
        THROW(MethodNotAllowed, "network-agent",
              "Removing untagged VLAN is not supported");
    }
}

VlanId SwitchPortVlan::get_port_primary_vlan(const PortIdentifier& port) {
    // get default VLAN on the switch port
    return EosAgent::get_instance()->get_default_vlan(port);
}

void SwitchPortVlan::set_port_primary_vlan(const PortIdentifier& port, VlanId vlan_id) {
    // set default VLAN on the switch port
    EosAgent::get_instance()->set_default_vlan(port, vlan_id);
}
