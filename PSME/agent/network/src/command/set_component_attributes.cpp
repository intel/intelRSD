/*!
 * @brief Implementation of SetComponentAttributes command.
 *
 * File contains all implementations of methods for SetComponentAttributes command.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file set_component_attributes.cpp
 */


#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"
#include "agent-framework/module/requests/validation/network.hpp"
#include "agent-framework/module/requests/validation/common.hpp"
#include "hal/switch_port_info_impl.hpp"
#include "hal/switch_info_impl.hpp"
#include "hal/switch_port_vlan.hpp"
#include "utils/lag.hpp"
#include "json-rpc/common.hpp"
#include <tuple>

using namespace agent_framework;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;

using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::requests::validation;
using namespace agent::network::hal;
using namespace agent::network::utils;

namespace {

using QosApplicationProtocolList = agent_framework::model::attribute::Array<attribute::QosApplicationProtocol>;
using QosPriorityGroupMappingList = agent_framework::model::attribute::Array<attribute::QosPriorityGroupMapping>;
using QosBandwidthAllocationList = agent_framework::model::attribute::Array<attribute::QosBandwidthAllocation>;

void set_switch_attributes(const std::string& uuid, const Attributes& attributes,
                           SetComponentAttributes::Response& response) {

    auto ethernet_switch = get_manager<EthernetSwitch>().get_entry(uuid);
    SwitchInfoImpl switch_info(ethernet_switch.get_switch_identifier());

    NetworkValidator::validate_set_switch_attributes(attributes);

    for (const auto& attribute_name : attributes.get_names()) {
        const auto& attribute_value = attributes.get_value(attribute_name);
        try {

            // PFC state
            if (literals::EthernetSwitch::PFC_ENABLED == attribute_name) {
                // Update PFC state on the switch
                switch_info.update_switch_pfc_enabled(ethernet_switch.get_pfc_enabled(), attribute_value.get<bool>());

                // Update PFC state in the model
                get_manager<EthernetSwitch>().get_entry_reference(uuid)->
                        set_pfc_enabled(attribute_value.get<bool>());
            }

            // LLDP state
            else if (literals::EthernetSwitch::LLDP_ENABLED == attribute_name) {
                // Update LLDP state on the switch
                switch_info.update_switch_lldp_enabled(ethernet_switch.get_lldp_enabled(), attribute_value.get<bool>());

                // Update LLDP state in the model
                get_manager<EthernetSwitch>().get_entry_reference(uuid)->
                        set_lldp_enabled(attribute_value.get<bool>());
            }

            // QoS Application Protocol
            else if (literals::EthernetSwitch::QOS_APPLICATION_PROTOCOL == attribute_name) {
                // Update QoS application protocol on the switch
                switch_info.update_switch_qos_application_protocol(
                        ethernet_switch.get_qos_application_protocol(),
                        QosApplicationProtocolList::from_json(attribute_value));

                // Update QoS application protocol in the model
                get_manager<EthernetSwitch>().get_entry_reference(uuid)->set_qos_application_protocol(
                        QosApplicationProtocolList::from_json(attribute_value));
            }

            // QoS Priority To Priority Group Mapping
            else if (literals::EthernetSwitch::QOS_PRIORITY_TO_PRIORITY_GROUP_MAPPING == attribute_name) {
                // Update QoS priority mapping on the switch
                switch_info.update_switch_qos_priority_group_mapping(
                        ethernet_switch.get_qos_priority_group_mapping(),
                        QosPriorityGroupMappingList::from_json(attribute_value));

                // Update QoS priority group mapping in the model
                get_manager<EthernetSwitch>().get_entry_reference(uuid)->set_qos_priority_group_mapping(
                        QosPriorityGroupMappingList::from_json(attribute_value));
            }

            // QoS Bandwidth Allocation
            else if (literals::EthernetSwitch::QOS_BANDWIDTH_ALLOCATION == attribute_name) {
                // Update QoS bandwidth allocation on the switch
                switch_info.update_switch_qos_bandwidth_allocation(
                        ethernet_switch.get_qos_bandwidth_allocation(),
                        QosBandwidthAllocationList::from_json(attribute_value));

                // Update QoS bandwidth allocation in the model
                get_manager<EthernetSwitch>().get_entry_reference(uuid)->set_qos_bandwidth_allocation(
                        QosBandwidthAllocationList::from_json(attribute_value));
            }

            else {
                /* no component found */
                THROW(UnsupportedField, "network-agent", "Setting attribute is not supported.",
                      attribute_name, attribute_value);
            }

            log_debug("network-agent", "Setting switch attributes [ attribute=" << attribute_name
                << " value=" << attributes.get_value(attribute_name)
                << " switch=" << get_manager<EthernetSwitch>().get_entry(uuid).get_switch_identifier() << " ]");
        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}

using PrioritiesList = agent_framework::model::attribute::Array<std::uint32_t>;
using DcbxState = agent_framework::model::enums::DcbxState;
using PortAttributeType = SwitchPortInfo::PortAttributeType;
using PortAttributeValue = SwitchPortInfo::PortAttributeValue;

// @TODO big cleanup and refactor after adding DCBX and LLDP
void set_port_attributes(const EthernetSwitchPort& port, const Attributes& attributes,
                         SetComponentAttributes::Response& response) {
    /* Make sure port is not an empty LAG */
    NetworkValidator::validate_set_port_attributes(port, attributes);

    auto network_components = NetworkComponents::get_instance();
    auto& port_vlan_manager = network_components->get_port_vlan_manager();
    SwitchPortInfoImpl port_info(port.get_port_identifier());

    for (const auto& attribute_name : attributes.get_names()) {
        const auto& attribute_value = attributes.get_value(attribute_name);
        log_debug("network-agent", "Setting port attributes, " << "port= " << port.get_port_identifier()
            << ", attribute=" << attribute_name << ", value=" << attribute_value << ".");
        try {
            PortAttributeValue value{};
            const auto type = port_info.get_port_attribute_type(attribute_name, attribute_value);

            value.set_value_from_json(attribute_value);

            /* Convert default VLAN UUID into VLAN id */
            if (type == PortAttributeType::DEFAULTVID) {
                const auto& vlan_uuid = attribute_value.get<std::string>();
                const auto& vlan_port = port_vlan_manager.get_entry(vlan_uuid);
                value.set(vlan_port.get_vlan_id());
            }

            log_debug("network-agent", "Setting port attributes [ attribute="
                << attribute_name << " port="
                << port.get_port_identifier() << " ]");

            if (type == PortAttributeType::PFC_ENABLED) {
                if (!port.get_pfc_enabled().has_value()) {
                    THROW(NetworkError, "network-agent",
                          std::string("PFC Enabled attribute can not be configured on ") +
                          std::string(port.get_port_identifier()) + std::string(" interface through eAPI. ") +
                          std::string("This interface has been recognized as inactive - ") +
                          std::string("check if the splitter cable is plugged in."));
                }

                // Update PFC state on the port
                port_info.update_switch_port_pfc_enabled(port.get_pfc_enabled(), attribute_value.get<bool>());

                // Update PFC state in the model
                get_manager<EthernetSwitchPort>().get_entry_reference(port.get_uuid())->
                    set_pfc_enabled(attribute_value.get<bool>());
            }

            // hal 'set_value' command does not support arrays of numbers so we need to repack and use dedicated method
            else if (type == PortAttributeType::PFC_ENABLED_PRIORITIES) {
                // Update PFC enabled priorities on the port
                port_info.update_switch_port_pfc_priorities(port.get_pfc_enabled_priorities(),
                                                            PrioritiesList::from_json(attribute_value));

                // Update PFC enabled priorities in the model
                get_manager<EthernetSwitchPort>().get_entry_reference(port.get_uuid())->
                    set_pfc_enabled_priorities(PrioritiesList::from_json(attribute_value));
            }

            else if (type == PortAttributeType::LLDP_ENABLED) {
                if (!port.get_lldp_enabled().has_value()) {
                    THROW(NetworkError, "network-agent",
                          std::string("LLDP Enabled attribute can not be configured on ") +
                          std::string(port.get_port_identifier()) + std::string(" interface through eAPI. ") +
                          std::string("This interface has been recognized as inactive - ") +
                          std::string("check if the splitter cable is plugged in."));
                }

                // Update LLDP state on the port
                port_info.update_switch_port_lldp_enabled(port.get_lldp_enabled(), attribute_value.get<bool>());

                // Update LLDP state in the model
                get_manager<EthernetSwitchPort>().get_entry_reference(port.get_uuid())->
                    set_lldp_enabled(attribute_value.get<bool>());
            }

            else if (type == PortAttributeType::DCBX_STATE) {
                if (!port.get_dcbx_state().has_value()) {
                    THROW(NetworkError, "network-agent",
                          std::string("DCBX State attribute can not be configured on ") +
                          std::string(port.get_port_identifier()) + std::string(" interface through eAPI. ") +
                          std::string("This interface has been recognized as inactive - ") +
                          std::string("check if the splitter cable is plugged in."));
                }

                const auto& dcbx_state = DcbxState::from_string(attribute_value.get<std::string>());

                // Update DCBX state on the port
                port_info.update_switch_port_dcbx_state(port.get_dcbx_state(), dcbx_state);

                // Update DCBX state in the model
                get_manager<EthernetSwitchPort>().get_entry_reference(port.get_uuid())->
                    set_dcbx_state(dcbx_state);
            }

            else {
                port_info.set_switch_port_attribute(type, value);
            }

        }
        catch (const GamiException& ex) {
            response.add_status({attribute_name, ex.get_error_code(), ex.get_message()});
        }
    }
}


void set_port_vlan_attributes(const EthernetSwitchPortVlan& port_vlan,
                              const attribute::Attributes& attributes,
                              SetComponentAttributes::Response&) {
    NetworkValidator::validate_set_port_vlan_attributes(attributes);
    SwitchPortVlan switch_port_vlan{};
    for (const auto& attribute_name : attributes.get_names()) {
        switch_port_vlan.set_attribute(port_vlan.get_uuid(), attribute_name,
                                       attributes.get_value(attribute_name));
    }
}


void set_component_attributes(const SetComponentAttributes::Request& request,
                              SetComponentAttributes::Response& response) {
    const auto& uuid = request.get_component();
    const auto& attributes = request.get_attributes();

    auto& switch_manager = get_manager<EthernetSwitch>();
    auto& port_manager = get_manager<EthernetSwitchPort>();
    auto& port_vlan_manager = get_manager<EthernetSwitchPortVlan>();

    /* check if this is switch attributes */
    if (switch_manager.entry_exists(uuid)) {
        set_switch_attributes(uuid, attributes, response);
        return;
    }

    /* check if this is switch port attributes */
    else if (port_manager.entry_exists(uuid)) {
        auto port = port_manager.get_entry(uuid);
        set_port_attributes(port, attributes, response);
        return;
    }

    /* check if this is VLAN component */
    else if (port_vlan_manager.entry_exists(uuid)) {
        const auto& vlan_port = port_vlan_manager.get_entry(uuid);
        log_debug("network-agent", "Setting vlan port attributes "
            << "[ vlan=" << vlan_port.get_vlan_id() << " uuid=" << uuid << " ]");
        set_port_vlan_attributes(vlan_port, attributes, response);
    }

    else {
        /* no component found */
        THROW(InvalidUuid, "network-agent", "Component not found, UUID = '" + uuid + "'.");
    }
}
}

REGISTER_COMMAND(SetComponentAttributes, ::set_component_attributes);
