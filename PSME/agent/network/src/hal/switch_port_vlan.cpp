/*!
 * @brief Switch Port Vlan class implementation.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file agent/network/src/hal/switch_port_vlan.cpp
 */


#include "hal/switch_info_impl.hpp"
#include "hal/switch_port_vlan.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/constants/network.hpp"


using namespace agent::network::hal;
using namespace agent_framework::model;
using namespace agent_framework::module;


SwitchPortVlan::SwitchPortVlan(VlanId vlan_id) {
    set_vlan_id(vlan_id);
}

SwitchPortVlan::~SwitchPortVlan() {}

void SwitchPortVlan::set_attribute(const std::string& vlan_uuid,
                                   const std::string& attribute_name,
                                   const json::Json& value) {
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
            const auto& port = get_manager<EthernetSwitchPort>().get_entry(port_vlan.get_parent_uuid());

            // update default VLAN on the switch port
            auto port_id = port.get_port_identifier();
            auto primary_vlan_id = get_port_primary_vlan(port_id);
            set_port_primary_vlan(port.get_port_identifier(), uint16_t(vlan_id));

            // update in the model
            auto vlan_ref = vlan_manager.get_entry_reference(vlan_uuid);
            vlan_ref->set_vlan_id(vlan_id);

            /* add new VLAN id to the switch */
            auto switch_module = get_manager<EthernetSwitch>().get_entry(port.get_parent_uuid());
            SwitchInfoImpl switch_info(switch_module.get_switch_identifier());
            switch_info.add_switch_vlan(vlan_id);

            // delete previous (primary) VLAN id from switch if it doesn't exist on any other port
            if (!switch_info.switch_vlan_exists(primary_vlan_id)) {
                switch_info.delete_switch_vlan(primary_vlan_id);
            }
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



