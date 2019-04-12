/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/network_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/network_commands.hpp"
#include "hal/switch_port_info_impl.hpp"
#include "network_config.hpp"

#include "utils/lag.hpp"
#include "utils/port.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model::enums;
using namespace agent_framework::model;
using namespace agent_framework;
using namespace agent::network::hal;
using namespace agent::network::utils;

namespace {
    /*!
     * @brief Get default vlan
     *
     * @param[out] port EthernetSwitchPort model
     * */
    void get_default_vlan(EthernetSwitchPort& port) {
        auto network_components = NetworkComponents::get_instance();
        auto& port_vlan_manager = network_components->get_port_vlan_manager();
        if (PortClass::Logical == port.get_port_class()) {
            if (is_logical_port_empty(port.get_port_identifier())) {
                /* PVID is not supported on empty LAG */
                return;
            }
        }
        SwitchPortInfoImpl port_info(port.get_port_identifier());
        SwitchPortInfo::PortAttributeValue default_vid{};
        port_info.get_switch_port_attribute(SwitchPortInfo::DEFAULTVID,
                                            default_vid);
        for (const auto& uuid : port_vlan_manager.get_keys(port.get_uuid())) {
            const auto port_vlan = port_vlan_manager.get_entry(uuid);
            if (uint32_t(default_vid) == port_vlan.get_vlan_id()) {
                port.set_default_vlan(uuid);
            }
        }
    }

    /*!
     * @brief Get max frame size
     *
     * @param[out] port EthernetSwitchPort model
     * */
    void get_max_frame_size(EthernetSwitchPort& port) {
        if (PortClass::Logical == port.get_port_class()) {
            if (is_logical_port_empty(port.get_port_identifier())) {
                /* MAX is not supported on empty LAG */
                return;
            }
        }
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfoImpl port_info(port.get_port_identifier());
        port_info.get_switch_port_attribute(SwitchPortInfo::FRAMESIZE, value);
        if (value.is_set()) {
            port.set_frame_size(uint32_t(value));
        }
    }

    /*!
    * @brief Process collections
    *
    * @param[out] port EthernetSwitchPort model
    * */
    void process_collections(EthernetSwitchPort& port) {
        /* process Members collection */
        if (PortClass::Logical == port.get_port_class()) {
            port.add_collection({CollectionName::PortMembers,
                                CollectionType::PortMembers});
        }
    }

    /*!
     * @brief Get switch port attributes
     *
     * @param[out] port EthernetSwitchPort model
     * */
    void get_switch_port_attributes(EthernetSwitchPort& port) {
        /* get physical port info */
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfoImpl port_info(port.get_port_identifier());
        port_info.get_switch_port_attribute(SwitchPortInfo::MACADDRESS, value);
        if (value.is_set()) {
            port.set_mac_address(std::string(value));
        }
        port_info.get_switch_port_attribute(SwitchPortInfo::IPV4ADDRESS, value);
        if (value.is_set() && !std::string(value).empty()) {
            attribute::Ipv4Address ipv4{};
            ipv4.set_address(std::string(value));
            port_info.get_switch_port_attribute(SwitchPortInfo::IPV4SUBNETMASK,
                                                value);
            ipv4.set_subnet_mask(std::string(value));
            port.set_ipv4_address(ipv4);
        }
        port_info.get_switch_port_attribute(SwitchPortInfo::AUTOSENSE, value);
        if (value.is_set()) {
            port.set_auto_sense(bool(value));
        }
        auto duplex = port_info.get_switch_port_duplex();
        if (SwitchPortInfo::Duplex::UNKNOWN != duplex) {
            port.set_full_duplex(SwitchPortInfo::Duplex::FULL == duplex);
        }
    }

    /*!
     * @brief Get LAG attributes
     *
     * @param[out] port EthernetSwitchPort model
     * */
    void get_logical_port_attributes(EthernetSwitchPort& port) {
        port.set_mac_address({});
        port.set_neighbor_mac({});
        port.set_auto_sense(false);
    }

    /*!
     * @brief Get common port attributes
     *
     * @param[out] port EthernetSwitchPort model
     * */
    void get_common_attributes(EthernetSwitchPort& port) {
        /* get physical port info */
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfoImpl port_info(port.get_port_identifier());
        /* get port admin/operational state thru Netlink API */
        port_info.get_switch_port_attribute(SwitchPortInfo::ADMINISTRATIVESTATE,
                                            value);
        port.set_administrative_state(AdministrativeState::from_string(
                                            std::string(value)));
        port_info.get_switch_port_attribute(SwitchPortInfo::OPERATIONALSTATE,
                                            value);
        port.set_operational_state(OperationalState::from_string(
                                            std::string(value)));
        /* get port speed thru Netlink API */
        port_info.get_switch_port_attribute(SwitchPortInfo::LINKSPEEDMBPS,
                                            value);
        port.set_link_speed_mbps(value.get_number());
    }

    void get_port_info(const GetEthernetSwitchPortInfo::Request& request,
                       GetEthernetSwitchPortInfo::Response& response) {
        /* get port reference */
        auto network_components = NetworkComponents::get_instance();
        auto& port_manager = network_components->get_port_manager();
        auto port_model = port_manager.get_entry(request.get_uuid());
        if (PortClass::Logical == port_model.get_port_class()) {
            /* get logical port info */
            get_logical_port_attributes(port_model);
        }
        else {
            /* get physical port info */
            get_switch_port_attributes(port_model);
        }

        /* get common attributes */
        get_common_attributes(port_model);

        /* get default vlan */
        get_default_vlan(port_model);

        /* get max frame size */
        get_max_frame_size(port_model);

        /* process port collections */
        process_collections(port_model);

        /* set port model to responce */
        response = port_model;
    }
}

REGISTER_COMMAND(GetEthernetSwitchPortInfo, ::get_port_info);
