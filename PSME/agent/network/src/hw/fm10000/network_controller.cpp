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
 *
 * @file hw/fm10000/network_controller.cpp
 *
 * @brief Implementation of network interface
 * */
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include "hw/fm10000/network_controller.hpp"
#include "logger/logger_factory.hpp"
#include "api/netlink/sysfs.hpp"
#include "api/netlink/vlan_message.hpp"
#include "api/netlink/port_message.hpp"
#include "api/netlink/switch_port_info.hpp"
#include "api/netlink/switch_info.hpp"
#include "api/acl.hpp"
#include "api/static_mac.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace agent::network::hw::fm10000;
using namespace agent::network::api::netlink;
using namespace agent::network;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::exceptions;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;

NetworkController::NetworkController() {}

NetworkController::~NetworkController() {}

void NetworkController::initialize() {
    // If initialized do not do this again.
    if(is_initialized()) {
        return;
    }
    /* get module references */
    auto network_manager = NetworkManager::get_instance();
    auto& module_manager = network_manager->get_module_manager();
    auto& switch_manager = network_manager->get_switch_manager();
    /* get & update manager module info */
    std::string manager_uuid{};
    if (module_manager.get_entry_count()) {
        manager_uuid = module_manager.get_keys().front();
        auto manager = module_manager.get_entry_reference(manager_uuid);
        SerialConsole serial{manager->get_serial_console()};
        serial.set_bitrate(115200);
        serial.set_data_bits(8);
        serial.set_flow_control(SerialConsoleFlowControl::None);
        serial.set_parity(SerialConsoleParity::None);
        serial.set_pin_out(SerialConsolePinOut::Cisco);
        serial.set_stop_bits(1);
        serial.set_signal_type(SerialConsoleSignalType::Rs232);
        manager->set_serial_console(std::move(serial));
        manager->add_collection({CollectionName::Switches,
                                CollectionType::EthernetSwitches, ""});
        manager->add_collection({CollectionName::Chassis,
                                CollectionType::Chassis, ""});
    }
    else {
        log_debug(GET_LOGGER("fm10000"), "No Manager module configured");
        return;
    }
    /* update switch module info */
    if (switch_manager.get_entry_count(manager_uuid)) {
        std::string switch_uuid{switch_manager.get_keys(manager_uuid).front()};
        auto switch_module = switch_manager.get_entry_reference(switch_uuid);
        /* TODO how do we know the location ? */
        switch_module->set_location(1);
        switch_module->add_collection({CollectionName::Ports,
                                      CollectionType::Ports, ""});
        switch_module->add_collection({CollectionName::Neighbors,
                                      CollectionType::NeighborSwitches, ""});
        switch_module->add_collection({CollectionName::Acls,
                                      CollectionType::Acls, ""});
        /* create port modules */
        init_switch_ports(switch_uuid);
        /* create port vlan modules */
        init_switch_vlan_ports();
        /* create ACL modules */
        init_acls(switch_uuid);
        /* create static MACs modules */
        init_static_macs(switch_uuid);
    }
    /* show network manager info */
    show_network_manager_info();

    log_debug(GET_LOGGER("fm10000"), "Switch is successfully initialized.");
    m_is_initialized = true;
}

void NetworkController::deinitialize() {
    // If not initialized do not deinitialize.
    if(!is_initialized()) {
        return;
    }

    m_is_initialized = false;
}

bool NetworkController::is_initialized()  {
    return m_is_initialized;
}

