/*!
 * @brief Switch Port Info class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file agent/network/src/hal/eos_sdk/switch_port_info_impl.cpp
 */

#include "hal/switch_port_info_impl.hpp"
#include "hal/eos_sdk/eos_agent.hpp"


using namespace agent::network::hal;

void SwitchPortInfoImpl::get_switch_port_link_state() {
    bool admin_up =
        EosAgent::get_instance()->is_admin_enabled(get_port_identifier());
    bool operational_up =
        EosAgent::get_instance()->is_operational_up(get_port_identifier());
    set_link_state(admin_up ?
                   SwitchPortInfo::State::Up :
                   SwitchPortInfo::State::Down );
    set_operational_state(operational_up ?
                          SwitchPortInfo::State::Up :
                          SwitchPortInfo::State::Down);
}

void SwitchPortInfoImpl::set_switch_port_link_state() {
    if (State::Unknown == get_link_state_enum()) {
        THROW(agent_framework::exceptions::NetworkError, "network",
              "Unknown port state");
    }
    EosAgent::get_instance()->set_admin_enable(get_port_identifier(),
                                               get_link_state_enum() == State::Up ? true : false);
}

uint32_t SwitchPortInfoImpl::get_switch_port_speed() {
    return EosAgent::get_instance()->get_speed(get_port_identifier());
}

void SwitchPortInfoImpl::get_switch_port_default_vid() {
    std::string port = get_port_identifier();
    uint16_t default_vlan = EosAgent::get_instance()->get_default_vlan(port);
    if (0 != default_vlan) {
        set_default_vlan_id(default_vlan);
    }
}

SwitchPortInfo::VlanInfoList SwitchPortInfoImpl::get_vlans() const {
    VlanInfoList vlans{};
    std::string port = get_port_identifier();
    for (uint32_t vid : EosAgent::get_instance()->get_tagged_vlans(port)) {
        vlans.emplace_back(vid, true);
    }
    uint32_t default_vlan = EosAgent::get_instance()->get_default_vlan(port);
    if (0 != default_vlan) {
        vlans.emplace_back(default_vlan, false);
    }
    return vlans;
}
