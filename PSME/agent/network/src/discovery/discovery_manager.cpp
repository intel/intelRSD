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
 * @file discovery_manager.cpp
 * @brief Initial discovery implementation.
 * */

#include "discovery/discovery_manager.hpp"
#include "loader/network_config.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "network_config.hpp"

#include "api/netlink/sysfs.hpp"
#include "api/acl.hpp"
#include "api/switch_info.hpp"
#include "api/static_mac.hpp"
#include "utils/utils.hpp"

#include <safe-string/safe_lib.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

using namespace agent::network;
using namespace agent::network::loader;
using namespace agent::network::api::netlink;
using namespace agent::network::discovery;
using namespace agent::network::utils;

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;

using std::string;

namespace {
    constexpr uint32_t SERIAL_DEFAULT_BITRATE = 115200;
    constexpr uint32_t SERIAL_DEFAULT_DATABITS = 8;
    constexpr uint32_t SERIAL_DEFAULT_STOPBIT = 1;
    constexpr uint32_t SWITCH_DEFAULT_LOCATION = 1;
    static constexpr char MESH_PORT_ENV_NAME[] = "MESH_PORTS";

    static void update_manager_info(const string& uuid) {
        /* get module reference */
        auto manager = CommonComponents::get_instance()->
                get_module_manager().get_entry_reference(uuid);
        /* update serial information */
        SerialConsole serial{manager->get_serial_console()};
        serial.set_bitrate(SERIAL_DEFAULT_BITRATE);
        serial.set_data_bits(SERIAL_DEFAULT_DATABITS);
        serial.set_flow_control(SerialConsoleFlowControl::None);
        serial.set_parity(SerialConsoleParity::None);
        serial.set_pin_out(SerialConsolePinOut::Cisco);
        serial.set_stop_bits(SERIAL_DEFAULT_STOPBIT);
        serial.set_signal_type(SerialConsoleSignalType::Rs232);
        /* add collections */
        manager->set_serial_console(std::move(serial));
        manager->add_collection({CollectionName::Switches,
                                CollectionType::EthernetSwitches, ""});
        manager->add_collection({CollectionName::Chassis,
                                CollectionType::Chassis, ""});
    }

    static string get_switch_uuid(const string& uuid) {
        auto uuids = NetworkComponents::get_instance()->
                                    get_switch_manager().get_keys(uuid);
        if (uuids.empty()) {
            throw std::runtime_error("No ethernet switches found");
        }
        return uuids.front();
    }

    static void update_switch_info(const string& switch_uuid) {
        /* get switch reference */
        auto switch_module = NetworkComponents::get_instance()->
                get_switch_manager().get_entry_reference(switch_uuid);
        /* TODO how do we know the location ? */
        switch_module->set_location(SWITCH_DEFAULT_LOCATION);
        switch_module->add_collection({CollectionName::EthernetSwitchPorts,
                                      CollectionType::EthernetSwitchPorts, ""});
        switch_module->add_collection({CollectionName::Neighbors,
                                      CollectionType::NeighborSwitches, ""});
        switch_module->add_collection({CollectionName::Acls,
                                      CollectionType::Acls, ""});
    }

    static inline bool is_mesh_port(const string& port_identifier) {
        auto env = std::getenv(MESH_PORT_ENV_NAME);
        if (!env) {
            return false;
        }
        std::stringstream ss{env};
        std::istream_iterator<std::string> eos{};
        std::istream_iterator<std::string> iit{ss};
        return (std::find(iit, eos, port_identifier) != eos);
    }

    static void add_physical_port(const string& switch_uuid,
            const string& port_identifier) {
        /* add port into network manager */
        auto network_components = NetworkComponents::get_instance();
        auto& port_manager = network_components->get_port_manager();
        EthernetSwitchPort port_model{switch_uuid};
        port_model.add_collection({CollectionName::Vlans,
                                  CollectionType::EthernetSwitchPortVlans, ""});
        port_model.add_collection({CollectionName::PortAcls,
                                  CollectionType::Acls, ""});
        port_model.add_collection({CollectionName::StaticMacs,
                                  CollectionType::StaticMacs, ""});
        port_model.set_link_technology(LinkTechnology::Ethernet);
        port_model.set_port_class(PortClass::Physical);
        port_model.set_port_type(is_mesh_port(port_identifier) ?
                                PortType::MeshPort : PortType::Upstream);
        port_model.set_port_mode(PortMode::Unknown);
        port_model.set_port_identifier(port_identifier);
        port_model.set_vlan_enable(true);
        port_model.set_status({State::Enabled, Health::OK});
        port_manager.add_entry(port_model);
    }

