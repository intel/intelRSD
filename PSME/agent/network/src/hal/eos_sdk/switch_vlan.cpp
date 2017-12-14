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
 * @file switch_vlan.cpp
 *
 * @brief Switch Vlan functionality.
 * */

#include "hal/switch_vlan.hpp"
#include "hal/eos_sdk/eos_agent.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/module/constants/network.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_network.hpp"


using namespace agent::network::hal;
using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::module;

SwitchVlan::SwitchVlan(VlanId vlan_id) {
    set_vlan_id(vlan_id);
}

void SwitchVlan::add_port(const PortIdentifier& port, bool tagged) {
    if (tagged) {
        EosAgent::get_instance()->add_tagged_vlan(port, get_vlan_id());
    }
    else {
        THROW(UnsupportedValue, "network-agent",
              "Adding untagged VLAN is not supported");
    }
}

void SwitchVlan::remove_port(const PortIdentifier& port, bool tagged) {
    if (tagged) {
        EosAgent::get_instance()->delete_tagged_vlan(port, get_vlan_id());
    }
    else {
        THROW(MethodNotAllowed, "network-agent",
              "Removing untagged VLAN is not supported");
    }
}

void SwitchVlan::set_attribute(const std::string& vlan_uuid,
                               const std::string& attribute_name,
                               const json::Json& value) const {
    if (literals::Vlan::VLAN_ID == attribute_name) {
        auto& vlan_manager = get_manager<EthernetSwitchPortVlan>();
        const auto& port_vlan = vlan_manager.get_entry(vlan_uuid);
        if (port_vlan.get_tagged()) {
            THROW(agent_framework::exceptions::UnsupportedValue, "network-agent",
                  "Changing ID of a tagged VLAN is not supported");
        }
        auto vlan_id = value.get<uint32_t>();
        const auto entries =
            vlan_manager.get_entries(port_vlan.get_parent_uuid(),
                                     [&vlan_id](const EthernetSwitchPortVlan& entry) {return vlan_id == entry.get_vlan_id();});
        if (!entries.empty()) {
            THROW(agent_framework::exceptions::InvalidValue, "network-agent",
                  "VLAN ID is already defined on this port");
        }
        if (vlan_id >= 1 && vlan_id <= 4094) {
            auto& port_manager = get_manager<EthernetSwitchPort>();
            const auto& port =
                port_manager.get_entry(port_vlan.get_parent_uuid());
            // update default VLAN on the switch port
            EosAgent::get_instance()->set_default_vlan(port.get_port_identifier(),
                                                       uint16_t(vlan_id));
            // update in the model
            auto vlan_ref = vlan_manager.get_entry_reference(vlan_uuid);
            vlan_ref->set_vlan_id(vlan_id);
        }
        else {
            THROW(agent_framework::exceptions::InvalidValue,
                  "network-agent", "VLAN ID is not valid.");
        }
    }
    else {
        THROW(agent_framework::exceptions::UnsupportedField, "network-agent",
              "Setting VLAN attribute is not supported.",
              attribute_name, value);
    }
}

SwitchVlan::~SwitchVlan() {}