void NetworkController::init_switch_ports(const std::string& switch_uuid) {
    SysFs sysfs{};

    /* create port map list */
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    try {
        const auto& port_list = sysfs.get_port_list();
        for (const auto& port_identifier : port_list) {
            string port_uuid{};
            if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                log_debug(GET_LOGGER("fm10000"), "Updating port " +
                                                            port_identifier);
                auto port = port_manager.get_entry_reference(port_uuid);
                /* add port collections */
                port->add_collection({CollectionName::Vlans,
                                      CollectionType::PortVlans, ""});
                port->add_collection({CollectionName::PortAcls,
                                      CollectionType::Acls, ""});
                port->add_collection({CollectionName::StaticMacs,
                                      CollectionType::StaticMacs, ""});
                /* update port link technology attribute */
                if (LinkTechnology::Unknown == port->get_link_technology()) {
                    port->set_link_technology(LinkTechnology::Ethernet);
                }
                /* update port type attribute */
                if (PortType::Unknown == port->get_port_type()) {
                    port->set_port_type(PortType::Upstream);
                }
                /* update port static attributes */
                port->set_port_class(PortClass::Physical);
                port->set_port_mode(PortMode::Unknown);
                port->set_vlan_enable(true);
                port->set_status({State::Enabled, Health::OK});
            }
            else {
                log_debug(GET_LOGGER("fm10000"), "Adding port " +
                                                            port_identifier);
                add_physical_port(switch_uuid, port_identifier);
            }
        }
    }
    catch (const std::runtime_error& error) {
        log_error(GET_LOGGER("fm10000"), error.what());
        throw;
    }

    /* create lag map list */
    try {
        const auto lag_list = sysfs.get_team_list();
        for (const auto& lag_identifier : lag_list) {
            log_debug(GET_LOGGER("fm10000"), "Adding LAG " + lag_identifier);
            add_logical_port(lag_identifier, switch_uuid);
        }
    }
    catch (const std::runtime_error& error) {
        log_error(GET_LOGGER("fm10000"), error.what());
        throw;
    }
}

void NetworkController::init_switch_vlan_port(const std::string& port_identifier) {
    SwitchPortInfo port_info{port_identifier};
    auto network_manager = NetworkManager::get_instance();
    auto& port_vlan_manager = network_manager->get_port_vlan_manager();
    for (const auto& vlan : port_info.get_vlans()) {
        std::string port_uuid{};
        get_port_uuid_by_identifier(port_identifier, port_uuid);
        PortVlan portvlan_model{port_uuid};
        portvlan_model.set_vlan_id(vlan.get_vlan_id());
        portvlan_model.set_tagged(vlan.get_vlan_tag());
        portvlan_model.set_vlan_enable(true);
        portvlan_model.set_status({
            enums::State::Enabled,
            enums::Health::OK
        });
        port_vlan_manager.add_entry(std::move(portvlan_model));
        log_debug(GET_LOGGER("fm10000"), "Adding port-vlan ["
                  << "port=" << port_identifier
                  << " vlan=" << vlan.get_vlan_id()
                  << " tag=" << vlan.get_vlan_tag()
                  << "]");
    }
}

void NetworkController::init_switch_vlan_ports() {
    /* add port-vlan pair to the network manager */
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    for (const auto& port_uuid : port_manager.get_keys()) {
        auto port = port_manager.get_entry(port_uuid);
        if (is_member_port(port.get_port_identifier())) {
            /* getting vlan list isn't allowed on member port */
            continue;
        }
        if (PortClass::Logical == port.get_port_class()) {
            if (is_logical_port_empty(port.get_port_identifier())) {
                /* it's not possible to get vlan list on empty lag */
                continue;
            }
        }
        /* get vlan list on the port */
        init_switch_vlan_port(port.get_port_identifier());
    }
}

namespace {
    static inline void init_forward_port (const string port_identifier,
                                          AclRule& model_rule,
                                          const NetworkController& controller) {
        if (!port_identifier.empty()) {
            string port_uuid{};
            if (controller.get_port_uuid_by_identifier(port_identifier,
                                                       port_uuid)) {
                model_rule.set_forward_mirror_port(port_uuid);
            }
        }
    }

    static inline void init_mirrored_ports (const api::Acl::Strings& ports,
                                            AclRule& model_rule,
                                            const NetworkController& controller) {
        AclRule::MirroredPorts port_uuids{};
        for (const auto& port_identifier : ports) {
            string port_uuid{};
            if (controller.get_port_uuid_by_identifier(port_identifier,
                                                       port_uuid)) {
                port_uuids.add_entry(port_uuid);
            }
        }
        model_rule.set_mirrored_ports(port_uuids);
    }

