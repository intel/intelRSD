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
 * */

#include "agent-framework/command/network/get_ethernet_switch_port_info.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "hw/fm10000/network_controller_manager.hpp"
#include "hw/fm10000/network_controller.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "api/dcrp/remote_switch_info.hpp"
#include "network_config.hpp"

using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::model::enums;
using namespace agent::network::api::netlink;
using namespace agent::network::api::dcrp;
using namespace agent::network::hw::fm10000;

namespace fm10000 {

using namespace agent_framework;

/*! Network command GetEthernetSwitchPortInfo */
class GetEthernetSwitchPortInfo final : public command::network::GetEthernetSwitchPortInfo {

public:
    /*! Command constructor */
    GetEthernetSwitchPortInfo() { }

    using network::GetEthernetSwitchPortInfo::execute;

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    void execute(const Request& request, Response& response) {
        /* get port reference */
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto port_model = port_manager.get_entry(request.get_port());

        /* get neighbor info */
        get_neighbor_info(port_model);

        if (PortClass::Logical == port_model.get_port_class()) {
            /* get physical port info */
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
        response.set_port(port_model);
    }

    /*! Command destructor */
    ~GetEthernetSwitchPortInfo();

    /*!
     * @brief Get neighbor info
     *
     * @param[out] port Port model
     * */
    void get_neighbor_info(SwitchPort& port) {
        string switch_identifier{};
        string port_identifier{};
        auto neighbor_info = port.get_neighbor_info();
        RemoteSwitchInfo remote_switch_info{};
        try {
            switch_identifier =
                remote_switch_info.get_neighbor_on_port(port.get_parent_uuid(),
                                                        port.get_port_identifier());
        }
        catch (std::runtime_error& err) {
            /* DCRP is not enabled, continue */
            log_warning(GET_LOGGER("fm10000"), "Getting remote switch info failed");
            log_debug(GET_LOGGER("fm10000"), err.what());
        }
        if (!switch_identifier.empty()) {
            neighbor_info.set_switch_identifier(switch_identifier);
        }
        /* get neighbor port identifier */
        if (PortType::MeshPort == port.get_port_type()) {
            SwitchPortInfo::PortAttributeValue value{};
            SwitchPortInfo port_info(port.get_port_identifier());
            port_info.get_switch_port_attribute(SwitchPortInfo::NEIGHBORINFO, value);
            if (!value.get_string().empty()) {
                neighbor_info.set_port_identifier(string(value));
            }
        }
        /* update port neighbor info */
        port.set_neighbor_info(neighbor_info);
    }

    /*!
     * @brief Get default vlan
     *
     * @param[out] port Port model
     * */
    void get_default_vlan(SwitchPort& port) {
        auto controller = NetworkControllerManager::get_network_controller();
        auto network_manager = NetworkManager::get_instance();
        auto& port_vlan_manager = network_manager->get_port_vlan_manager();
        if (controller->is_member_port(port.get_port_identifier())) {
            /* PVID is not supported on member ports */
            return;
        }
        if (PortClass::Logical == port.get_port_class()) {
            if (controller->is_logical_port_empty(port.get_port_identifier())) {
                /* PVID is not supported on empty LAG */
                return;
            }
        }
        SwitchPortInfo port_info(port.get_port_identifier());
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
     * @param[out] port Port model
     * */
    void get_max_frame_size(SwitchPort& port) {
        auto controller = NetworkControllerManager::get_network_controller();
        if (PortClass::Logical == port.get_port_class()) {
            if (controller->is_logical_port_empty(port.get_port_identifier())) {
                /* MAX is not supported on empty LAG */
                return;
            }
        }
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfo port_info(port.get_port_identifier());
        port_info.get_switch_port_attribute(SwitchPortInfo::FRAMESIZE, value);
        port.set_frame_size(uint32_t(value));
    }

    /*!
     * @brief Process collection
     *
     * @param[out] port Port model
     * */
    void process_collections(SwitchPort& port) {
        /* process Members collection */
        if (PortClass::Logical == port.get_port_class()) {
            port.add_collection({CollectionName::PortMembers,
                                CollectionType::PortMembers, ""});
        }
    }

    /*!
     * @brief Get switch port attributes
     *
     * @param[out] port Port model
     * */
    void get_switch_port_attributes(SwitchPort& port) {
        /* get physical port info */
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfo port_info(port.get_port_identifier());
        if (LinkTechnology::PCIe == port.get_link_technology()) {
            /* attributes specific for PCIe port */
            port_info.get_switch_port_attribute(SwitchPortInfo::MACADDRESS,
                                                value);
            port.set_mac_address(string(value));
            port.set_neighbor_mac(string(value));
            port.set_auto_sense(false);
        }
        else {
            /* attributes specific for non-PCIe port */
            port_info.get_switch_port_attribute(SwitchPortInfo::AUTOSENSE,
                                                value);
            port.set_auto_sense(bool(value));
            port.set_mac_address({});
            port.set_neighbor_mac({});
        }
    }

    /*!
     * @brief Get LAG attributes
     *
     * @param[out] port Port model
     * */
    void get_logical_port_attributes(SwitchPort& port) {
        port.set_mac_address({});
        port.set_neighbor_mac({});
        port.set_auto_sense(false);
    }

    /*!
     * @brief Get common port attributes
     *
     * @param[out] port Port model
     * */
    void get_common_attributes(SwitchPort& port) {
        /* get physical port info */
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfo port_info(port.get_port_identifier());
        /* get port admin/operational state thru Netlink API */
        port_info.get_switch_port_attribute(SwitchPortInfo::ADMINISTRATIVESTATE,
                                            value);
        port.set_administrative_state(AdministrativeState::from_string(
                                            string(value)));
        port_info.get_switch_port_attribute(SwitchPortInfo::OPERATIONALSTATE,
                                            value);
        port.set_operational_state(OperationalState::from_string(
                                            string(value)));
        /* get port speed thru Netlink API */
        port_info.get_switch_port_attribute(SwitchPortInfo::LINKSPEEDMBPS,
                                            value);
        port.set_link_speed_mbps(value.get_number());
    }
};

GetEthernetSwitchPortInfo::~GetEthernetSwitchPortInfo() { }

}

static Command::Register<fm10000::GetEthernetSwitchPortInfo> g("fm10000");
