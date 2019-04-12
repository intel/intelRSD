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
 * @file agent/network/src/hal/switch_info_impl.cpp
 */

#include "hal/switch_info_impl.hpp"
#include "agent-framework/module/network_components.hpp"

using namespace agent_framework;
using namespace agent_framework::module;
using namespace agent_framework::model;

using namespace agent::network::hal;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;

SwitchInfoImpl::SwitchInfoImpl(const std::string& mgmt_port) {
    set_is_enabled(true);
    set_management_port(mgmt_port);
}

SwitchInfoImpl::~SwitchInfoImpl() {}

void SwitchInfoImpl::read_switch_info() {
    read_mgmt_mac_address();
}

bool SwitchInfoImpl::switch_vlan_exists(VlanId vlan_id) {
    auto& port_manager = get_manager<EthernetSwitchPort>();
    auto& port_vlan_manager = get_manager<EthernetSwitchPortVlan>();

    /* check if the VLAN id exists on any port */
    for (const auto& port_uuid : port_manager.get_keys()) {
        for (const auto& vlan_uuid : port_vlan_manager.get_keys(port_uuid)) {
            auto port_vlan_module = port_vlan_manager.get_entry(vlan_uuid);
            if (vlan_id == port_vlan_module.get_vlan_id()) {
                return true;
            }
        }
    }

    return false;
}

void SwitchInfoImpl::update_switch_pfc_enabled(bool current_pfc_state, bool new_pfc_state) {

    if (current_pfc_state == new_pfc_state) {
        return;
    }

    try {
        set_switch_pfc_enabled(new_pfc_state);
    }
    catch (json_rpc::JsonRpcException& e) {
        THROW(NetworkError, "network-agent", "eAPI exception on setting switch QoS PFC state: " +
                                             std::string(e.what()));
    }
}

void SwitchInfoImpl::update_switch_lldp_enabled(bool current_lldp_state, bool new_lldp_state) {

    if (current_lldp_state == new_lldp_state) {
        return;
    }

    try {
        set_switch_lldp_enabled(new_lldp_state);
    }
    catch (json_rpc::JsonRpcException& e) {
        THROW(NetworkError, "network-agent", "eAPI exception on setting switch QoS LLDP state: " +
                                             std::string(e.what()));
    }
}

using QosApplicationProtocolList = agent_framework::model::attribute::Array<attribute::QosApplicationProtocol>;

void SwitchInfoImpl::update_switch_qos_application_protocol(
    const QosApplicationProtocolList& current_application_mappings,
    const QosApplicationProtocolList& new_application_mappings) {

    // Delete the application protocols from the switch,
    // if they do not exist on the list of new attributes in the request
    for (const auto& switch_qos : current_application_mappings) {
        auto found = false;

        for (const auto& request_qos : new_application_mappings) {
            // Check if the application protocol configured on the switch,
            // also exists in the request ...
            if (switch_qos.get_protocol() == request_qos.get_protocol() &&
                switch_qos.get_port() == request_qos.get_port()) {
                found = true;
                break;
            }
        }

        if (!found) {
            try {
                // ... if not, delete the application protocol from the switch.
                delete_switch_qos_application_protocol(
                    switch_qos.get_protocol(), switch_qos.get_port(), switch_qos.get_priority());
            }
            catch (json_rpc::JsonRpcException& e) {
                THROW(NetworkError, "network-agent", "eAPI exception on deleting switch QoS application protocol: " +
                                                     std::string(e.what()));
            }
        }
    }

    // Add or update the application protocols on the switch,
    // if they do not exist in the switch or their application protocols need to be updated.
    for (const auto& request_qos : new_application_mappings) {
        auto found = false;

        // Check if the given application protocol already exists on the switch,
        // and its priority is the same like in the request ...
        for (const auto& switch_qos : current_application_mappings) {
            if (switch_qos.get_protocol() == request_qos.get_protocol() &&
                switch_qos.get_port() == request_qos.get_port() &&
                switch_qos.get_priority() == request_qos.get_priority()) {
                found = true;
                break;
            }
        }

        if (!found) {
            try {
                // ... if not, add or update the application protocol on the switch.
                add_switch_qos_application_protocol(
                    request_qos.get_protocol(), request_qos.get_port(), request_qos.get_priority());
            }
            catch (json_rpc::JsonRpcException& e) {
                THROW(NetworkError, "network-agent", "eAPI exception on adding switch QoS application protocol: " +
                                                     std::string(e.what()));
            }
        }
    }
}