    static inline void init_acl_action (const api::Acl::Rule& rule,
                                        AclRule& model_rule) {
        switch(rule.get_action()) {
            case api::Acl::ActionType::ACTION_DENY:
                model_rule.set_action(enums::AclAction::Deny);
                break;
            case api::Acl::ActionType::ACTION_PERMIT:
                model_rule.set_action(enums::AclAction::Permit);
                break;
            case api::Acl::ActionType::ACTION_FORWARD:
                model_rule.set_action(enums::AclAction::Forward);
                break;
            case api::Acl::ActionType::ACTION_MIRROR:
                model_rule.set_action(enums::AclAction::Mirror);
                break;
            default:
                break;
        }
    }

    static inline void init_acl_mirror_type (const api::Acl::Rule& rule,
                                             AclRule& model_rule) {
        switch(rule.get_mirror_type()) {
            case api::Acl::MirrorType::MIRROR_INGRESS:
                model_rule.set_mirror_type(enums::AclMirrorType::Ingress);
                break;
            case api::Acl::MirrorType::MIRROR_EGRESS:
                model_rule.set_mirror_type(enums::AclMirrorType::Egress);
                break;
            case api::Acl::MirrorType::MIRROR_BIDIRECTIONAL:
                model_rule.set_mirror_type(enums::AclMirrorType::Bidirectional);
                break;
            case api::Acl::MirrorType::MIRROR_REDIRECT:
                model_rule.set_mirror_type(enums::AclMirrorType::Redirect);
                break;
            case api::Acl::MirrorType::MIRROR_NA:
            default:
                break;
        }
    }

    static inline void get_condition_ip(const api::Acl::Condition& condition,
                                        AclRule::Ip& ip) {
        in_addr addr{};
        addr.s_addr = uint32_t(condition.value);
        ip.set_address(inet_ntoa(addr));
        if (condition.mask) {
            addr.s_addr = uint32_t(condition.mask);
            ip.set_mask(inet_ntoa(addr));
        }
    }

    static inline void get_condition_mac(const api::Acl::Condition& condition,
                                         AclRule::Mac& mac) {
        uint8_t octets[ETH_ALEN]{};
        std::memcpy(octets, &condition.value, ETH_ALEN);
        std::reverse(octets, octets + ETH_ALEN);
        mac.set_address(SwitchInfo::format_mac_address(octets));
        if (condition.mask) {
            std::memcpy(octets, &condition.mask, ETH_ALEN);
            std::reverse(octets, octets + ETH_ALEN);
            mac.set_mask(SwitchInfo::format_mac_address(octets));
        }
    }

    static inline void get_condition_vlan(const api::Acl::Condition& condition,
                                          AclRule::VlanId& vlan_id) {
        vlan_id.set_id(uint32_t(condition.value));
        if (condition.mask) {
            vlan_id.set_mask(uint32_t(condition.mask));
        }
    }

    static inline void get_condition_port(const api::Acl::Condition& condition,
                                          AclRule::Port& port) {
        port.set_port(uint32_t(condition.value));
        if (condition.mask) {
            port.set_mask(uint32_t(condition.mask));
        }
    }

    static inline void init_acl_conditions (const api::Acl::Rule& rule,
                                            AclRule& model_rule) {
        for (const auto& condition : rule.get_conditions()) {
            AclRule::Ip ip{};
            AclRule::Mac mac{};
            AclRule::VlanId vlan_id{};
            AclRule::Port port{};
            switch(condition.type) {
                case api::Acl::ConditionType::CONDITION_IP_SRC:
                    get_condition_ip(condition, ip);
                    model_rule.set_source_ip(ip);
                    break;
                case api::Acl::ConditionType::CONDITION_IP_DEST:
                    get_condition_ip(condition, ip);
                    model_rule.set_destination_ip(ip);
                    break;
                case api::Acl::ConditionType::CONDITION_MAC_SRC:
                    get_condition_mac(condition, mac);
                    model_rule.set_source_mac(mac);
                    break;
                case api::Acl::ConditionType::CONDITION_MAC_DEST:
                    get_condition_mac(condition, mac);
                    model_rule.set_destination_mac(mac);
                    break;
                case api::Acl::ConditionType::CONDITION_VLAN:
                    get_condition_vlan(condition, vlan_id);
                    model_rule.set_vlan_id(vlan_id);
                    break;
                case api::Acl::ConditionType::CONDITION_PROTOCOL:
                    model_rule.set_protocol(condition.value);
                    break;
                case api::Acl::ConditionType::CONDITION_L4_PORT_SRC:
                    get_condition_port(condition, port);
                    model_rule.set_source_port(port);
                    break;
                case api::Acl::ConditionType::CONDITION_L4_PORT_DEST:
                    get_condition_port(condition, port);
                    model_rule.set_destination_port(port);
                    break;
                default:
                    break;
            }
        }
    }

