/*!
 * @brief Switch Info class implementation.
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
 * @file agent/network/src/hal/eos_eapi/switch_info_impl.cpp
 */

#include "hal/switch_info_impl.hpp"
#include "agent-framework/module/network_components.hpp"
#include "hal/eos_eapi/eapi_client.hpp"
#include "hal/eos_eapi/set_switch_pfc_enabled.hpp"
#include "hal/eos_eapi/set_switch_lldp_enabled.hpp"
#include "hal/eos_eapi/add_switch_vlan.hpp"
#include "hal/eos_eapi/delete_switch_vlan.hpp"
#include "hal/eos_eapi/add_switch_qos_application_protocol.hpp"
#include "hal/eos_eapi/delete_switch_qos_application_protocol.hpp"
#include "hal/eos_eapi/add_switch_qos_bandwidth_allocation.hpp"
#include "hal/eos_eapi/delete_switch_qos_bandwidth_allocation.hpp"
#include "hal/eos_eapi/add_switch_qos_priority_group_mapping.hpp"
#include "hal/eos_eapi/delete_switch_qos_priority_group_mapping.hpp"

using namespace agent::network::hal;


void SwitchInfoImpl::set_switch_lldp_enabled(bool state) {
    SetSwitchLldpEnabled command{state};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::set_switch_pfc_enabled(bool state) {
    SetSwitchPfcEnabled command{state};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::add_switch_vlan(VlanId vlan_id) {
    AddSwitchVlan command{vlan_id};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::delete_switch_vlan(VlanId vlan_id) {
    DeleteSwitchVlan command{vlan_id};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::add_switch_qos_application_protocol(ProtocolId protocol, std::uint32_t port,
                                                         std::uint32_t priority) {
    AddSwitchQosApplicationProtocol command{protocol, port, priority};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::delete_switch_qos_application_protocol(ProtocolId protocol, std::uint32_t port,
                                                         std::uint32_t priority) {
    DeleteSwitchQosApplicationProtocol command{protocol, port, priority};

    EapiClient::get_instance()->eapi_request(command);
}


void SwitchInfoImpl::add_switch_qos_priority_group_mapping(std::uint32_t priority, std::uint32_t priority_group) {
    AddSwitchQosPriorityGroupMapping command{priority, priority_group};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::delete_switch_qos_priority_group_mapping(std::uint32_t priority, std::uint32_t priority_group) {
    DeleteSwitchQosPriorityGroupMapping command{priority, priority_group};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::add_switch_qos_bandwidth_allocation(std::uint32_t bandwidth_percent,
                                                         std::uint32_t priority_group) {
    AddSwitchQosBandwidthAllocation command{bandwidth_percent, priority_group};

    EapiClient::get_instance()->eapi_request(command);
}

void SwitchInfoImpl::delete_switch_qos_bandwidth_allocation(std::uint32_t bandwidth_percent,
                                                            std::uint32_t priority_group) {
    DeleteSwitchQosBandwidthAllocation command{bandwidth_percent, priority_group};

    EapiClient::get_instance()->eapi_request(command);
}