using QosPriorityGroupMappingList = agent_framework::model::attribute::Array<attribute::QosPriorityGroupMapping>;

void SwitchInfoImpl::update_switch_qos_priority_group_mapping(
    const QosPriorityGroupMappingList& current_priority_mappings,
    const QosPriorityGroupMappingList& new_priority_mappings) {

    // Delete the priority group mappings from the switch,
    // if they do not exist on the list of new attributes in the request
    for (const auto& switch_qos : current_priority_mappings) {
        auto found = false;

        for (const auto& request_qos : new_priority_mappings) {
            // Check if the priority configured on the switch,
            // and also exists in the new request ...
            if (switch_qos.get_priority() == request_qos.get_priority()) {
                found = true;
                break;
            }
        }

        if (!found) {
            try {
                // ... if not, delete the priority group mapping from the switch.
                delete_switch_qos_priority_group_mapping(switch_qos.get_priority(), switch_qos.get_priority_group());
            }
            catch (json_rpc::JsonRpcException& e) {
                THROW(NetworkError, "network-agent", "eAPI exception on deleting switch QoS priority group mapping: " +
                                                     std::string(e.what()));
            }
        }
    }

    // Add or update the priority group mapping on the switch,
    // if they do not exist in the switch or their priority group attributes need to be updated.
    for (const auto& request_qos : new_priority_mappings) {
        auto found = false;

        // Check if the given priority group mapping already exists on the switch,
        // and its priority group is the same like in the request ...
        for (const auto& switch_qos : current_priority_mappings) {
            if (switch_qos.get_priority() == request_qos.get_priority() &&
                switch_qos.get_priority_group() == request_qos.get_priority_group()) {
                found = true;
                break;
            }
        }

        if (!found) {
            try {
                // ... if not, add or update the priority group mapping on the switch.
                add_switch_qos_priority_group_mapping(request_qos.get_priority(), request_qos.get_priority_group());
            }
            catch (json_rpc::JsonRpcException& e) {
                THROW(NetworkError, "network-agent", "eAPI exception on adding switch QoS priority group mapping: " +
                                                     std::string(e.what()));
            }
        }
    }
}

using QosBandwidthAllocationList = agent_framework::model::attribute::Array<attribute::QosBandwidthAllocation>;

void SwitchInfoImpl::update_switch_qos_bandwidth_allocation(
    const QosBandwidthAllocationList& current_bandwidth_allocations,
    const QosBandwidthAllocationList& new_bandwidth_allocations) {

    // Delete the bandwidth allocations from the switch,
    // if they do not exist on the list of new attributes in the request
    for (const auto& switch_qos : current_bandwidth_allocations) {
        auto found = false;

        for (const auto& request_qos : new_bandwidth_allocations) {
            // Check if the priority group configured on the switch,
            // and also exists in the new request ...
            if (switch_qos.get_priority_group() == request_qos.get_priority_group()) {
                found = true;
                break;
            }
        }

        if (!found) {
            try {
                // ... if not, delete the bandwidth allocation from the switch.
                delete_switch_qos_bandwidth_allocation(switch_qos.get_bandwidth_percent(), switch_qos.get_priority_group());
            }
            catch (json_rpc::JsonRpcException& e) {
                THROW(NetworkError, "network-agent", "eAPI exception on deleting switch QoS bandwidth allocation: " +
                                                     std::string(e.what()));
            }
        }
    }

    // Add or update the bandwidth allocations on the switch,
    // if they do not exist on the switch or their bandwidth attributes need to be updated.
    for (const auto& request_qos : new_bandwidth_allocations) {
        auto found = false;

        // Check if the given bandwidth allocation already exists on the switch,
        // and its bandwidth is the same like in the request ...
        for (const auto& switch_qos : current_bandwidth_allocations) {
            if (switch_qos.get_priority_group() == request_qos.get_priority_group() &&
                switch_qos.get_bandwidth_percent() == request_qos.get_bandwidth_percent()) {
                found = true;
                break;
            }
        }

        if (!found) {
            try {
                // ... if not, add or update the bandwidth allocation on the switch.
                add_switch_qos_bandwidth_allocation(request_qos.get_bandwidth_percent(), request_qos.get_priority_group());
            }
            catch (json_rpc::JsonRpcException& e) {
                THROW(NetworkError, "network-agent", "eAPI exception on adding switch QoS bandwidth allocation: " +
                                                     std::string(e.what()));
            }
        }
    }
}