    static inline void init_acl_rule (const api::Acl::Rule& rule,
                                      AclRule& model_rule,
                                      const NetworkController& controller) {
        model_rule.set_rule_id(rule.get_rule_id());
        init_acl_action(rule, model_rule);
        init_forward_port(rule.get_destination_port(), model_rule, controller);
        init_mirrored_ports(rule.get_mirrored_ports(), model_rule, controller);
        init_acl_mirror_type(rule, model_rule);
        init_acl_conditions(rule, model_rule);
    }

    static inline void init_acl_rules (const string& acl_uuid,
                                       const api::Acl::Rules& rules,
                                       const NetworkController& controller) {
        auto network_manager = NetworkManager::get_instance();
        auto& acl_rule_manager = network_manager->get_acl_rule_manager();
        for (const auto& rule : rules) {
            AclRule model_rule{acl_uuid};
            init_acl_rule(rule, model_rule, controller);
            model_rule.set_status({enums::State::Enabled, enums::Health::OK});
            acl_rule_manager.add_entry(std::move(model_rule));
        }
    }

    static inline void read_acl_rules(const string& acl_uuid,
                                      NetworkController& controller) {
        api::Acl acl{};

        try {
            acl.read_acl_rule_list(controller.get_acl_name(acl_uuid));
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("fm10000"),
                      string("Failed reading ACL rule list from switch: ") +
                      e.what());
            return;
        }
        init_acl_rules(acl_uuid, acl.get_rules(), controller);
    }

    static inline void init_acls_with_ports (const string& switch_uuid,
                                             api::Acl& acl,
                                             NetworkController& controller) {
        auto network_manager = NetworkManager::get_instance();
        auto& acl_manager = network_manager->get_acl_manager();
        auto& acl_port_manager = network_manager->get_acl_port_manager();
        for (const auto& acl_name : acl.get_acls()) {
            Acl acl_model(switch_uuid);
            const auto acl_uuid = acl_model.get_uuid();
            acl_model.set_status({enums::State::Enabled,
                                 enums::Health::OK});
            acl_model.add_collection({CollectionName::AclRules,
                                      CollectionType::Rules, ""});
            acl_model.add_collection({CollectionName::AclPorts,
                                      CollectionType::Ports, ""});
            // keep ACL UUID -> ACL name mapping
            controller.add_acl_name(acl_uuid, acl_name);
            acl_manager.add_entry(std::move(acl_model));
            for (const auto& port_identifier : acl.get_bound_ports(acl_name)) {
                string port_uuid{};
                if (controller.get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                    acl_port_manager.add_entry(acl_uuid, port_uuid, "");
                }
            }
            read_acl_rules(acl_uuid, controller);
        }
    }

    void add_static_macs_to_model(const string& port_uuid,
                                  const api::StaticMac::Table& macs) {
        auto network_manager = NetworkManager::get_instance();
        auto& static_mac_manager = network_manager->get_static_mac_manager();
        for (const auto& pair : macs) {
            StaticMac static_mac{port_uuid};
            static_mac.set_address(pair.first);
            static_mac.set_vlan_id(pair.second);
            static_mac.set_status({enums::State::Enabled, enums::Health::OK});
            static_mac_manager.add_entry(static_mac);
        }
    }

    void init_static_macs_on_port(const string& port_uuid) {
        auto network_manager = NetworkManager::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        auto port = port_manager.get_entry(port_uuid);
        try {
            api::StaticMac::Table macs{};
            api::StaticMac::get_macs(port.get_port_identifier(), macs);
            add_static_macs_to_model(port_uuid, macs);
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("fm10000"),
                      string("Failed reading static MAC list on port ")
                      + port_uuid + " : " + e.what());
        }
    }
}