    static void discover_switch_ports(const string& switch_uuid) {
        SysFs sysfs{};

        /* discover physical & logical ports */
        auto network_components = NetworkComponents::get_instance();
        auto& port_manager = network_components->get_port_manager();
        try {
            /* physical ports */
            const auto& port_list = sysfs.get_port_list();
            for (const auto& port_identifier : port_list) {
                string port_uuid{};
                if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                    log_debug(GET_LOGGER("network-agent"), "Updating port " +
                                                                port_identifier);
                    auto port = port_manager.get_entry_reference(port_uuid);
                    /* add port collections */
                    port->add_collection({CollectionName::Vlans,
                                         CollectionType::EthernetSwitchPortVlans, ""});
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
                    log_debug(GET_LOGGER("network-agent"), "Adding physical port " +
                                                                port_identifier);
                    add_physical_port(switch_uuid, port_identifier);
                }
            }
            /* logical ports */
            const auto lag_list = sysfs.get_team_list();
            for (const auto& lag_identifier : lag_list) {
                log_debug(GET_LOGGER("network-agent"), "Adding LAG " + lag_identifier);
                add_logical_port(lag_identifier, switch_uuid);
            }
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("network-agent"), e.what());
            throw;
        }
    }

    static inline void init_forward_port (const string port_identifier,
                                          AclRule& model_rule) {
        if (!port_identifier.empty()) {
            string port_uuid{};
            if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                model_rule.set_forward_mirror_port(port_uuid);
            }
        }
    }

    static inline void init_mirrored_ports (const api::Acl::Strings& ports,
                                            AclRule& model_rule) {
        AclRule::MirroredPorts port_uuids{};
        for (const auto& port_identifier : ports) {
            string port_uuid{};
            if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
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
        if (EOK != memcpy_s(octets, ETH_ALEN, &condition.value, ETH_ALEN)) {
            throw std::runtime_error("Cannot copy memory with MAC address.");
        }
        std::reverse(octets, octets + ETH_ALEN);
        mac.set_address(api::SwitchInfo::format_mac_address(octets));
        if (condition.mask) {
            if (EOK != memcpy_s(octets, ETH_ALEN, &condition.mask, ETH_ALEN)) {
                throw std::runtime_error("Cannot copy memory with MAC address mask.");
            };
            std::reverse(octets, octets + ETH_ALEN);
            mac.set_mask(api::SwitchInfo::format_mac_address(octets));
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
                                      AclRule& model_rule) {
        model_rule.set_rule_id(rule.get_rule_id());
        init_acl_action(rule, model_rule);
        init_forward_port(rule.get_destination_port(), model_rule);
        init_mirrored_ports(rule.get_mirrored_ports(), model_rule);
        init_acl_mirror_type(rule, model_rule);
        init_acl_conditions(rule, model_rule);
    }

    static inline void init_acl_rules (const string& acl_uuid,
                                       const api::Acl::Rules& rules) {
        auto network_manager = NetworkComponents::get_instance();
        auto& acl_rule_manager = network_manager->get_acl_rule_manager();
        for (const auto& rule : rules) {
            AclRule model_rule{acl_uuid};
            init_acl_rule(rule, model_rule);
            model_rule.set_status({enums::State::Enabled, enums::Health::OK});
            acl_rule_manager.add_entry(std::move(model_rule));
        }
    }

    static inline void read_acl_rules(const string& acl_uuid) {
        api::Acl acl{};

        try {
            acl.read_acl_rule_list(api::Acl::get_acl_name(acl_uuid));
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("fm10000"),
                      string("Failed reading ACL rule list from switch: ") +
                      e.what());
            return;
        }
        init_acl_rules(acl_uuid, acl.get_rules());
    }

    static inline void init_acls_with_ports (const string& switch_uuid,
                                             api::Acl& acl) {
        auto network_manager = NetworkComponents::get_instance();
        auto& acl_manager = network_manager->get_acl_manager();
        auto& acl_port_manager = network_manager->get_acl_port_manager();
        for (const auto& acl_name : acl.get_acls()) {
            Acl acl_model{switch_uuid};
            const auto acl_uuid = acl_model.get_uuid();
            acl_model.set_status({enums::State::Enabled,
                                 enums::Health::OK});
            acl_model.add_collection({CollectionName::AclRules,
                                      CollectionType::Rules, ""});
            acl_model.add_collection({CollectionName::AclPorts,
                                      CollectionType::EthernetSwitchPorts, ""});
            acl_model.set_name(acl_name);
            // keep ACL UUID -> ACL name mapping
            api::Acl::add_acl_name(acl_uuid, acl_name);
            acl_manager.add_entry(acl_model);
            for (const auto& port_identifier : acl.get_bound_ports(acl_name)) {
                string port_uuid{};
                if (get_port_uuid_by_identifier(port_identifier, port_uuid)) {
                    acl_port_manager.add_entry(acl_uuid, port_uuid, "");
                }
            }
            read_acl_rules(acl_uuid);
        }
    }

    static void discover_acls(const string& switch_uuid) {
        api::Acl acl{};
        try {
            acl.read_acl_list();
        }
        catch (const std::runtime_error& e) {
            log_error(GET_LOGGER("network-agent"),
                      string("Failed reading ACL list from switch: ") + e.what());
            return;
        }
        init_acls_with_ports(switch_uuid, acl);
    }

    void add_static_macs_to_model(const string& port_uuid,
                                  const api::StaticMac::Table& macs) {
        auto network_manager = NetworkComponents::get_instance();
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
        auto network_manager = NetworkComponents::get_instance();
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

    static void discover_static_macs(const string& switch_uuid) {
        auto network_manager = NetworkComponents::get_instance();
        auto& port_manager = network_manager->get_port_manager();
        for (const auto& port_uuid : port_manager.get_keys(switch_uuid)) {
            ::init_static_macs_on_port(port_uuid);
        }
    }

    static void discover_switch_vlan_ports(const string&) {
        /* add port-vlan pair to the network manager */
        auto network_components = NetworkComponents::get_instance();
        auto& port_manager = network_components->get_port_manager();
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

    static void show_network_modules() {
        auto network_components = NetworkComponents::get_instance();
        auto common_components = CommonComponents::get_instance();

        auto& module_manager = common_components->get_module_manager();
        auto& switch_manager = network_components->get_switch_manager();
        auto& port_manager = network_components->get_port_manager();
        auto& port_vlan_manager = network_components->get_port_vlan_manager();
        for (const auto& uuid : module_manager.get_keys()) {
            log_debug(GET_LOGGER("network-agent"), "Manager UUID: " + uuid);
            for (const auto& switch_uuid : switch_manager.get_keys(uuid)) {
                log_debug(GET_LOGGER("network-agent"), "Switch UUID: " + switch_uuid);
                for (const auto& port_uuid : port_manager.get_keys(switch_uuid)) {
                    log_debug(GET_LOGGER("network-agent"), "Port UUID: " + port_uuid);
                    for (const auto& port_vlan_uuid :
                                        port_vlan_manager.get_keys(port_uuid)) {
                        log_debug(GET_LOGGER("network-agent"), "Port-vlan UUID: "
                                                                + port_vlan_uuid);
                    }
                }
            }
        }
    }

}

DiscoveryManager::DiscoveryManager() {}
DiscoveryManager::~DiscoveryManager() {}

void DiscoveryManager::wait_for_complete() {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_cv.wait(lock);
}

void DiscoveryManager::discovery(const std::string& uuid) {
    /* start discovery */
    ::update_manager_info(uuid);
    auto switch_uuid = get_switch_uuid(uuid);
    ::update_switch_info(switch_uuid);
    ::discover_switch_ports(switch_uuid);
    ::discover_switch_vlan_ports(switch_uuid);
    ::discover_acls(switch_uuid);
    ::discover_static_macs(switch_uuid);
    ::show_network_modules();

    /* notify discovery done */
    m_cv.notify_all();
}