void NetworkController::init_static_macs(const std::string& switch_uuid) {
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    for (const auto& port_uuid : port_manager.get_keys(switch_uuid)) {
        ::init_static_macs_on_port(port_uuid);
    }
}

void NetworkController::init_acls(const std::string& switch_uuid) {
    api::Acl acl{};

    try {
        acl.read_acl_list();
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("fm10000"),
                  string("Failed reading ACL list from switch: ") + e.what());
        return;
    }
    init_acls_with_ports(switch_uuid, acl, *this);
}

NetworkController::UUID
NetworkController::add_logical_port(const std::string& lag_identifier,
                    const std::string& switch_uuid) {
    /* add port into network manager */
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    SwitchPort port_model{switch_uuid};
    port_model.add_collection({CollectionName::Vlans,
                               CollectionType::PortVlans, ""});
    port_model.add_collection({CollectionName::PortAcls,
                               CollectionType::Acls, ""});
    port_model.add_collection({CollectionName::StaticMacs,
                               CollectionType::StaticMacs, ""});
    port_model.set_link_technology(LinkTechnology::Ethernet);
    port_model.set_port_class(PortClass::Logical);
    port_model.set_port_type(PortType::Upstream);
    port_model.set_port_mode(PortMode::LinkAggregationStatic);
    port_model.set_port_identifier(lag_identifier);
    port_model.set_vlan_enable(true);
    port_model.set_status({
        enums::State::Enabled,
        enums::Health::OK
    });
    port_manager.add_entry(port_model);
    return port_model.get_uuid();
}

vector<string> NetworkController::get_logical_port_members(
                            const string& lag_identifier) const {
    SwitchPortInfo port_info{lag_identifier};
    return port_info.get_port_members();
}

bool NetworkController::is_logical_port_empty(
                            const string& lag_identifier) const {
    SwitchPortInfo port_info{lag_identifier};
    return port_info.get_port_members().size() == 0;
}

bool NetworkController::is_member_port(const string& port_identifier) const {
    SwitchPortInfo port_info{port_identifier};
    return port_info.is_member();
}

string NetworkController::get_lag_identifier(const string& port_identifier) const {
    SwitchPortInfo port_info{port_identifier};
    return port_info.get_master_port();
}

void NetworkController::
validate_port_members(const UUIDList& ports, int lag_port_speed)
{
    auto network_manager = NetworkManager::get_instance();
    auto& port_manager = network_manager->get_port_manager();
    int speed{lag_port_speed};

    for (const auto& port_uuid : ports) {
        /* check that member port is present */
        if (!port_manager.entry_exists(port_uuid)) {
            THROW(InvalidUuid, "fm10000",
                string("Member port ") + port_uuid + " not found");
        }

        /* get switch port instance */
        const auto port = port_manager.get_entry(port_uuid);

        /* check if port is physical */
        if (PortClass::Logical == port.get_port_class()) {
            THROW(InvalidParameters, "fm10000", "Member port cannot be a Lag.");
        }

        /* check if port is not master of another network device. */
        if (is_member_port(port.get_port_identifier())) {
            THROW(InvalidParameters, "fm10000",
                        "Port is already added to another Lag.");
        }

        /* PCIe ports cannot participate in Link Aggregation. */
        if (LinkTechnology::PCIe == port.get_link_technology()) {
            THROW(InvalidParameters, "fm10000",
                        "PCIe port are not supported on Lag.");
        }

        /* DCRP mesh ports cannot be part of the LAG */
        if (PortType::MeshPort == port.get_port_type()) {
            THROW(InvalidParameters, "fm10000",
                  string("Mesh ports are not allowed on Lag: ") +
                  port.get_port_identifier());
        }

        /* check speed for all ports to be the same */
        SwitchPortInfo::PortAttributeValue value{};
        SwitchPortInfo switch_port_info(port.get_port_identifier());
        switch_port_info.get_switch_port_attribute(
                                        SwitchPortInfo::LINKSPEEDMBPS, value);
        if (INVALID_PORT_SPEED == speed) {
            speed = int(value);
        }
        else if (speed != int(value)) {
            THROW(InvalidParameters, "fm10000",
                        "Speed should be same for all members of the Lag.");
        }
    }
}
